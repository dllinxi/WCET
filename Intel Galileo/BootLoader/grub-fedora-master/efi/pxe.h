#ifndef PXE_H
#define PXE_H 1

#include "byteswap.h"

extern char *grub_efi_pxe_get_config_path(grub_efi_loaded_image_t *LoadedImage);
extern void grub_print_dhcp_info(grub_efi_loaded_image_t *loaded_image);
extern char *grub_efi_pxe_path_to_path_name(void);


#define EFI_PXE_BASE_CODE_PROTOCOL \
    { 0x03c4e603, 0xac28, 0x11d3, {0x9a, 0x2d, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d} }
static grub_efi_guid_t PxeBaseCodeProtocol = EFI_PXE_BASE_CODE_PROTOCOL;

struct _EFI_PXE_BASE_CODE;

typedef enum {
	EFI_PXE_BASE_CODE_TFTP_FIRST,
	EFI_PXE_BASE_CODE_TFTP_GET_FILE_SIZE,
	EFI_PXE_BASE_CODE_TFTP_READ_FILE,
	EFI_PXE_BASE_CODE_TFTP_WRITE_FILE,
	EFI_PXE_BASE_CODE_TFTP_READ_DIRECTORY,
	EFI_PXE_BASE_CODE_MTFTP_GET_FILE_SIZE,
	EFI_PXE_BASE_CODE_MTFTP_READ_FILE,
	EFI_PXE_BASE_CODE_MTFTP_READ_DIRECTORY,
	EFI_PXE_BASE_CODE_MTFTP_LAST
} EFI_PXE_BASE_CODE_TFTP_OPCODE;

typedef struct {
    grub_efi_uint8_t                   Addr[4];
} EFI_IPv4_ADDRESS;

typedef struct {
    grub_efi_uint8_t                   Addr[16];
} EFI_IPv6_ADDRESS;

typedef struct {
    grub_efi_uint8_t                   Addr[32];
} EFI_MAC_ADDRESS;

typedef union {
    grub_efi_uint32_t      Addr[4];
    EFI_IPv4_ADDRESS    v4;
    EFI_IPv6_ADDRESS    v6;
} EFI_IP_ADDRESS;

typedef grub_efi_uint16_t EFI_PXE_BASE_CODE_UDP_PORT;

typedef struct {
	EFI_IP_ADDRESS MCastIp;
	EFI_PXE_BASE_CODE_UDP_PORT CPort;
	EFI_PXE_BASE_CODE_UDP_PORT SPort;
	grub_efi_uint16_t ListenTimeout;
	grub_efi_uint16_t TransmitTimeout;
} EFI_PXE_BASE_CODE_MTFTP_INFO;

typedef EFI_STATUS (*EFI_PXE_BASE_CODE_MTFTP)(
	struct _EFI_PXE_BASE_CODE *This,
	EFI_PXE_BASE_CODE_TFTP_OPCODE Operation,
	void *BufferPtr,
	grub_efi_boolean_t Overwrite,
	grub_efi_uint64_t *BufferSize,
	grub_efi_uintn_t *BlockSize,
	EFI_IP_ADDRESS *ServerIp,
	grub_efi_uint8_t *Filename,
	EFI_PXE_BASE_CODE_MTFTP_INFO *Info,
	grub_efi_boolean_t DontUseBuffer);

typedef struct {
    grub_efi_uint8_t                           BootpOpcode;
    grub_efi_uint8_t                           BootpHwType;
    grub_efi_uint8_t                           BootpHwAddrLen;
    grub_efi_uint8_t                           BootpGateHops;
    grub_efi_uint32_t                          BootpIdent;
    grub_efi_uint16_t                          BootpSeconds;
    grub_efi_uint16_t                          BootpFlags;
    grub_efi_uint8_t                           BootpCiAddr[4];
    grub_efi_uint8_t                           BootpYiAddr[4];
    grub_efi_uint8_t                           BootpSiAddr[4];
    grub_efi_uint8_t                           BootpGiAddr[4];
    grub_efi_uint8_t                           BootpHwAddr[16];
    grub_efi_uint8_t                           BootpSrvName[64];
    grub_efi_uint8_t                           BootpBootFile[128];
    grub_efi_uint32_t                          DhcpMagik;
    grub_efi_uint8_t                           DhcpOptions[56];
} EFI_PXE_BASE_CODE_DHCPV4_PACKET;

