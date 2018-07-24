#include <grub/efi/efi.h>
#include <grub/efi/api.h>
#include <grub/efi/misc.h>
#include <grub/misc.h>

#include <shared.h>
#include <filesys.h>
#include "pxe.h"

struct tftp_info tftp_info = {
	.LoadedImage = NULL,
	.Pxe = NULL,
	.ServerIp = NULL,
	.BasePath = NULL
};

/*
 * CLIENT MAC ADDR: 00 15 17 4C E6 74
 * CLIENT IP: 10.16.52.158  MASK: 255.255.255.0  DHCP IP: 10.16.52.16
 * GATEWAY IP: 10.16.52.254
 *
 * TSize.Running LoadFile()
 *
 * TFTP.status: 5
 * got to grub_efi_pxe_get_config_path
 * SiAddr: 10.16.52.16
 * BootpHwAddr: 00:15:17:4c:e6:74:00:00:00:00:00:00:00:00:00:00
 * BootpSrvName:
 * BootpBootFile: X86PC/UNDI/pxelinux/bootx64.efi
 */

static grub_efi_status_t tftp_get_file_size_defective_buffer_fallback(
	char *Filename,
	grub_efi_uintn_t *Size)
{
	EFI_PXE_BASE_CODE_TFTP_OPCODE OpCode = EFI_PXE_BASE_CODE_TFTP_READ_FILE;
	char *Buffer = NULL;
	grub_efi_boolean_t Overwrite = 0;
	grub_efi_boolean_t DontUseBuffer = 0;
	grub_efi_uint64_t BufferSize = 4096;
	grub_efi_uintn_t BlockSize = 512;
	grub_efi_status_t rc = GRUB_EFI_BUFFER_TOO_SMALL;
	char *FullPath = NULL;

	while (rc == GRUB_EFI_BUFFER_TOO_SMALL) {
		char *NewBuffer;

		if (Buffer) {
			grub_free(Buffer);
			Buffer = NULL;
		}
		BufferSize *= 2;
		NewBuffer = grub_malloc(BufferSize);
		if (!NewBuffer)
			return GRUB_EFI_OUT_OF_RESOURCES;
		Buffer = NewBuffer;

		if (tftp_info.BasePath) {
			int PathSize = 0;
			PathSize = strlen(tftp_info.BasePath) + 2 +
				   strlen(Filename);
			FullPath = grub_malloc(PathSize);
			grub_sprintf(FullPath, "%s/%s", tftp_info.BasePath,
				     Filename);
		} else {
			FullPath = grub_malloc(strlen(Filename));
			strcpy(FullPath, Filename);
		}

		rc = Call_Service_10(tftp_info.Pxe->Mtftp, tftp_info.Pxe,
			OpCode, Buffer, Overwrite, &BufferSize, &BlockSize,
			tftp_info.ServerIp, FullPath, NULL, DontUseBuffer);
		if (rc == GRUB_EFI_SUCCESS || rc == GRUB_EFI_BUFFER_TOO_SMALL)
			*Size = BufferSize;
	}
	grub_free(FullPath);
	grub_free(Buffer);
	return rc;
}

grub_efi_status_t tftp_get_file_size(
	char *Filename,
	grub_efi_uintn_t *Size)
{
	EFI_PXE_BASE_CODE_TFTP_OPCODE OpCode = EFI_PXE_BASE_CODE_TFTP_GET_FILE_SIZE;
	char Buffer[8192];
	grub_efi_boolean_t Overwrite = 0;
	grub_efi_boolean_t DontUseBuffer = 0;
	grub_efi_uint64_t BufferSize = 8192;
	grub_efi_uintn_t BlockSize = 512;
	grub_efi_status_t rc;
	char *FullPath = NULL;

	if (tftp_info.BasePath) {
		int PathSize = 0;
		PathSize = strlen(tftp_info.BasePath) + 2 + strlen(Filename);
		FullPath = grub_malloc(PathSize);
		grub_sprintf(FullPath, "%s/%s", tftp_info.BasePath, Filename);
	} else {
		FullPath = grub_malloc(strlen(Filename));
		strcpy(FullPath, Filename);
	}

	rc = Call_Service_10(tftp_info.Pxe->Mtftp, tftp_info.Pxe, OpCode,
		Buffer, Overwrite, &BufferSize, &BlockSize, tftp_info.ServerIp,
		FullPath, NULL, DontUseBuffer);
	if (rc == GRUB_EFI_BUFFER_TOO_SMALL)
		rc = tftp_get_file_size_defective_buffer_fallback(Filename, Size);
	if (rc == GRUB_EFI_SUCCESS)
		*Size = BufferSize;
	grub_free(FullPath);
	return rc;
}

