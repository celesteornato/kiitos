.PHONY: all clean debug dbgrun run archi
.SUFFIXES: .o .c .S .s .od .psf

WARNS=\
    -Wall\
    -Wextra\
    -Wpedantic\
    -Wunused\
    -Wunused\
    -Wfloat-equal\
    -Wundef\
    -Wshadow\
    -Wpointer-arith\
    -Wcast-align\
    -Wstrict-prototypes\
    -Wstrict-overflow=5\
    -Wwrite-strings\
    -Wcast-qual\
    -Wswitch-default\
    -Wswitch-enum\
    -Wconversion\
    -Wvla\
    -Wunreachable-code

CFLAGS+= \
	--std=c17 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-PIC \
	-m64 \
	-march=x86-64 \
 	-mno-mmx \
    -mno-80387 \
    -mno-sse \
    -mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel

LDFLAGS += \
	-Wl,-m,elf_x86_64 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -T src/linker.ld

OUTDIR=bin
BUILDDIR=build
EXENAME=kiitos2
EXE=$(OUTDIR)/$(EXENAME)

# BSD Make, GNU Make
SRCS=${:!find src -name '*.c'!}
SRCS+=$(wildcard src/*.c)
SRCS+=$(wildcard src/*/*.c)

ASMSRCS=${:!find src -name '*.S'!}
ASMSRCS+=$(wildcard src/*.S)
ASMSRCS+=$(wildcard src/*/*.S)

FONTSRCS=${:!find src -name '*.psf'!}
FONTSRCS+=$(wildcard src/assets/*.psf)

# BSD Make, GNU Make
OBJ=${SRCS:S/.c/.o/g}
OBJ+=$(patsubst %.c,%.o,$(SRCS))

OBJ+=${ASMSRCS:S/.S/.o/g}
OBJ+=$(patsubst %.S,%.o,$(ASMSRCS))

OBJ+=${FONTSRCS:S/.psf/.o/g}
OBJ+=$(patsubst %.psf,%.o,$(FONTSRCS))

OBJDBG=${SRCS:S/.c/.od/g}
OBJDBG+=$(patsubst %.c,%.od,$(SRCS))

OBJDBG+=${ASMSRCS:S/.S/.o/g}
OBJDBG+=$(patsubst %.S,%.o,$(ASMSRCS))

OBJDBG+=${FONTSRCS:S/.psf/.o/g}
OBJDBG+=$(patsubst %.psf,%.o,$(FONTSRCS))

DBGFLAGS=$(CFLAGS) $(WARNS) -g -O0

all: archi $(EXE)
debug: archi $(EXE).dbg

$(EXE): $(OBJ)
	cc $(CFLAGS) -O2 $(LDFLAGS) $(WARNS) $(OBJ) -o $(EXE)

$(EXE).dbg: $(OBJDBG)
	cc $(DBGFLAGS) $(LDFLAGS) $(OBJDBG) -o $(EXE).dbg
	rm -rf $(OBJDBG)

.c.od:
	cc $(DBGFLAGS) -c $< -o $@
.c.o:
	cc $(CFLAGS) $(WARNS) -c $< -o $@
.S.o:
	cc -c $< -o $@
.s.o:
	cc -c $< -o $@
.psf.o:
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@

limine/limine:
	git clone "https://github.com/limine-bootloader/limine.git" --branch=v9.x-binary --depth=1 limine
	make -C limine

$(OUTDIR)/image.iso: $(EXE) limine/limine
	mkdir -p $(BUILDDIR)/iso_root
	mkdir -p $(BUILDDIR)/iso_root/boot
	cp -v $(EXE) $(BUILDDIR)/iso_root/boot/
	mkdir -p $(BUILDDIR)/iso_root/boot/limine
	cp -v src/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
		limine/limine-uefi-cd.bin $(BUILDDIR)/iso_root/boot/limine/

	mkdir -p $(BUILDDIR)/iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI $(BUILDDIR)/iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI $(BUILDDIR)/iso_root/EFI/BOOT/

	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
			-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
			-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			$(BUILDDIR)/iso_root -o $(OUTDIR)/image.iso

	limine/limine bios-install $(OUTDIR)/image.iso

$(OUTDIR)/imagedbg.iso: $(EXE).dbg limine/limine
	mkdir -p $(BUILDDIR)/iso_root
	mkdir -p $(BUILDDIR)/iso_root/boot
	cp -v $(EXE).dbg $(BUILDDIR)/iso_root/boot/$(EXENAME)
	mkdir -p $(BUILDDIR)/iso_root/boot/limine
	cp -v src/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
		limine/limine-uefi-cd.bin $(BUILDDIR)/iso_root/boot/limine/

	mkdir -p $(BUILDDIR)/iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI $(BUILDDIR)/iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI $(BUILDDIR)/iso_root/EFI/BOOT/

	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
			-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
			-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			$(BUILDDIR)/iso_root -o $(OUTDIR)/imagedbg.iso

	limine/limine bios-install $(OUTDIR)/imagedbg.iso

run: $(OUTDIR)/image.iso
	qemu-system-x86_64 -D logs/qemu.log -d int,cpu_reset -M smm=off -no-reboot  -drive format=raw,file=$(OUTDIR)/image.iso
dbgrun: $(OUTDIR)/imagedbg.iso
	qemu-system-x86_64 -D logs/qemu.log -d int,cpu_reset -M smm=off -s -S -drive format=raw,file=$(OUTDIR)/imagedbg.iso

archi:
	mkdir -p bin logs

clean:
	rm -rf $(OUTDIR)/*
	rm -rf $(BUILDDIR)/*
	rm -f $(OBJ)