// TBD in EFI v1.1
//typedef struct {
//    grub_efi_uint8_t                           reserved;
//} EFI_PXE_BASE_CODE_DHCPV6_PACKET;

typedef union {
    grub_efi_uint8_t                               Raw[1472];
    EFI_PXE_BASE_CODE_DHCPV4_PACKET     Dhcpv4;
//    EFI_PXE_BASE_CODE_DHCPV6_PACKET     Dhcpv6;
} EFI_PXE_BASE_CODE_PACKET;

typedef struct {
    grub_efi_uint8_t                   Type;
    grub_efi_uint8_t                   Code;
    grub_efi_uint16_t                  Checksum;
    union {
        grub_efi_uint32_t              reserved;
        grub_efi_uint32_t              Mtu;
        grub_efi_uint32_t              Pointer;
        struct {
            grub_efi_uint16_t          Identifier;
            grub_efi_uint16_t          Sequence;
        } Echo;
    } u;
    grub_efi_uint8_t                   Data[494];
} EFI_PXE_BASE_CODE_ICMP_ERROR;

typedef struct {
    grub_efi_uint8_t                   ErrorCode;
    grub_efi_char8_t                   ErrorString[127];
} EFI_PXE_BASE_CODE_TFTP_ERROR;


#define EFI_PXE_BASE_CODE_MAX_IPCNT             8
typedef struct {
    grub_efi_uint8_t                       Filters;
    grub_efi_uint8_t                       IpCnt;
    grub_efi_uint16_t                      reserved;
    EFI_IP_ADDRESS              IpList[EFI_PXE_BASE_CODE_MAX_IPCNT];
} EFI_PXE_BASE_CODE_IP_FILTER;

#define EFI_PXE_BASE_CODE_IP_FILTER_STATION_IP             0x0001
#define EFI_PXE_BASE_CODE_IP_FILTER_BROADCAST              0x0002
#define EFI_PXE_BASE_CODE_IP_FILTER_PROMISCUOUS            0x0004
#define EFI_PXE_BASE_CODE_IP_FILTER_PROMISCUOUS_MULTICAST  0x0008

typedef struct {
    EFI_IP_ADDRESS       IpAddr;
    EFI_MAC_ADDRESS      MacAddr;
} EFI_PXE_BASE_CODE_ARP_ENTRY;

typedef struct {
    EFI_IP_ADDRESS       IpAddr;
    EFI_IP_ADDRESS       SubnetMask;
    EFI_IP_ADDRESS       GwAddr;
} EFI_PXE_BASE_CODE_ROUTE_ENTRY;

#define EFI_PXE_BASE_CODE_MAX_ARP_ENTRIES       8
#define EFI_PXE_BASE_CODE_MAX_ROUTE_ENTRIES     8

