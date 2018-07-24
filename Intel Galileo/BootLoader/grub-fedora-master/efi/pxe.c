
#include <grub/efi/efi.h>
#include <grub/efi/api.h>
#include <grub/efi/misc.h>
#include <grub/misc.h>

#include <shared.h>
#include <stddef.h>

#include "pxe.h"
#include "dhcp.h"

/* Search path is:
 *
 * X86PC/UNDI/pxelinux/pxelinux.cfg/1902dcf5-7190-d811-bbd6-6ef21c690030
 * X86PC/UNDI/pxelinux/pxelinux.cfg/01-00-30-6e-f2-1c-69
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A103437
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A10343
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A1034
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A103
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A10
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A1
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0A
 * X86PC/UNDI/pxelinux/pxelinux.cfg/0
 * X86PC/UNDI/pxelinux/pxelinux.cfg/default
 *
 * The paths we get from uefi are like:
 * .BootBootFile: X86PC/UNDI/pxelinux/bootx64.efi
 * .BootCiAddr: 0.0.0.0
 * .BootYiAddr: 10.16.52.158
 * .BootSiAddr: 10.16.52.16
 */

typedef struct {
	char *options;
	EFI_DHCP4_PACKET_OPTION *current_option;
} dhcp_option_parser;

static void dhcp_option_parser_reset(dhcp_option_parser *parser,
				EFI_PXE_BASE_CODE_PACKET *packet)
{
	char *addr;

	addr = (char *)packet;
	addr += offsetof(EFI_PXE_BASE_CODE_DHCPV4_PACKET, DhcpOptions);
	parser->current_option = (void *)addr;
	parser->options = (void *)addr;
}

static int dhcp_option_parser_next(dhcp_option_parser *parser,
				EFI_DHCP4_PACKET_OPTION **option)
{
	char *current_option;
	if (parser->current_option->OpCode == 255) {
		*option = NULL;
		return 0;
	}
	current_option = (char *)parser->current_option;
	current_option += 2 + parser->current_option->Length;
	parser->current_option = (EFI_DHCP4_PACKET_OPTION *)current_option;

	*option = parser->current_option;
	return 1;
}

#define DHCPMAGIK "\x63\x82\x53\x63"

static int get_dhcp_client_id(EFI_PXE_BASE_CODE_PACKET *packet, uuid_t *uuid)
{
	dhcp_option_parser parser;
	EFI_DHCP4_PACKET_OPTION *option;

	dhcp_option_parser_reset(&parser, packet);

	if (memcmp((char *)&packet->Dhcpv4.DhcpMagik, DHCPMAGIK, 4))
		return 0;

	while (dhcp_option_parser_next(&parser, &option)) {
		int i;
		char data[option->Length];

		if (option->OpCode != 97)
			continue;

		if (option->Length != 17)
			continue;

		memcpy(data, option->Data, option->Length);
		if (data[0] != 0)
			continue;

		/* 97[17]: 009cfe245ed0c8bd45a79f54ea5fbd3d97
		 *                               ^^^^^^^^^^^^ uint8_t[]
		 *                             ^^ uint8_t
		 *                           ^^ uint8_t
		 *                       ^^^^ BE uint16_t
		 *                   ^^^^ BE uint16_t
		 *           ^^^^^^^^ BE uint32_t
		 *         ^^ "type".  0 means UUID.
		 */
		memcpy(uuid, data+1, 16);
		uuid->time_low = htonl(uuid->time_low);
		uuid->time_mid = htons(uuid->time_mid);
		uuid->time_hi_ver = htons(uuid->time_hi_ver);

		return 1;
	}
	return 0;
}

