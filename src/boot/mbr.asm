[BITS 16]
[ORG 0x7C00]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Marqueur visuel 'M' (MBR)
    mov [0xb8000], word 0x0e4d 

    ; 1. Charger le Loader (Secteur 2, 4 secteurs) à 0x1000
    mov bx, 0x1000
    mov al, 4
    mov cl, 2
    call disk_load

    ; 2. Charger le Kernel (Secteur 6, 40 secteurs) à 0x20000
    ; 0x2000:0x0000 = 0x20000
    mov ax, 0x2000
    mov es, ax
    mov bx, 0x0000
    mov al, 60      ; On lit 40 secteurs (20 Ko)
    mov cl, 6
    call disk_load

    ; Marqueur visuel 'K' (Kernel chargé)
    mov [0xb8002], word 0x0a4b 

    jmp 0x0000:0x1000

disk_load:
    mov ah, 0x02
    mov ch, 0
    mov dh, 0
    mov dl, 0x80
    int 0x13
    jc disk_error
    ret

disk_error:
    mov [0xb8000], word 0x0c45 ; 'E' rouge en cas d'erreur
    hlt

times 510-($-$$) db 0
dw 0xAA55