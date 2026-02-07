[BITS 16]
[ORG 0x1000]

start_loader:
    ; Marqueur 'L' (Loader 16 bits)
    mov [0xb8004], word 0x0e4c 

    ; Activer A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Charger GDT
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:init_pm

[BITS 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    
    ; Setup Paging (Identity map 2MB)
    mov edi, 0x3000
    push edi
    mov ecx, 3072
    xor eax, eax
    rep stosb
    pop edi

    mov dword [edi], 0x4003      ; PML4 -> PDP
    mov dword [edi + 0x1000], 0x5003 ; PDP -> PD
    mov dword [edi + 0x2000], 0x0083 ; PD -> 2MB Page

    mov cr3, edi
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_descriptor]
    jmp 0x08:init_lm

[BITS 64]
init_lm:
    ; --- AJOUT ICI ---
    mov rsp, 0x90000    ; Définit une pile sécurisée loin du kernel
    ; -----------------

    ; Marqueur '6'
    mov rax, 0x0e360e360e360e36
    mov [0xb8006], rax

    mov rax, 0x20000
    jmp rax

; GDTs
gdt_start: dq 0
gdt_code: dq 0x00CF9A000000FFFF
gdt_data: dq 0x00CF92000000FFFF
gdt_end:
gdt_descriptor: dw gdt_end - gdt_start - 1
                dd gdt_start

gdt64_start: dq 0
gdt64_code: dq 0x0020980000000000
gdt64_data: dq 0x0000920000000000
gdt64_end:
gdt64_descriptor: dw gdt64_end - gdt64_start - 1
                  dd gdt64_start

times 2048-($-$$) db 0