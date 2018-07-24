#ifndef DHCP_H
#define DHCP_H 1

#include "pxe.h"

#define EFI_DHCP4_PROTOCOL_GUID \
{ 0x8a219718, 0x4ef5, 0x4761, {0x91,0xc8,0xc0,0xf0,0x4b,0xda,0x9e,0x56} }
static grub_efi_guid_t DHCP4Protocol = EFI_DHCP4_PROTOCOL_GUID;

#define EFI_DHCP4_SERVICE_BINDING_PROTOCOL_GUID \
{ 0x9d9a39d8, 0xbd42, 0x4a73, {0xa4,0xd5,0x8e,0xe9,0x4b,0xe1,0x13,0x80} }
static grub_efi_guid_t DHCP4SbProtocol = EFI_DHCP4_SERVICE_BINDING_PROTOCOL_GUID;

#define EFI_PXE_DHCP4_PROTOCOL_GUID \
{ 0x03c4e624, 0xac28, 0x11d3, {0x9a,0x2d,0x00,0x90,0x29,0x3f,0xc1,0x4d} }
static grub_efi_guid_t PxeDHCP4Protocol = EFI_PXE_DHCP4_PROTOCOL_GUID;


typedef EFI_STATUS (*EFI_DHCP4_GET_MODE_DATA)();
typedef EFI_STATUS (*EFI_DHCP4_CONFIGURE)();
typedef EFI_STATUS (*EFI_DHCP4_START)();
typedef EFI_STATUS (*EFI_DHCP4_RENEW_REBIND)();
typedef EFI_STATUS (*EFI_DHCP4_RELEASE)();
typedef EFI_STATUS (*EFI_DHCP4_STOP)();
typedef EFI_STATUS (*EFI_DHCP4_BUILD)();
typedef EFI_STATUS (*EFI_DHCP4_TRANSMIT_RECIEVE)();
typedef EFI_STATUS (*EFI_DHCP4_PARSE)();

typedef struct _EFI_DHCP4_PROTOCOL {
	EFI_DHCP4_GET_MODE_DATA GetModeData;
	EFI_DHCP4_CONFIGURE Configure;
	EFI_DHCP4_START Start;
	EFI_DHCP4_RENEW_REBIND RenewRebind;
	EFI_DHCP4_RELEASE Release;
	EFI_DHCP4_STOP Stop;
	EFI_DHCP4_BUILD Build;
	EFI_DHCP4_TRANSMIT_RECIEVE TransmitReceive;
	EFI_DHCP4_PARSE Parse;
} EFI_DHCP4_PROTOCOL;

typedef enum {
	Dhcp4Stopped,
	Dhcp4Init,
	Dhcp4Selecting,
	Dhcp4Requesting,
	Dhcp4Bound,
	Dhcp4Renewing,
	Dhcp4Rebinding,
	Dhcp4InitReboot,
	Dhcp4Rebooting,
} EFI_DHCP4_STATE;

typedef enum {
	Dhcp4SendDiscover = 0x1,
	Dhcp4RcvdOffer,
	Dhcp4SelectOffer,
	Dhcp4SendRequest,
	Dhcp4RcvdAck,
	Dhcp4RcvdNak,
	Dhcp4SendDecline,
	Dhcp4BoundCompleted,
	Dhcp4EnterRenewing,
	Dhcp4EnterRebinding,
	Dhcp4AddressLost,
	Dhcp4Fail,
} EFI_DHCP4_EVENT;

typedef struct {
	grub_efi_uint8_t OpCode;
	grub_efi_uint8_t HwType;
	grub_efi_uint8_t HwAddrLen;
	grub_efi_uint8_t Hops;
	grub_efi_uint32_t xid;
	grub_efi_uint16_t Seconds;
	grub_efi_uint16_t reserved;
	EFI_IPv4_ADDRESS ClientAddr;
	EFI_IPv4_ADDRESS YourAddr;
	EFI_IPv4_ADDRESS ServerAddr;
	EFI_IPv4_ADDRESS GatewayAddr;
	grub_efi_uint8_t ClientHwAddr[16];
	char ServerName[64];
	char BootFileName[128];
} EFI_DHCP4_HEADER;

typedef struct {
	grub_efi_uint32_t Size;
	grub_efi_uint32_t Length;
	struct {
		EFI_DHCP4_HEADER Header;
		grub_efi_uint32_t Magik;
		grub_efi_uint8_t option[];
	} Dhcp4;
} EFI_DHCP4_PACKET;

typedef struct {
	grub_efi_uint8_t OpCode;
	grub_efi_uint8_t Length;
	grub_efi_uint8_t Data[1];
} EFI_DHCP4_PACKET_OPTION;

typedef EFI_STATUS (*EFI_DHCP4_CALLBACK) (
	EFI_DHCP4_PROTOCOL *This,
	void *Context,
	EFI_DHCP4_STATE CurrentState,
	EFI_DHCP4_EVENT Dhcp4Event,
	EFI_DHCP4_PACKET *Packet,
	EFI_DHCP4_PACKET **NewPacket);

typedef struct {
	grub_efi_uint32_t DiscoverTryCount;
	grub_efi_uint32_t *DiscoverTimeout;
	grub_efi_uint32_t RequestTryCount;
	grub_efi_uint32_t *RequestTimeout;
	EFI_IPv4_ADDRESS ClientAddress;
	EFI_DHCP4_CALLBACK Dhcp4Callback;
	void *CallbackContext;
	grub_efi_uint32_t OptionCount;
	EFI_DHCP4_PACKET_OPTION **OptionList;
} EFI_DHCP4_CONFIG_DATA;

typedef struct {
	EFI_DHCP4_STATE State;
	EFI_DHCP4_CONFIG_DATA ConfigData;
	EFI_IPv4_ADDRESS ClientAddress;
	EFI_MAC_ADDRESS ClientMacAddress;
	EFI_IPv4_ADDRESS ServerAddress;
	EFI_IPv4_ADDRESS RouterAddress;
	EFI_IPv4_ADDRESS SubnetMask;
	grub_efi_uint32_t LeaseTime;
	EFI_DHCP4_PACKET *ReplyPacket;
} EFI_DHCP4_MODE_DATA;

#endif /* DHCP_H */
