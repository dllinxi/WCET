# Do not change this order if you don't know what you are doing.
AUTOMAKE_OPTIONS = 1.7 gnu
SUBDIRS = netboot stage2 stage1 lib util docs
if PLATFORM_EFI
SUBDIRS += efi
else
SUBDIRS += grub
endif
EXTRA_DIST = BUGS MAINTENANCE
