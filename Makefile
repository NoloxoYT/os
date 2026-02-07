ASM=nasm
CC=gcc
LD=ld
OBJCOPY=objcopy

BUILD_DIR=build
SRC_DIR=src

CFLAGS=-ffreestanding -mno-red-zone -m64 -fno-pie -fno-leading-underscore \
       -fno-asynchronous-unwind-tables -fno-stack-protector -Isrc/kernel -c

all: $(BUILD_DIR)/os.img

$(BUILD_DIR)/mbr.bin: $(SRC_DIR)/boot/mbr.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

$(BUILD_DIR)/loader.bin: $(SRC_DIR)/boot/loader.asm
	$(ASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel/main.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/drivers.o: $(SRC_DIR)/kernel/drivers.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.o $(BUILD_DIR)/drivers.o $(BUILD_DIR)/nvim.o
	$(LD) -T linker.ld $^ -o $(BUILD_DIR)/kernel.tmp
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel.tmp $@

$(BUILD_DIR)/nvim.o: $(SRC_DIR)/apps/nvim.c
	$(CC) $(CFLAGS) $< -o $@
$(BUILD_DIR)/os.img: $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@
	# On s'assure que le fichier est assez gros pour le BIOS
	truncate -s 64k $@
clean:
	rm -rf $(BUILD_DIR)