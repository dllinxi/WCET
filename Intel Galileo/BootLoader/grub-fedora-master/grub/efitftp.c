#include <shared.h>
#include <filesys.h>

int efi_tftp_mount (void);
int efi_tftp_read (char *buf, int len);
int efi_tftp_dir (char *dirname);
void efi_tftp_close (void);

int
efi_tftp_mount (void)
{
	grub_printf("non-efi efi_tftp_mount()\n");
	return 0;
}

int
efi_tftp_read (char *addr, int size)
{
	grub_printf ("non-efi efi_tftp_read (0x%x, %d)\n", (long) addr, size);
	return 0;
}

int
efi_tftp_dir (char *dirname)
{
	grub_printf ("non-efi efi_ftp_dir (%s)\n", dirname);
	return 0;
}

void
efi_tftp_close (void)
{
	grub_printf ("non-efi efi_tftp_close ()\n");
}