#if 0
static void grub_dump_dhcp_options(EFI_PXE_BASE_CODE_PACKET *packet)
{
	dhcp_option_parser parser;
	EFI_DHCP4_PACKET_OPTION *option;
	char hex[] = "0123456789abcdef";
	int i;
	int j = 0;

	dhcp_option_parser_reset(&parser, packet);

	if (memcmp((char *)&packet->Dhcpv4.DhcpMagik, DHCPMAGIK, 4))
		return;

	/* 54[4]: a0014301
	 * 51[4]: 00004506
	 * 1[4]: ffffff00
	 * 3[4]: a00143ef
	 * 6[8]: a001ff20a001ff30
	 * 15[48]: 96e6374716c6c6e226f637e2275646861647e236f6d60226f637e2275646861647e236f6d602275646861647e236f6d6
	 * 28[4]: a00143ff
	 * 40[10]: 275646861647e236f6d6
	 * 41[8]: a001ff20a001ff30
	 * 58[4]: 0000a203
	 * 59[4]: 0000944d
	 * this is the one we want:
	 * 97[17]: 009cfe245ed0c8bd45a79f54ea5fbd3d97
	 *                               ^^^^^^^^^^^^ in order
	 *                             ^^
	 *                           ^^
	 *                       ^^^^ out of order
	 *                   ^^^^ out of order
	 *           ^^^^^^^^ out of order
	 *         ^^ "type".  0 means UUID.
	 * 255[0]:
	 */
	while (dhcp_option_parser_next(&parser, &option)) {
		char data[option->Length + 1];

		memcpy(data, option->Data, option->Length);
		data[option->Length] = '\0';

		grub_printf("%d[%d]: ", option->OpCode, option->Length);
		for (i = 0; i < option->Length; i++) {
			grub_printf("%c%c", hex[data[i] & 0xf],
					hex[(data[i] & 0xf0) >> 4]);
		}
		printf("\n");
	}

}

void grub_print_dhcp_info(grub_efi_loaded_image_t *loaded_image)
{
	EFI_PXE_BASE_CODE *pxe = NULL;
	EFI_PXE_BASE_CODE_PACKET *packet;

	grub_printf("got to %s\n", __func__);

	pxe = grub_efi_locate_protocol(&PxeBaseCodeProtocol, NULL);
	if (pxe == NULL)
		return;

	printf("DhcpDiscover options:\n");
	packet = (EFI_PXE_BASE_CODE_PACKET *)&pxe->Mode->DhcpDiscover.Dhcpv4;
	grub_dump_dhcp_options(packet);

	printf("DhcpAck options:\n");
	packet = (EFI_PXE_BASE_CODE_PACKET *)&pxe->Mode->DhcpAck.Dhcpv4;
	grub_dump_dhcp_options(packet);

	printf("PxeDiscover options:\n");
	packet = (EFI_PXE_BASE_CODE_PACKET *)&pxe->Mode->PxeDiscover.Dhcpv4;
	grub_dump_dhcp_options(packet);

	printf("PxeReply options:\n");
	packet = (EFI_PXE_BASE_CODE_PACKET *)&pxe->Mode->PxeReply.Dhcpv4;
	grub_dump_dhcp_options(packet);

#if 0
	printf("pxe->Mode->DhcpAck.Dhcpv4: \n");
	printf("\t.BootSrvName: %s\n", pxe->Mode->DhcpAck.Dhcpv4.BootpSrvName);
	printf("\t.BootBootFile: %s\n", pxe->Mode->DhcpAck.Dhcpv4.BootpBootFile);
	printf("\t.BootCiAddr: %d.%d.%d.%d\n",
		pxe->Mode->DhcpAck.Dhcpv4.BootpCiAddr[0],
		pxe->Mode->DhcpAck.Dhcpv4.BootpCiAddr[1],
		pxe->Mode->DhcpAck.Dhcpv4.BootpCiAddr[2],
		pxe->Mode->DhcpAck.Dhcpv4.BootpCiAddr[3]);
	printf("\t.BootYiAddr: %d.%d.%d.%d\n",
		pxe->Mode->DhcpAck.Dhcpv4.BootpYiAddr[0],
		pxe->Mode->DhcpAck.Dhcpv4.BootpYiAddr[1],
		pxe->Mode->DhcpAck.Dhcpv4.BootpYiAddr[2],
		pxe->Mode->DhcpAck.Dhcpv4.BootpYiAddr[3]);
	printf("\t.BootSiAddr: %d.%d.%d.%d\n",
		pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr[0],
		pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr[1],
		pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr[2],
		pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr[3]);
	printf("\t.BootGiAddr: %d.%d.%d.%d\n",
		pxe->Mode->DhcpAck.Dhcpv4.BootpGiAddr[0],
		pxe->Mode->DhcpAck.Dhcpv4.BootpGiAddr[1],
		pxe->Mode->DhcpAck.Dhcpv4.BootpGiAddr[2],
		pxe->Mode->DhcpAck.Dhcpv4.BootpGiAddr[3]);
	}
	printf("\n");
#endif


}
#endif