static grub_efi_status_t tftp_read_file(
	char *Filename,
	char *Buffer,
	grub_efi_uint64_t BufferSize)
{
	EFI_PXE_BASE_CODE_TFTP_OPCODE OpCode = EFI_PXE_BASE_CODE_TFTP_READ_FILE;
	grub_efi_boolean_t Overwrite = 0;
	grub_efi_boolean_t DontUseBuffer = 0;
	grub_efi_uintn_t BlockSize = 512;
	grub_efi_status_t rc;
	char *FullPath = NULL;

	if (tftp_info.BasePath) {
		int PathSize = 0;
		PathSize = strlen(tftp_info.BasePath) + 2 + strlen(Filename);
		FullPath = grub_malloc(PathSize);
		grub_sprintf(FullPath, "%s/%s", tftp_info.BasePath, Filename);
	} else {
		FullPath = grub_malloc(strlen(Filename));
		strcpy(FullPath, Filename);
	}

	rc = Call_Service_10(tftp_info.Pxe->Mtftp, tftp_info.Pxe, OpCode,
		Buffer, Overwrite, &BufferSize, &BlockSize, tftp_info.ServerIp,
		FullPath, NULL, DontUseBuffer);
	grub_free(FullPath);
	return rc;
}

int
efi_tftp_mount (void)
{
	if (current_drive != NETWORK_DRIVE) {
		return 0;
	}
	return 1;
}

int
efi_tftp_read (char *addr, int size)
{
	int rc;

	if (tftp_info.LastPath == NULL) {
		grub_printf(" = 0 (no path known)\n");
		return 0;
	}
	if (tftp_info.Buffer == NULL) {
		grub_printf(" = 0 (no file open)\n");
		return 0;
	}
	if (filemax == -1) {
		grub_printf(" = 0 (file not found)\n");
		return 0;
	}
	if (filepos == 0) {
		rc = tftp_read_file(tftp_info.LastPath, tftp_info.Buffer,
				    filemax);
	}

	grub_memmove(addr, tftp_info.Buffer+filepos, size);
	filepos += size;

	return size;
}

int
efi_tftp_dir (char *dirname)
{
	int rc;
	int ch;
	grub_efi_uintn_t size;
	int len;
	char *name;

	ch = nul_terminate(dirname);
	len = strlen(dirname);

	name = grub_malloc(len + 1);
	grub_memmove(name, dirname, len);
	name[len] = '\0';
	dirname[len] = ch;

#if 0
	if (print_possibilities)
		return 1;
#endif

	filemax = -1;

	rc = tftp_get_file_size(name, &size);
	if (rc == GRUB_EFI_SUCCESS) {
		tftp_info.LastPath = grub_malloc(strlen(name) + 1);
		sprintf(tftp_info.LastPath, "%s", name);
		filemax = size;
		filepos = 0;

		tftp_info.Buffer = grub_malloc(filemax);

		return 1;
	}
	return 0;
}

void
efi_tftp_close (void)
{
	filepos = 0;
	filemax = -1;
	grub_free(tftp_info.LastPath);
	tftp_info.LastPath = NULL;
	grub_free(tftp_info.Buffer);
	tftp_info.Buffer = NULL;
}
