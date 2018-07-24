
pkglibdir = $(libdir)/$(PACKAGE)/$(host_cpu)-$(host_vendor)
pkgdatadir = $(datadir)/$(PACKAGE)/$(host_cpu)-$(host_vendor)

if PLATFORM_EFI

if NETBOOT_SUPPORT
NETBOOT_FLAGS = -I$(top_srcdir)/netboot -DSUPPORT_NETBOOT=1
else
NETBOOT_FLAGS =
endif

if SERIAL_SUPPORT
SERIAL_FLAGS = -DSUPPORT_SERIAL=1
else
SERIAL_FLAGS =
endif

if HERCULES_SUPPORT
HERCULES_FLAGS = -DSUPPORT_HERCULES=1
else
HERCULES_FLAGS =
endif

if GRAPHICS_SUPPORT
GRAPHICS_FLAGS = -DSUPPORT_GRAPHICS=1
else
GRAPHICS_FLAGS =
endif

GRUBEFI_FORMAT = efi-app-$(EFI_ARCH)
GRUBSO_LD_SCRIPT = @GNUEFI_LDS@
GRUBSO_LD_FLAGS = -T $(GRUBSO_LD_SCRIPT) -nostdlib -shared -Bsymbolic

GRUBSO_OBJS = efimain.o
GRUBSO_LIBS = @GNUEFI_CRT0@ $(top_srcdir)/stage2/libstage2.a \
		libgrubefi.a @LIBGCC@

if NETBOOT_SUPPORT
GRUBSO_LIBS += $(top_srcdir)/netboot/libdrivers.a
endif

pkgdata_DATA = grub.efi

grub.efi: grub.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel \
                   -j .rela -j .reloc --target=$(GRUBEFI_FORMAT) $^ $@

grub.so: $(GRUBSO_OBJS) $(GRUBSO_LIBS) @LIBGNUEFI@
	$(LD) -o $@ $(GRUBSO_LD_FLAGS) $^
	echo '-------------- unresolved symbols ---------------------'
	! nm $@ | grep -iw u
	echo '-------------------------------------------------------'

crt0-efi.o: $(EFI_ARCH)/crt0-efi.S
	$(CC) -o $@ -c $(libgrubefi_a_CFLAGS) $^

efimain.o: efimain.c
	$(CC) -o $@ -c $(libgrubefi_a_CFLAGS) $^

clean-local:
	-rm -rf grub.so grub.efi

RELOC_FLAGS = $(STAGE2_CFLAGS) -I$(top_srcdir)/stage1 \
	-I$(top_srcdir)/lib -I. -I$(top_srcdir) -I$(top_srcdir)/stage2 \
	-fno-builtin $(NETBOOT_FLAGS) $(SERIAL_FLAGS) \
	$(HERCULES_FLAGS) $(GRAPHICS_FLAGS)

noinst_LIBRARIES = libgrubefi.a
libgrubefi_a_SOURCES = $(EFI_ARCH)/callwrap.S eficore.c efimm.c efimisc.c \
	eficon.c efidisk.c graphics.c efigraph.c efiuga.c efidp.c \
	font_8x16.c efiserial.c $(EFI_ARCH)/loader/linux.c efichainloader.c \
	xpm.c pxe.c efitftp.c
libgrubefi_a_CFLAGS = $(RELOC_FLAGS) -nostdinc

endif