static void icmp_print_error(EFI_PXE_BASE_CODE *pxe)
{
	EFI_PXE_BASE_CODE_ICMP_ERROR *err = &pxe->Mode->IcmpError;
	int i;
	//char hex[] = "0123456789abcdef";

	printf("icmp error\n");
	printf("type: %d code: %d\n", err->Type, err->Code);
	printf("data: \n");
	for(i = 0; i < 464; i+=16) {
		int x;
		for (x = i; x < i+4; x++)
			printf("%02x ", err->Data[x]);
		printf(" ");
		for (x = i+4; x < i+8; x++)
			printf("%02x ", err->Data[x]);
		printf(" ");
		printf(" ");
		for (x = i+8; x < i+12; x++)
			printf("%02x ", err->Data[x]);
		printf(" ");
		for (x = i+12; x < i+16; x++)
			printf("%02x ", err->Data[x]);
		printf("\n");
	}
}

static int grub_efi_pxe_check_for_file(
	EFI_PXE_BASE_CODE *pxe,
	EFI_IP_ADDRESS *ServerIp,
	char *BootpBootFile,
	char *configname,
	char **returnpath)
{
	size_t bplen = strlen(BootpBootFile);
	char *Filename = grub_malloc(24 + bplen + 40);
	char *lastslash = Filename + bplen;
	grub_efi_uintn_t size;
	int i;
	EFI_STATUS rc;
	char Buffer[8192];

	memcpy(Filename, BootpBootFile, bplen);

	for (i = 0; i < bplen; i++) {
		if (Filename[i] == '/')
			lastslash = Filename + i;
	}
	if (*lastslash) {
		*lastslash++ = '/';
		*lastslash = '\0';
	}

	sprintf(lastslash, configname);

	printf("tftp://%d.%d.%d.%d/%s\n",
		ServerIp->v4.Addr[0], ServerIp->v4.Addr[1],
		ServerIp->v4.Addr[2], ServerIp->v4.Addr[3],
		Filename);

	rc = tftp_get_file_size(Filename, &size);
	if (rc == GRUB_EFI_ICMP_ERROR)
		icmp_print_error(pxe);

	if (rc == GRUB_EFI_SUCCESS) {
		*returnpath = Filename;
		return size;
	}
	grub_free(Filename);
	return 0;
}

static void get_pxe_server(EFI_PXE_BASE_CODE *pxe, EFI_IP_ADDRESS **Address)
{
	EFI_IP_ADDRESS *tmp = grub_malloc(sizeof *tmp);
	if (tmp) {
		memset(tmp, '\0', sizeof (*tmp));
		memcpy(&tmp->Addr[0], pxe->Mode->DhcpAck.Dhcpv4.BootpSiAddr, 4);
		*Address = tmp;
	}
}

static char *get_pxe_file_dir(EFI_PXE_BASE_CODE *pxe)
{
	char *FileDir = NULL;
	char *DirEnd = NULL;
	char *BootpBootFile;
	size_t bplen;

	BootpBootFile = pxe->Mode->DhcpAck.Dhcpv4.BootpBootFile;
	bplen = strlen(BootpBootFile);
	FileDir = grub_malloc(bplen + 1);
	memcpy(FileDir, BootpBootFile, bplen);
	FileDir[bplen] = '\0';

	DirEnd = grub_strrchr(FileDir, '/');
	if (!DirEnd)
		DirEnd = FileDir;

	*DirEnd = '\0';

	return FileDir;
}

static void set_pxe_info(grub_efi_loaded_image_t *LoadedImage,
			EFI_PXE_BASE_CODE *pxe)
{
	tftp_info.LoadedImage = LoadedImage;
	tftp_info.Pxe = pxe;
	get_pxe_server(pxe, &tftp_info.ServerIp);
	tftp_info.BasePath = get_pxe_file_dir(pxe);
}

