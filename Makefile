# Toolchain
CC = cc
LD = ld
AS = nasm

# C compiler flags
CFLAGS = \
	-g \
	-O0 \
	-pipe \
	-Wall \
	-Wextra \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fPIE \
    -m64 \
    -march=x86-64 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-80387 \
    -mno-red-zone \
	-fsanitize=undefined
	
# C preprocessor flags
CPPFLAGS = \
	-I src \
	-MMD \
	-MP

# Linker flags
LDFLAGS = \
	-m elf_x86_64 \
    -nostdlib \
    -static \
    -pie \
    --no-dynamic-linker \
    -z text \
    -z max-page-size=0x1000 \
    -T config/linker.ld

# NASM flags
NASMFLAGS = \
	-Wall \
	-w-reloc-rel-dword \
	-w-reloc-abs-qword \
    -f elf64

# QEMU flags
QEMUFLAGS = \
	-m 4G \
	-debugcon stdio \
	-cdrom $(IMAGE_NAME).iso \
	-boot d \
	-no-reboot -no-shutdown \
#	-s -S

# Source files
CFILES := $(shell cd src && find -L * -type f -name '*.c')
ASFILES := $(shell cd src && find -L * -type f -name '*.S')
NASMFILES := $(shell cd src && find -L * -type f -name '*.asm')
HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Output
OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
KERNEL = alpine
IMAGE_NAME = alpine

all: limine bin/$(KERNEL) iso run

all-kvm: limine bin/$(KERNEL) iso run-kvm

run:
	qemu-system-x86_64 $(QEMUFLAGS)

run-kvm:
	qemu-system-x86_64 $(QEMUFLAGS) -enable-kvm

iso:
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -va bin/. iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp -v config/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

bin/$(KERNEL): Makefile config/linker.ld $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) $(LDFLAGS) -o $@
    
obj/%.c.o: src/%.c Makefile src/limine.h
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.S.o: src/%.S Makefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.asm.o: src/%.asm Makefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

src/limine.h:
	curl -Lo src/limine.h https://github.com/limine-bootloader/limine/raw/trunk/limine.h

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1
	make -C limine CC="cc" CFLAGS="-g -O2 -pipe" CPPFLAGS="" LDFLAGS="" LIBS=""

clean:
	rm -rf obj
	rm -f $(IMAGE_NAME).iso

distclean:
	rm -rf limine
	rm -f src/limine.h