typedef struct {
    grub_efi_boolean_t                         Started;
    grub_efi_boolean_t                         Ipv6Available;
    grub_efi_boolean_t                         Ipv6Supported;
    grub_efi_boolean_t                         UsingIpv6;
    grub_efi_boolean_t                         BisSupported;
    grub_efi_boolean_t                         BisDetected;
    grub_efi_boolean_t                         AutoArp;
    grub_efi_boolean_t                         SendGUID;
    grub_efi_boolean_t                         DhcpDiscoverValid;
    grub_efi_boolean_t                         DhcpAckReceived;
    grub_efi_boolean_t                         ProxyOfferReceived;
    grub_efi_boolean_t                         PxeDiscoverValid;
    grub_efi_boolean_t                         PxeReplyReceived;
    grub_efi_boolean_t                         PxeBisReplyReceived;
    grub_efi_boolean_t                         IcmpErrorReceived;
    grub_efi_boolean_t                         TftpErrorReceived;
    grub_efi_boolean_t                         MakeCallbacks;
    grub_efi_uint8_t                           TTL;
    grub_efi_uint8_t                           ToS;
    EFI_IP_ADDRESS                  StationIp;
    EFI_IP_ADDRESS                  SubnetMask;
    EFI_PXE_BASE_CODE_PACKET        DhcpDiscover;
    EFI_PXE_BASE_CODE_PACKET        DhcpAck;
    EFI_PXE_BASE_CODE_PACKET        ProxyOffer;
    EFI_PXE_BASE_CODE_PACKET        PxeDiscover;
    EFI_PXE_BASE_CODE_PACKET        PxeReply;
    EFI_PXE_BASE_CODE_PACKET        PxeBisReply;
    EFI_PXE_BASE_CODE_IP_FILTER     IpFilter;
    grub_efi_uint32_t                          ArpCacheEntries;
    EFI_PXE_BASE_CODE_ARP_ENTRY     ArpCache[EFI_PXE_BASE_CODE_MAX_ARP_ENTRIES];
    grub_efi_uint32_t                          RouteTableEntries;
    EFI_PXE_BASE_CODE_ROUTE_ENTRY   RouteTable[EFI_PXE_BASE_CODE_MAX_ROUTE_ENTRIES];
    EFI_PXE_BASE_CODE_ICMP_ERROR    IcmpError;
    EFI_PXE_BASE_CODE_TFTP_ERROR    TftpError;
} EFI_PXE_BASE_CODE_MODE;

typedef EFI_STATUS (*EFI_PXE_BASE_CODE_START)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_STOP)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_DHCP)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_DISCOVER)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_UDP_WRITE)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_UDP_READ)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_SET_IP_FILTER)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_ARP)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_SET_PARAMETERS)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_SET_STATION_IP)();
typedef EFI_STATUS (*EFI_PXE_BASE_CODE_SET_PACKETS)();

typedef struct _EFI_PXE_BASE_CODE{
	grub_efi_uint64_t Revision;
	EFI_PXE_BASE_CODE_START             Start;
	EFI_PXE_BASE_CODE_STOP              Stop;
	EFI_PXE_BASE_CODE_DHCP              Dhcp;
	EFI_PXE_BASE_CODE_DISCOVER          Discover;
	EFI_PXE_BASE_CODE_MTFTP             Mtftp;
	EFI_PXE_BASE_CODE_UDP_WRITE         UdpWrite;
	EFI_PXE_BASE_CODE_UDP_READ          UdpRead;
	EFI_PXE_BASE_CODE_SET_IP_FILTER     SetIpFilter;
	EFI_PXE_BASE_CODE_ARP               Arp;
	EFI_PXE_BASE_CODE_SET_PARAMETERS    SetParameters;
	EFI_PXE_BASE_CODE_SET_STATION_IP    SetStationIp;
	EFI_PXE_BASE_CODE_SET_PACKETS       SetPackets;
	EFI_PXE_BASE_CODE_MODE              *Mode;
} EFI_PXE_BASE_CODE;

typedef struct {
	grub_efi_uint32_t time_low;
	grub_efi_uint16_t time_mid;
	grub_efi_uint16_t time_hi_ver;
	grub_efi_uint8_t clock_seq_hi;
	grub_efi_uint8_t clock_seq_low;
	grub_efi_uint8_t node[6];
} uuid_t;

struct tftp_info {
	grub_efi_loaded_image_t *LoadedImage;
	EFI_PXE_BASE_CODE *Pxe;
	EFI_IP_ADDRESS *ServerIp;
	char *BasePath;
	char *LastPath;
	char *Buffer;
};

extern struct tftp_info tftp_info;
extern grub_efi_status_t tftp_get_file_size(
	char *Filename,
	grub_efi_uintn_t *Size);

#endif /* PXE_H */