char *grub_efi_pxe_get_config_path(grub_efi_loaded_image_t *LoadedImage)
{
	EFI_PXE_BASE_CODE *pxe = NULL;
	EFI_IP_ADDRESS ServerIp;
	char *FileName = NULL;
	EFI_PXE_BASE_CODE_DHCPV4_PACKET *packet;
	uuid_t uuid;
	grub_efi_uintn_t FileSize = 0;
	grub_efi_status_t rc = GRUB_EFI_SUCCESS;
	char *ConfigPath = NULL;
	char hex[] = "0123456789ABCDEF";
	char hexip[9];
	int hexiplen;

	grub_efi_handle_t *handle, *handles;
	grub_efi_uintn_t num_handles;

	handles = grub_efi_locate_handle(GRUB_EFI_BY_PROTOCOL,
					&PxeBaseCodeProtocol,
					NULL, &num_handles);

	if (!handles)
	  return NULL;

	for (handle = handles; num_handles--; handle++) {
		pxe = grub_efi_open_protocol(*handle, &PxeBaseCodeProtocol,
			GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
		if (!pxe || !pxe->Mode)
			continue;
		if (pxe->Mode->Started && pxe->Mode->DhcpAckReceived)
			break;
	}
	grub_free(handles);

	if (!pxe)
		return NULL;

	set_pxe_info(LoadedImage, pxe);

	FileName = grub_malloc(strlen("1902dcf5-7190-d811-bbd6-6ef21c690030"));

	packet = &pxe->Mode->DhcpDiscover.Dhcpv4;

	if (get_dhcp_client_id((EFI_PXE_BASE_CODE_PACKET *)packet, &uuid)) {

		uuid.time_mid = 0x0011;
		sprintf(FileName,
			"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			uuid.time_low, uuid.time_mid, uuid.time_hi_ver,
			uuid.clock_seq_hi, uuid.clock_seq_low,
			uuid.node[0], uuid.node[1], uuid.node[2],
			uuid.node[3], uuid.node[4], uuid.node[5]);

		rc = tftp_get_file_size(FileName, &FileSize);
		if (rc == GRUB_EFI_SUCCESS) {
			char *ReturnFile = grub_malloc(strlen("(nd)/") +
						strlen(FileName) + 1);
			sprintf(ReturnFile, "(nd)/%s", FileName);
			grub_free(FileName);
			//sprintf(tftp_info.LastPath, FileName);
			return ReturnFile;
		}
	}

	packet = &pxe->Mode->DhcpAck.Dhcpv4;

	if (!memcmp(packet->BootpHwAddr + 6, "\x00\x00\x00\x00\x00"
					     "\x00\x00\x00\x00\x00", 10) &&
			memcmp(packet->BootpHwAddr, "\x00\x00\x00\x00\x00\x00",
				6)) {
		char mac[21];
		sprintf(mac, "01-%c%c-%c%c-%c%c-%c%c-%c%c-%c%c",
			hex[(packet->BootpHwAddr[0] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[0] & 0xf],
			hex[(packet->BootpHwAddr[1] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[1] & 0xf],
			hex[(packet->BootpHwAddr[2] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[2] & 0xf],
			hex[(packet->BootpHwAddr[3] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[3] & 0xf],
			hex[(packet->BootpHwAddr[4] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[4] & 0xf],
			hex[(packet->BootpHwAddr[5] & 0xf0) >> 4],
			hex[packet->BootpHwAddr[5] & 0xf]);

		rc = tftp_get_file_size(mac, &FileSize);
		if (rc == GRUB_EFI_SUCCESS) {
			char *ReturnFile = grub_malloc(strlen("(nd)/") +
						strlen(mac) + 1);
			sprintf(ReturnFile, "(nd)/%s", mac);
			return ReturnFile;
		}

	}

	sprintf(hexip, "%c%c%c%c%c%c%c%c",
		hex[(packet->BootpYiAddr[0] & 0xf0) >> 4],
		hex[packet->BootpYiAddr[0] & 0xf],
		hex[(packet->BootpYiAddr[1] & 0xf0) >> 4],
		hex[packet->BootpYiAddr[1] & 0xf],
		hex[(packet->BootpYiAddr[2] & 0xf0) >> 4],
		hex[packet->BootpYiAddr[2] & 0xf],
		hex[(packet->BootpYiAddr[3] & 0xf0) >> 4],
		hex[packet->BootpYiAddr[3] & 0xf]);

	for (hexiplen = strlen(hexip); hexiplen > 0; hexiplen--)
	{
		hexip[hexiplen] = '\0';
		rc = tftp_get_file_size(hexip, &FileSize);
		if (rc == GRUB_EFI_SUCCESS) {
			char *ReturnFile = grub_malloc(strlen("(nd)/") +
						strlen(hexip) + 1);
			sprintf(ReturnFile, "(nd)/%s", hexip);
			return ReturnFile;
		}
	}

	rc = tftp_get_file_size("efidefault", &FileSize);
	if (rc == GRUB_EFI_SUCCESS) {
		char *ReturnFile = grub_malloc(strlen("(nd)/efidefault")+1);
		sprintf(ReturnFile, "(nd)/efidefault");
		return ReturnFile;
	}

	return NULL;
}
