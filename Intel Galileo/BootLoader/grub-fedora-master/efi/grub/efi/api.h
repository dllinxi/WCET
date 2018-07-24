/* efi.h - declare EFI types and functions */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2006  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GRUB_EFI_API_HEADER
#define GRUB_EFI_API_HEADER	1

#include <grub/types.h>
#include <grub/efi/eficall.h>

/* For consistency and safety, we name the EFI-defined types differently.
   All names are transformed into lower case, _t appended, and
   grub_efi_ prepended.  */

/* Constants.  */
#define GRUB_EFI_EVT_TIMER				0x80000000
#define GRUB_EFI_EVT_RUNTIME				0x40000000
#define GRUB_EFI_EVT_RUNTIME_CONTEXT			0x20000000
#define GRUB_EFI_EVT_NOTIFY_WAIT			0x00000100
#define GRUB_EFI_EVT_NOTIFY_SIGNAL			0x00000200
#define GRUB_EFI_EVT_SIGNAL_EXIT_BOOT_SERVICES		0x00000201
#define GRUB_EFI_EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE	0x60000202

#define GRUB_EFI_TPL_APPLICATION	4
#define GRUB_EFI_TPL_CALLBACK		8
#define GRUB_EFI_TPL_NOTIFY		16
#define GRUB_EFI_TPL_HIGH_LEVEL		31

#define GRUB_EFI_MEMORY_UC	0x0000000000000001
#define GRUB_EFI_MEMORY_WC	0x0000000000000002
#define GRUB_EFI_MEMORY_WT	0x0000000000000004
#define GRUB_EFI_MEMORY_WB	0x0000000000000008
#define GRUB_EFI_MEMORY_UCE	0x0000000000000010
#define GRUB_EFI_MEMORY_WP	0x0000000000001000
#define GRUB_EFI_MEMORY_RP	0x0000000000002000
#define GRUB_EFI_MEMORY_XP	0x0000000000004000
#define GRUB_EFI_MEMORY_RUNTIME	0x8000000000000000

#define GRUB_EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL	0x00000001
#define GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL		0x00000002
#define GRUB_EFI_OPEN_PROTOCOL_TEST_PROTOCOL		0x00000004
#define GRUB_EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER	0x00000008
#define GRUB_EFI_OPEN_PROTOCOL_BY_DRIVER		0x00000010
#define GRUB_EFI_OPEN_PROTOCOL_BY_EXCLUSIVE		0x00000020

#define GRUB_EFI_VARIABLE_NON_VOLATILE		0x0000000000000001
#define GRUB_EFI_VARIABLE_BOOTSERVICE_ACCESS	0x0000000000000002
#define GRUB_EFI_VARIABLE_RUNTIME_ACCESS	0x0000000000000004

#define GRUB_EFI_TIME_ADJUST_DAYLIGHT	0x01
#define GRUB_EFI_TIME_IN_DAYLIGHT	0x02

#define GRUB_EFI_UNSPECIFIED_TIMEZONE	0x07FF

#define GRUB_EFI_OPTIONAL_PTR	0x00000001

#define GRUB_EFI_PCI_IO_GUID	\
  { 0x4cf5b200, 0x68b8, 0x4ca5, \
    { 0x9e, 0xec, 0xb2, 0x3e, 0x3f, 0x50, 0x02, 0x9a } \
  }

#define GRUB_EFI_PCI_ROOT_IO_GUID \
  { 0x2F707EBB, 0x4A1A, 0x11d4, \
      { 0x9A, 0x38, 0x00, 0x90,	0x27, 0x3F, 0xC1, 0x4D } \
  }

#define GRUB_EFI_LOADED_IMAGE_GUID	\
  { 0x5b1b31a1, 0x9562, 0x11d2, \
    { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

#define GRUB_EFI_DISK_IO_GUID	\
  { 0xce345171, 0xba0b, 0x11d2, \
    { 0x8e, 0x4f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

#define GRUB_EFI_BLOCK_IO_GUID	\
  { 0x964e5b21, 0x6459, 0x11d2, \
    { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

#define GRUB_EFI_DEVICE_PATH_GUID	\
  { 0x09576e91, 0x6d3f, 0x11d2, \
    { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

#define GRUB_EFI_DEVICE_PATH_FROM_TEXT_GUID	\
  { 0x05c99a21, 0xc70f, 0x4ad2, \
    { 0x8a, 0x5f, 0x35, 0xdf, 0x33, 0x43, 0xf5, 0x1e } \
  }

#define GRUB_EFI_GRAPHICS_OUTPUT_GUID	\
  { 0x9042a9de, 0x23dc, 0x4a38, \
    { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } \
  }

#define GRUB_EFI_UGA_DRAW_GUID \
  { 0x982c298b, 0xf4fa, 0x41cb, \
    { 0xb8, 0x38, 0x77, 0xaa, 0x68, 0x8f, 0xb8, 0x39 } \
  }

#define GRUB_EFI_UGA_IO_GUID \
  { 0x61a4d49e, 0x6f68, 0x4f1b, \
    { 0xb9, 0x22, 0xa8, 0x6e, 0xed, 0xb, 0x7, 0xa2 } \
  }

#define GRUB_EFI_SIMPLE_FILE_SYSTEM_GUID	\
  { 0x964e5b22, 0x6459, 0x11d2, \
    { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
  }

#define GRUB_EFI_SERIAL_IO_GUID		\
  { 0xbb25cf6f, 0xf1d4, 0x11d2, \
    { 0x9a, 0x0c, 0x00, 0x90, 0x27, 0x3F, 0xc1, 0xfd } \
  }

/* Enumerations.  */
enum grub_efi_timer_delay
{
  GRUB_EFI_TIMER_CANCEL,
  GRUB_EFI_TIMER_PERIODIC,
  GRUB_EFI_TIMER_RELATIVE
};
typedef enum grub_efi_timer_delay grub_efi_timer_delay_t;

enum grub_efi_allocate_type
{
  GRUB_EFI_ALLOCATE_ANY_PAGES,
  GRUB_EFI_ALLOCATE_MAX_ADDRESS,
  GRUB_EFI_ALLOCATE_ADDRESS,
  GRUB_EFI_MAX_ALLOCATION_TYPE
};
typedef enum grub_efi_allocate_type grub_efi_allocate_type_t;

enum grub_efi_memory_type
{
  GRUB_EFI_RESERVED_MEMORY_TYPE,
  GRUB_EFI_LOADER_CODE,
  GRUB_EFI_LOADER_DATA,
  GRUB_EFI_BOOT_SERVICES_CODE,
  GRUB_EFI_BOOT_SERVICES_DATA,
  GRUB_EFI_RUNTIME_SERVICES_CODE,
  GRUB_EFI_RUNTIME_SERVICES_DATA,
  GRUB_EFI_CONVENTIONAL_MEMORY,
  GRUB_EFI_UNUSABLE_MEMORY,
  GRUB_EFI_ACPI_RECLAIM_MEMORY,
  GRUB_EFI_ACPI_MEMORY_NVS,
  GRUB_EFI_MEMORY_MAPPED_IO,
  GRUB_EFI_MEMORY_MAPPED_IO_PORT_SPACE,
  GRUB_EFI_PAL_CODE,
  GRUB_EFI_MAX_MEMORY_TYPE
};
typedef enum grub_efi_memory_type grub_efi_memory_type_t;

enum grub_efi_interface_type
{
  GRUB_EFI_NATIVE_INTERFACE
};
typedef enum grub_efi_interface_type grub_efi_interface_type_t;

enum grub_efi_locate_search_type
{
  GRUB_EFI_ALL_HANDLES,
  GRUB_EFI_BY_REGISTER_NOTIFY,
  GRUB_EFI_BY_PROTOCOL
};
typedef enum grub_efi_locate_search_type grub_efi_locate_search_type_t;

enum grub_efi_reset_type
{
  GRUB_EFI_RESET_COLD,
  GRUB_EFI_RESET_WARM,
  GRUB_EFI_RESET_SHUTDOWN
};
typedef enum grub_efi_reset_type grub_efi_reset_type_t;

/* Types.  */
typedef char grub_efi_boolean_t;
typedef long grub_efi_intn_t;
typedef unsigned long grub_efi_uintn_t;
typedef grub_int8_t grub_efi_int8_t;
typedef grub_uint8_t grub_efi_uint8_t;
typedef grub_int16_t grub_efi_int16_t;
typedef grub_uint16_t grub_efi_uint16_t;
typedef grub_int32_t grub_efi_int32_t;
typedef grub_uint32_t grub_efi_uint32_t;
typedef grub_int64_t grub_efi_int64_t;
typedef grub_uint64_t grub_efi_uint64_t;
typedef grub_uint8_t grub_efi_char8_t;
typedef grub_uint16_t grub_efi_char16_t;

typedef grub_efi_intn_t grub_efi_status_t;

#define GRUB_EFI_ERROR_CODE(value)	\
  ((1L << (sizeof (grub_efi_status_t) * 8 - 1)) | (value))

#define GRUB_EFI_WARNING_CODE(value)	(value)

#define GRUB_EFI_SUCCESS		0

#define GRUB_EFI_LOAD_ERROR		GRUB_EFI_ERROR_CODE (1)
#define GRUB_EFI_INVALID_PARAMETER	GRUB_EFI_ERROR_CODE (2)
#define GRUB_EFI_UNSUPPORTED		GRUB_EFI_ERROR_CODE (3)
#define GRUB_EFI_BAD_BUFFER_SIZE	GRUB_EFI_ERROR_CODE (4)
#define GRUB_EFI_BUFFER_TOO_SMALL	GRUB_EFI_ERROR_CODE (5)
#define GRUB_EFI_NOT_READY		GRUB_EFI_ERROR_CODE (6)
#define GRUB_EFI_DEVICE_ERROR		GRUB_EFI_ERROR_CODE (7)
#define GRUB_EFI_WRITE_PROTECTED	GRUB_EFI_ERROR_CODE (8)
#define GRUB_EFI_OUT_OF_RESOURCES	GRUB_EFI_ERROR_CODE (9)
#define GRUB_EFI_VOLUME_CORRUPTED	GRUB_EFI_ERROR_CODE (10)
#define GRUB_EFI_VOLUME_FULL		GRUB_EFI_ERROR_CODE (11)
#define GRUB_EFI_NO_MEDIA		GRUB_EFI_ERROR_CODE (12)
#define GRUB_EFI_MEDIA_CHANGED		GRUB_EFI_ERROR_CODE (13)
#define GRUB_EFI_NOT_FOUND		GRUB_EFI_ERROR_CODE (14)
#define GRUB_EFI_ACCESS_DENIED		GRUB_EFI_ERROR_CODE (15)
#define GRUB_EFI_NO_RESPONSE		GRUB_EFI_ERROR_CODE (16)
#define GRUB_EFI_NO_MAPPING		GRUB_EFI_ERROR_CODE (17)
#define GRUB_EFI_TIMEOUT		GRUB_EFI_ERROR_CODE (18)
#define GRUB_EFI_NOT_STARTED		GRUB_EFI_ERROR_CODE (19)
#define GRUB_EFI_ALREADY_STARTED	GRUB_EFI_ERROR_CODE (20)
#define GRUB_EFI_ABORTED		GRUB_EFI_ERROR_CODE (21)
#define GRUB_EFI_ICMP_ERROR		GRUB_EFI_ERROR_CODE (22)
#define GRUB_EFI_TFTP_ERROR		GRUB_EFI_ERROR_CODE (23)
#define GRUB_EFI_PROTOCOL_ERROR		GRUB_EFI_ERROR_CODE (24)
#define GRUB_EFI_INCOMPATIBLE_VERSION	GRUB_EFI_ERROR_CODE (25)
#define GRUB_EFI_SECURITY_VIOLATION	GRUB_EFI_ERROR_CODE (26)
#define GRUB_EFI_CRC_ERROR		GRUB_EFI_ERROR_CODE (27)

#define GRUB_EFI_WARN_UNKNOWN_GLYPH	GRUB_EFI_WARNING_CODE (1)
#define GRUB_EFI_WARN_DELETE_FAILURE	GRUB_EFI_WARNING_CODE (2)
#define GRUB_EFI_WARN_WRITE_FAILURE	GRUB_EFI_WARNING_CODE (3)
#define GRUB_EFI_WARN_BUFFER_TOO_SMALL	GRUB_EFI_WARNING_CODE (4)

typedef void *grub_efi_handle_t;
typedef void *grub_efi_event_t;
typedef grub_efi_uint64_t grub_efi_lba_t;
typedef grub_efi_uintn_t grub_efi_tpl_t;
typedef grub_uint8_t grub_efi_mac_address_t[32];
typedef grub_uint8_t grub_efi_ipv4_address_t[4];
typedef grub_uint16_t grub_efi_ipv6_address_t[8];
typedef grub_uint8_t grub_efi_ip_address_t[8] __attribute__ ((aligned (4)));
typedef grub_efi_uint64_t grub_efi_physical_address_t;
typedef grub_efi_uint64_t grub_efi_virtual_address_t;

struct grub_efi_guid
{
  grub_uint32_t data1;
  grub_uint16_t data2;
  grub_uint16_t data3;
  grub_uint8_t data4[8];
} __attribute__ ((aligned (8)));
typedef struct grub_efi_guid grub_efi_guid_t;

/* XXX although the spec does not specify the padding, this actually
   must have the padding!  */
struct grub_efi_memory_descriptor
{
  grub_efi_uint32_t type;
  grub_efi_uint32_t padding;
  grub_efi_physical_address_t physical_start;
  grub_efi_virtual_address_t virtual_start;
  grub_efi_uint64_t num_pages;
  grub_efi_uint64_t attribute;
};
typedef struct grub_efi_memory_descriptor grub_efi_memory_descriptor_t;

/* Device Path definitions.  */
struct grub_efi_device_path
{
  grub_efi_uint8_t type;
  grub_efi_uint8_t subtype;
  grub_efi_uint8_t length[2];
};
typedef struct grub_efi_device_path grub_efi_device_path_t;
/* XXX EFI does not define EFI_DEVICE_PATH_PROTOCOL but uses it.
   It seems to be identical to EFI_DEVICE_PATH.  */
typedef struct grub_efi_device_path grub_efi_device_path_protocol_t;

#define GRUB_EFI_DEVICE_PATH_TYPE(dp)		((dp)->type & 0x7f)
#define GRUB_EFI_DEVICE_PATH_SUBTYPE(dp)	((dp)->subtype)
#define GRUB_EFI_DEVICE_PATH_LENGTH(dp)		\
  ((dp)->length[0] | ((grub_efi_uint16_t) ((dp)->length[1]) << 8))

/* The End of Device Path nodes.  */
#define GRUB_EFI_END_DEVICE_PATH_TYPE			(0xff & 0x7f)

#define GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE		0xff
#define GRUB_EFI_END_THIS_DEVICE_PATH_SUBTYPE		0x01

#define GRUB_EFI_END_ENTIRE_DEVICE_PATH(dp)	\
  (GRUB_EFI_DEVICE_PATH_TYPE (dp) == GRUB_EFI_END_DEVICE_PATH_TYPE \
   && (GRUB_EFI_DEVICE_PATH_SUBTYPE (dp) \
       == GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE))

#define GRUB_EFI_NEXT_DEVICE_PATH(dp)	\
  ((grub_efi_device_path_t *) ((char *) (dp) \
                               + GRUB_EFI_DEVICE_PATH_LENGTH (dp)))

/* Hardware Device Path.  */
#define GRUB_EFI_HARDWARE_DEVICE_PATH_TYPE		1

#define GRUB_EFI_PCI_DEVICE_PATH_SUBTYPE		1

struct grub_efi_pci_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint8_t function;
  grub_efi_uint8_t device;
};
typedef struct grub_efi_pci_device_path grub_efi_pci_device_path_t;

#define GRUB_EFI_PCCARD_DEVICE_PATH_SUBTYPE		2

struct grub_efi_pccard_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint8_t function;
};
typedef struct grub_efi_pccard_device_path grub_efi_pccard_device_path_t;

#define GRUB_EFI_MEMORY_MAPPED_DEVICE_PATH_SUBTYPE	3

struct grub_efi_memory_mapped_device_path
{
  grub_efi_device_path_t header;
  grub_efi_memory_type_t memory_type;
  grub_efi_physical_address_t start_address;
  grub_efi_physical_address_t end_address;
};
typedef struct grub_efi_memory_mapped_device_path
  grub_efi_memory_mapped_device_path_t;

#define GRUB_EFI_VENDOR_DEVICE_PATH_SUBTYPE		4

struct grub_efi_vendor_device_path
{
  grub_efi_device_path_t header;
  grub_efi_guid_t vendor_guid;
  grub_efi_uint8_t vendor_defined_data[0];
};
typedef struct grub_efi_vendor_device_path grub_efi_vendor_device_path_t;

#define GRUB_EFI_CONTROLLER_DEVICE_PATH_SUBTYPE		5

struct grub_efi_controller_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t controller_number;
};
typedef struct grub_efi_controller_device_path
  grub_efi_controller_device_path_t;

/* ACPI Device Path.  */
#define GRUB_EFI_ACPI_DEVICE_PATH_TYPE			2

#define GRUB_EFI_ACPI_DEVICE_PATH_SUBTYPE		1

struct grub_efi_acpi_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t hid;
  grub_efi_uint32_t uid;
};
typedef struct grub_efi_acpi_device_path grub_efi_acpi_device_path_t;

#define GRUB_EFI_EXPANDED_ACPI_DEVICE_PATH_SUBTYPE	2

struct grub_efi_expanded_acpi_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t hid;
  grub_efi_uint32_t uid;
  grub_efi_uint32_t cid;
  char hidstr[1];
};
typedef struct grub_efi_expanded_acpi_device_path
  grub_efi_expanded_acpi_device_path_t;

#define GRUB_EFI_EXPANDED_ACPI_HIDSTR(dp)	\
  (((grub_efi_expanded_acpi_device_path_t *) dp)->hidstr)
#define GRUB_EFI_EXPANDED_ACPI_UIDSTR(dp)	\
  (GRUB_EFI_EXPANDED_ACPI_HIDSTR(dp) \
   + grub_strlen (GRUB_EFI_EXPANDED_ACPI_HIDSTR(dp)) + 1)
#define GRUB_EFI_EXPANDED_ACPI_CIDSTR(dp)	\
  (GRUB_EFI_EXPANDED_ACPI_UIDSTR(dp) \
   + grub_strlen (GRUB_EFI_EXPANDED_ACPI_UIDSTR(dp)) + 1)

/* Messaging Device Path.  */
#define GRUB_EFI_MESSAGING_DEVICE_PATH_TYPE		3

#define GRUB_EFI_ATAPI_DEVICE_PATH_SUBTYPE		1

struct grub_efi_atapi_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint8_t primary_secondary;
  grub_efi_uint8_t slave_master;
  grub_efi_uint16_t lun;
};
typedef struct grub_efi_atapi_device_path grub_efi_atapi_device_path_t;

#define GRUB_EFI_SCSI_DEVICE_PATH_SUBTYPE		2

struct grub_efi_scsi_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint16_t pun;
  grub_efi_uint16_t lun;
};
typedef struct grub_efi_scsi_device_path grub_efi_scsi_device_path_t;

#define GRUB_EFI_FIBRE_CHANNEL_DEVICE_PATH_SUBTYPE	3

struct grub_efi_fibre_channel_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t reserved;
  grub_efi_uint64_t wwn;
  grub_efi_uint64_t lun;
};
typedef struct grub_efi_fibre_channel_device_path
  grub_efi_fibre_channel_device_path_t;

#define GRUB_EFI_1394_DEVICE_PATH_SUBTYPE		4

struct grub_efi_1394_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t reserved;
  grub_efi_uint64_t guid;
};
typedef struct grub_efi_1394_device_path grub_efi_1394_device_path_t;

#define GRUB_EFI_USB_DEVICE_PATH_SUBTYPE		5

struct grub_efi_usb_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint8_t parent_port_number;
  grub_efi_uint8_t interface;
};
typedef struct grub_efi_usb_device_path grub_efi_usb_device_path_t;

#define GRUB_EFI_USB_CLASS_DEVICE_PATH_SUBTYPE		15

struct grub_efi_usb_class_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint16_t vendor_id;
  grub_efi_uint16_t product_id;
  grub_efi_uint8_t device_class;
  grub_efi_uint8_t device_subclass;
  grub_efi_uint8_t device_protocol;
};
typedef struct grub_efi_usb_class_device_path
  grub_efi_usb_class_device_path_t;

#define GRUB_EFI_I2O_DEVICE_PATH_SUBTYPE		6

struct grub_efi_i2o_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t tid;
};
typedef struct grub_efi_i2o_device_path grub_efi_i2o_device_path_t;

#define GRUB_EFI_MAC_ADDRESS_DEVICE_PATH_SUBTYPE	11

struct grub_efi_mac_address_device_path
{
  grub_efi_device_path_t header;
  grub_efi_mac_address_t mac_address;
  grub_efi_uint8_t if_type;
};
typedef struct grub_efi_mac_address_device_path
  grub_efi_mac_address_device_path_t;

#define GRUB_EFI_IPV4_DEVICE_PATH_SUBTYPE		12

struct grub_efi_ipv4_device_path
{
  grub_efi_device_path_t header;
  grub_efi_ipv4_address_t local_ip_address;
  grub_efi_ipv4_address_t remote_ip_address;
  grub_efi_uint16_t local_port;
  grub_efi_uint16_t remote_port;
  grub_efi_uint16_t protocol;
  grub_efi_uint8_t static_ip_address;
};
typedef struct grub_efi_ipv4_device_path grub_efi_ipv4_device_path_t;

#define GRUB_EFI_IPV6_DEVICE_PATH_SUBTYPE		13

struct grub_efi_ipv6_device_path
{
  grub_efi_device_path_t header;
  grub_efi_ipv6_address_t local_ip_address;
  grub_efi_ipv6_address_t remote_ip_address;
  grub_efi_uint16_t local_port;
  grub_efi_uint16_t remote_port;
  grub_efi_uint16_t protocol;
  grub_efi_uint8_t static_ip_address;
};
typedef struct grub_efi_ipv6_device_path grub_efi_ipv6_device_path_t;

#define GRUB_EFI_INFINIBAND_DEVICE_PATH_SUBTYPE		9

struct grub_efi_infiniband_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t resource_flags;
  grub_efi_uint8_t port_gid[16];
  grub_efi_uint64_t remote_id;
  grub_efi_uint64_t target_port_id;
  grub_efi_uint64_t device_id;
};
typedef struct grub_efi_infiniband_device_path
  grub_efi_infiniband_device_path_t;

#define GRUB_EFI_UART_DEVICE_PATH_SUBTYPE		14

struct grub_efi_uart_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t reserved;
  grub_efi_uint64_t baud_rate;
  grub_efi_uint8_t data_bits;
  grub_efi_uint8_t parity;
  grub_efi_uint8_t stop_bits;
};
typedef struct grub_efi_uart_device_path grub_efi_uart_device_path_t;

#define GRUB_EFI_VENDOR_MESSAGING_DEVICE_PATH_SUBTYPE	10

struct grub_efi_vendor_messaging_device_path
{
  grub_efi_device_path_t header;
  grub_efi_guid_t vendor_guid;
  grub_efi_uint8_t vendor_defined_data[0];
};
typedef struct grub_efi_vendor_messaging_device_path
  grub_efi_vendor_messaging_device_path_t;

/* Media Device Path.  */
#define GRUB_EFI_MEDIA_DEVICE_PATH_TYPE			4

#define GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE		1

struct grub_efi_hard_drive_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t partition_number;
  grub_efi_lba_t partition_start;
  grub_efi_lba_t partition_size;
  grub_efi_uint8_t partition_signature[16];
  grub_efi_uint8_t mbr_type;
  grub_efi_uint8_t signature_type;
};
typedef struct grub_efi_hard_drive_device_path
  grub_efi_hard_drive_device_path_t;

#define GRUB_EFI_CDROM_DEVICE_PATH_SUBTYPE		2

struct grub_efi_cdrom_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint32_t boot_entry;
  grub_efi_lba_t partition_start;
  grub_efi_lba_t partition_size;
};
typedef struct grub_efi_cdrom_device_path grub_efi_cdrom_device_path_t;

#define GRUB_EFI_VENDOR_MEDIA_DEVICE_PATH_SUBTYPE	3

struct grub_efi_vendor_media_device_path
{
  grub_efi_device_path_t header;
  grub_efi_guid_t vendor_guid;
  grub_efi_uint8_t vendor_defined_data[0];
};
typedef struct grub_efi_vendor_media_device_path
  grub_efi_vendor_media_device_path_t;

#define GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE		4

struct grub_efi_file_path_device_path
{
  grub_efi_device_path_t header;
  grub_efi_char16_t path_name[0];
};
typedef struct grub_efi_file_path_device_path
  grub_efi_file_path_device_path_t;

#define GRUB_EFI_PROTOCOL_DEVICE_PATH_SUBTYPE		5

struct grub_efi_protocol_device_path
{
  grub_efi_device_path_t header;
  grub_efi_guid_t guid;
};
typedef struct grub_efi_protocol_device_path grub_efi_protocol_device_path_t;

/* BIOS Boot Specification Device Path.  */
#define GRUB_EFI_BIOS_DEVICE_PATH_TYPE			5

#define GRUB_EFI_BIOS_DEVICE_PATH_SUBTYPE		1

struct grub_efi_bios_device_path
{
  grub_efi_device_path_t header;
  grub_efi_uint16_t device_type;
  grub_efi_uint16_t status_flags;
  char description[0];
};
typedef struct grub_efi_bios_device_path grub_efi_bios_device_path_t;

struct grub_efi_device_path_from_text
{
  grub_efi_device_path_t * (*convert_text_to_device_node) (const grub_efi_char16_t *text_device_node);
  grub_efi_device_path_t * (*convert_text_to_device_path) (const grub_efi_char16_t *text_device_path);
};
typedef struct grub_efi_device_path_from_text grub_efi_device_path_from_text_t;

struct grub_efi_open_protocol_information_entry
{
  grub_efi_handle_t agent_handle;
  grub_efi_handle_t controller_handle;
  grub_efi_uint32_t attributes;
  grub_efi_uint32_t open_count;
};
typedef struct grub_efi_open_protocol_information_entry
  grub_efi_open_protocol_information_entry_t;

struct grub_efi_time
{
  grub_efi_uint16_t year;
  grub_efi_uint8_t month;
  grub_efi_uint8_t day;
  grub_efi_uint8_t hour;
  grub_efi_uint8_t minute;
  grub_efi_uint8_t second;
  grub_efi_uint8_t pad1;
  grub_efi_uint32_t nanosecond;
  grub_efi_int16_t time_zone;
  grub_efi_uint8_t daylight;
  grub_efi_uint8_t pad2;
};
typedef struct grub_efi_time grub_efi_time_t;

struct grub_efi_time_capabilities
{
  grub_efi_uint32_t resolution;
  grub_efi_uint32_t accuracy;
  grub_efi_boolean_t sets_to_zero;
};
typedef struct grub_efi_time_capabilities grub_efi_time_capabilities_t;

struct grub_efi_input_key
{
  grub_efi_uint16_t scan_code;
  grub_efi_char16_t unicode_char;
};
typedef struct grub_efi_input_key grub_efi_input_key_t;

struct grub_efi_simple_text_output_mode
{
  grub_efi_int32_t max_mode;
  grub_efi_int32_t mode;
  grub_efi_int32_t attribute;
  grub_efi_int32_t cursor_column;
  grub_efi_int32_t cursor_row;
  grub_efi_boolean_t cursor_visible;
};
typedef struct grub_efi_simple_text_output_mode
  grub_efi_simple_text_output_mode_t;

/* Tables.  */
struct grub_efi_table_header
{
  grub_efi_uint64_t signature;
  grub_efi_uint32_t revision;
  grub_efi_uint32_t header_size;
  grub_efi_uint32_t crc32;
  grub_efi_uint32_t reserved;
};
typedef struct grub_efi_table_header grub_efi_table_header_t;

struct grub_efi_boot_services
{
  grub_efi_table_header_t hdr;

    grub_efi_tpl_t (*raise_tpl) (grub_efi_tpl_t new_tpl);

  void (*restore_tpl) (grub_efi_tpl_t old_tpl);

    grub_efi_status_t
    (*allocate_pages) (grub_efi_allocate_type_t type,
		       grub_efi_memory_type_t memory_type,
		       grub_efi_uintn_t pages,
		       grub_efi_physical_address_t * memory);

    grub_efi_status_t
    (*free_pages) (grub_efi_physical_address_t memory,
		   grub_efi_uintn_t pages);

    grub_efi_status_t
    (*get_memory_map) (grub_efi_uintn_t * memory_map_size,
		       grub_efi_memory_descriptor_t * memory_map,
		       grub_efi_uintn_t * map_key,
		       grub_efi_uintn_t * descriptor_size,
		       grub_efi_uint32_t * descriptor_version);

    grub_efi_status_t
    (*allocate_pool) (grub_efi_memory_type_t pool_type,
		      grub_efi_uintn_t size, void **buffer);

    grub_efi_status_t (*free_pool) (void *buffer);

    grub_efi_status_t
    (*create_event) (grub_efi_uint32_t type,
		     grub_efi_tpl_t notify_tpl,
		     void (*notify_function) (grub_efi_event_t event,
					      void *context),
		     void *notify_context, grub_efi_event_t * event);

    grub_efi_status_t
    (*set_timer) (grub_efi_event_t event,
		  grub_efi_timer_delay_t type,
		  grub_efi_uint64_t trigger_time);

    grub_efi_status_t
    (*wait_for_event) (grub_efi_uintn_t num_events,
		       grub_efi_event_t * event, grub_efi_uintn_t * index);

    grub_efi_status_t (*signal_event) (grub_efi_event_t event);

    grub_efi_status_t (*close_event) (grub_efi_event_t event);

    grub_efi_status_t (*check_event) (grub_efi_event_t event);

    grub_efi_status_t
    (*install_protocol_interface) (grub_efi_handle_t * handle,
				   grub_efi_guid_t * protocol,
				   grub_efi_interface_type_t interface_type,
				   void *interface);

    grub_efi_status_t
    (*reinstall_protocol_interface) (grub_efi_handle_t handle,
				     grub_efi_guid_t * protocol,
				     void *old_interface, void *new_inteface);

    grub_efi_status_t
    (*uninstall_protocol_interface) (grub_efi_handle_t handle,
				     grub_efi_guid_t * protocol,
				     void *interface);

    grub_efi_status_t
    (*handle_protocol) (grub_efi_handle_t handle,
			grub_efi_guid_t * protocol, void **interface);

  void *reserved;

    grub_efi_status_t
    (*register_protocol_notify) (grub_efi_guid_t * protocol,
				 grub_efi_event_t event, void **registration);

    grub_efi_status_t
    (*locate_handle) (grub_efi_locate_search_type_t search_type,
		      grub_efi_guid_t * protocol,
		      void *search_key,
		      grub_efi_uintn_t * buffer_size,
		      grub_efi_handle_t * buffer);

    grub_efi_status_t
    (*locate_device_path) (grub_efi_guid_t * protocol,
			   grub_efi_device_path_t ** device_path,
			   grub_efi_handle_t * device);

    grub_efi_status_t
    (*install_configuration_table) (grub_efi_guid_t * guid, void *table);

    grub_efi_status_t
    (*load_image) (grub_efi_boolean_t boot_policy,
		   grub_efi_handle_t parent_image_handle,
		   grub_efi_device_path_t * file_path,
		   void *source_buffer,
		   grub_efi_uintn_t source_size,
		   grub_efi_handle_t * image_handle);

    grub_efi_status_t
    (*start_image) (grub_efi_handle_t image_handle,
		    grub_efi_uintn_t * exit_data_size,
		    grub_efi_char16_t ** exit_data);

    grub_efi_status_t
    (*exit) (grub_efi_handle_t image_handle,
	     grub_efi_status_t exit_status,
	     grub_efi_uintn_t exit_data_size,
	     grub_efi_char16_t * exit_data) __attribute__ ((noreturn));

    grub_efi_status_t (*unload_image) (grub_efi_handle_t image_handle);

    grub_efi_status_t
    (*exit_boot_services) (grub_efi_handle_t image_handle,
			   grub_efi_uintn_t map_key);

    grub_efi_status_t (*get_next_monotonic_count) (grub_efi_uint64_t * count);

    grub_efi_status_t (*stall) (grub_efi_uintn_t microseconds);

    grub_efi_status_t
    (*set_watchdog_timer) (grub_efi_uintn_t timeout,
			   grub_efi_uint64_t watchdog_code,
			   grub_efi_uintn_t data_size,
			   grub_efi_char16_t * watchdog_data);

    grub_efi_status_t
    (*connect_controller) (grub_efi_handle_t controller_handle,
			   grub_efi_handle_t * driver_image_handle,
			   grub_efi_device_path_protocol_t *
			   remaining_device_path,
			   grub_efi_boolean_t recursive);

    grub_efi_status_t
    (*disconnect_controller) (grub_efi_handle_t controller_handle,
			      grub_efi_handle_t driver_image_handle,
			      grub_efi_handle_t child_handle);

    grub_efi_status_t
    (*open_protocol) (grub_efi_handle_t handle,
		      grub_efi_guid_t * protocol,
		      void **interface,
		      grub_efi_handle_t agent_handle,
		      grub_efi_handle_t controller_handle,
		      grub_efi_uint32_t attributes);

    grub_efi_status_t
    (*close_protocol) (grub_efi_handle_t handle,
		       grub_efi_guid_t * protocol,
		       grub_efi_handle_t agent_handle,
		       grub_efi_handle_t controller_handle);

    grub_efi_status_t
    (*open_protocol_information) (grub_efi_handle_t handle,
				  grub_efi_guid_t * protocol,
				  grub_efi_open_protocol_information_entry_t
				  ** entry_buffer,
				  grub_efi_uintn_t * entry_count);

    grub_efi_status_t
    (*protocols_per_handle) (grub_efi_handle_t handle,
			     grub_efi_guid_t *** protocol_buffer,
			     grub_efi_uintn_t * protocol_buffer_count);

    grub_efi_status_t
    (*locate_handle_buffer) (grub_efi_locate_search_type_t search_type,
			     grub_efi_guid_t * protocol,
			     void *search_key,
			     grub_efi_uintn_t * no_handles,
			     grub_efi_handle_t ** buffer);

    grub_efi_status_t
    (*locate_protocol) (grub_efi_guid_t * protocol,
			void *registration, void **interface);

    grub_efi_status_t
    (*install_multiple_protocol_interfaces) (grub_efi_handle_t * handle, ...);

    grub_efi_status_t
    (*uninstall_multiple_protocol_interfaces) (grub_efi_handle_t handle, ...);

    grub_efi_status_t
    (*calculate_crc32) (void *data,
			grub_efi_uintn_t data_size,
			grub_efi_uint32_t * crc32);

  void (*copy_mem) (void *destination, void *source, grub_efi_uintn_t length);

  void
    (*set_mem) (void *buffer, grub_efi_uintn_t size, grub_efi_uint8_t value);
};
typedef struct grub_efi_boot_services grub_efi_boot_services_t;

struct grub_efi_runtime_services
{
  grub_efi_table_header_t hdr;

    grub_efi_status_t
    (*get_time) (grub_efi_time_t * time,
		 grub_efi_time_capabilities_t * capabilities);

    grub_efi_status_t (*set_time) (grub_efi_time_t * time);

    grub_efi_status_t
    (*get_wakeup_time) (grub_efi_boolean_t * enabled,
			grub_efi_boolean_t * pending, grub_efi_time_t * time);

    grub_efi_status_t
    (*set_wakeup_time) (grub_efi_boolean_t enabled, grub_efi_time_t * time);

    grub_efi_status_t
    (*set_virtual_address_map) (grub_efi_uintn_t memory_map_size,
				grub_efi_uintn_t descriptor_size,
				grub_efi_uint32_t descriptor_version,
				grub_efi_memory_descriptor_t * virtual_map);

    grub_efi_status_t
    (*convert_pointer) (grub_efi_uintn_t debug_disposition, void **address);

    grub_efi_status_t
    (*get_variable) (grub_efi_char16_t * variable_name,
		     grub_efi_guid_t * vendor_guid,
		     grub_efi_uint32_t * attributes,
		     grub_efi_uintn_t * data_size, void *data);

    grub_efi_status_t
    (*get_next_variable_name) (grub_efi_uintn_t * variable_name_size,
			       grub_efi_char16_t * variable_name,
			       grub_efi_guid_t * vendor_guid);

    grub_efi_status_t
    (*set_variable) (grub_efi_char16_t * variable_name,
		     grub_efi_guid_t * vendor_guid,
		     grub_efi_uint32_t attributes,
		     grub_efi_uintn_t data_size, void *data);

    grub_efi_status_t
    (*get_next_high_monotonic_count) (grub_efi_uint32_t * high_count);

  void
    (*reset_system) (grub_efi_reset_type_t reset_type,
		     grub_efi_status_t reset_status,
		     grub_efi_uintn_t data_size,
		     grub_efi_char16_t * reset_data);
};
typedef struct grub_efi_runtime_services grub_efi_runtime_services_t;

struct grub_efi_configuration_table
{
  grub_efi_guid_t vendor_guid;
  void *vendor_table;
};
typedef struct grub_efi_configuration_table grub_efi_configuration_table_t;

struct grub_efi_simple_input_interface
{
  grub_efi_status_t
    (*reset) (struct grub_efi_simple_input_interface * this,
	      grub_efi_boolean_t extended_verification);

  grub_efi_status_t
    (*read_key_stroke) (struct grub_efi_simple_input_interface * this,
			grub_efi_input_key_t * key);

  grub_efi_event_t wait_for_key;
};
typedef struct grub_efi_simple_input_interface
  grub_efi_simple_input_interface_t;

struct grub_efi_simple_text_output_interface
{
  grub_efi_status_t
    (*reset) (struct grub_efi_simple_text_output_interface * this,
	      grub_efi_boolean_t extended_verification);

  grub_efi_status_t
    (*output_string) (struct grub_efi_simple_text_output_interface * this,
		      grub_efi_char16_t * string);

  grub_efi_status_t
    (*test_string) (struct grub_efi_simple_text_output_interface * this,
		    grub_efi_char16_t * string);

  grub_efi_status_t
    (*query_mode) (struct grub_efi_simple_text_output_interface * this,
		   grub_efi_uintn_t mode_number,
		   grub_efi_uintn_t * columns, grub_efi_uintn_t * rows);

  grub_efi_status_t
    (*set_mode) (struct grub_efi_simple_text_output_interface * this,
		 grub_efi_uintn_t mode_number);

  grub_efi_status_t
    (*set_attributes) (struct grub_efi_simple_text_output_interface * this,
		       grub_efi_uintn_t attribute);

  grub_efi_status_t
    (*clear_screen) (struct grub_efi_simple_text_output_interface * this);

  grub_efi_status_t
    (*set_cursor_position) (struct grub_efi_simple_text_output_interface *
			    this, grub_efi_uintn_t column,
			    grub_efi_uintn_t row);

  grub_efi_status_t
    (*enable_cursor) (struct grub_efi_simple_text_output_interface * this,
		      grub_efi_boolean_t visible);

  grub_efi_simple_text_output_mode_t *mode;
};
typedef struct grub_efi_simple_text_output_interface
  grub_efi_simple_text_output_interface_t;

#define GRUB_EFI_BLACK		0x00
#define GRUB_EFI_BLUE		0x01
#define GRUB_EFI_GREEN		0x02
#define GRUB_EFI_CYAN		0x03
#define GRUB_EFI_RED		0x04
#define GRUB_EFI_MAGENTA	0x05
#define GRUB_EFI_BROWN		0x06
#define GRUB_EFI_LIGHTGRAY	0x07
#define GRUB_EFI_BRIGHT		0x08
#define GRUB_EFI_DARKGRAY	0x08
#define GRUB_EFI_LIGHTBLUE	0x09
#define GRUB_EFI_LIGHTGREEN	0x0A
#define GRUB_EFI_LIGHTCYAN	0x0B
#define GRUB_EFI_LIGHTRED	0x0C
#define GRUB_EFI_LIGHTMAGENTA	0x0D
#define GRUB_EFI_YELLOW		0x0E
#define GRUB_EFI_WHITE		0x0F

#define GRUB_EFI_BACKGROUND_BLACK	0x00
#define GRUB_EFI_BACKGROUND_BLUE	0x10
#define GRUB_EFI_BACKGROUND_GREEN	0x20
#define GRUB_EFI_BACKGROUND_CYAN	0x30
#define GRUB_EFI_BACKGROUND_RED		0x40
#define GRUB_EFI_BACKGROUND_MAGENTA	0x50
#define GRUB_EFI_BACKGROUND_BROWN	0x60
#define GRUB_EFI_BACKGROUND_LIGHTGRAY	0x70

#define GRUB_EFI_TEXT_ATTR(fg, bg)	((fg) | ((bg)))

struct grub_efi_system_table
{
  grub_efi_table_header_t hdr;
  grub_efi_char16_t *firmware_vendor;
  grub_efi_uint32_t firmware_revision;
  grub_efi_handle_t console_in_handler;
  grub_efi_simple_input_interface_t *con_in;
  grub_efi_handle_t console_out_handler;
  grub_efi_simple_text_output_interface_t *con_out;
  grub_efi_handle_t standard_error_handle;
  grub_efi_simple_text_output_interface_t *std_err;
  grub_efi_runtime_services_t *runtime_services;
  grub_efi_boot_services_t *boot_services;
  grub_efi_uintn_t num_table_entries;
  grub_efi_configuration_table_t *configuration_table;
};
typedef struct grub_efi_system_table grub_efi_system_table_t;

struct grub_efi_loaded_image
{
  grub_efi_uint32_t revision;
  grub_efi_handle_t parent_handle;
  grub_efi_system_table_t *system_table;

  grub_efi_handle_t device_handle;
  grub_efi_device_path_t *file_path;
  void *reserved;

  grub_efi_uint32_t load_options_size;
  void *load_options;

  void *image_base;
  grub_efi_uint64_t image_size;
  grub_efi_memory_type_t image_code_type;
  grub_efi_memory_type_t image_data_type;

    grub_efi_status_t (*unload) (grub_efi_handle_t image_handle);
};
typedef struct grub_efi_loaded_image grub_efi_loaded_image_t;

struct grub_efi_disk_io
{
  grub_efi_uint64_t revision;
    grub_efi_status_t (*read) (struct grub_efi_disk_io * this,
			       grub_efi_uint32_t media_id,
			       grub_efi_uint64_t offset,
			       grub_efi_uintn_t buffer_size, void *buffer);
    grub_efi_status_t (*write) (struct grub_efi_disk_io * this,
				grub_efi_uint32_t media_id,
				grub_efi_uint64_t offset,
				grub_efi_uintn_t buffer_size, void *buffer);
};
typedef struct grub_efi_disk_io grub_efi_disk_io_t;

struct grub_efi_block_io_media
{
  grub_efi_uint32_t media_id;
  grub_efi_boolean_t removable_media;
  grub_efi_boolean_t media_present;
  grub_efi_boolean_t logical_partition;
  grub_efi_boolean_t read_only;
  grub_efi_boolean_t write_caching;
  grub_efi_uint8_t pad[3];
  grub_efi_uint32_t block_size;
  grub_efi_uint32_t io_align;
  grub_efi_uint8_t pad2[4];
  grub_efi_lba_t last_block;
};
typedef struct grub_efi_block_io_media grub_efi_block_io_media_t;

struct grub_efi_block_io
{
  grub_efi_uint64_t revision;
  grub_efi_block_io_media_t *media;
    grub_efi_status_t (*reset) (struct grub_efi_block_io * this,
				grub_efi_boolean_t extended_verification);
    grub_efi_status_t (*read_blocks) (struct grub_efi_block_io * this,
				      grub_efi_uint32_t media_id,
				      grub_efi_lba_t lba,
				      grub_efi_uintn_t buffer_size,
				      void *buffer);
    grub_efi_status_t (*write_blocks) (struct grub_efi_block_io * this,
				       grub_efi_uint32_t media_id,
				       grub_efi_lba_t lba,
				       grub_efi_uintn_t buffer_size,
				       void *buffer);
    grub_efi_status_t (*flush_blocks) (struct grub_efi_block_io * this);
};
typedef struct grub_efi_block_io grub_efi_block_io_t;

struct grub_efi_pixel_bitmask
{
  grub_efi_uint32_t red_mask;
  grub_efi_uint32_t green_mask;
  grub_efi_uint32_t blue_mask;
  grub_efi_uint32_t reserved_mask;
};
typedef struct grub_efi_pixel_bitmask grub_efi_pixel_bitmask_t;

enum grub_efi_graphics_pixel_format
{
  GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR,
  GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR,
  GRUB_EFI_PIXEL_BIT_MASK,
  GRUB_EFI_PIXEL_BLT_ONLY,
};
typedef enum grub_efi_graphics_pixel_format grub_efi_graphics_pixel_format_t;

struct grub_efi_graphics_output_mode_information
{
  grub_efi_uint32_t version;
  grub_efi_uint32_t horizontal_resolution;
  grub_efi_uint32_t vertical_resolution;
  grub_efi_graphics_pixel_format_t pixel_format;
  grub_efi_pixel_bitmask_t pixel_information;
  grub_efi_uint32_t pixels_per_scan_line;
};
typedef struct grub_efi_graphics_output_mode_information
  grub_efi_graphics_output_mode_information_t;

struct grub_efi_graphics_output_bgrr_pixel
{
  grub_efi_uint8_t blue;
  grub_efi_uint8_t green;
  grub_efi_uint8_t red;
  grub_efi_uint8_t reserved;
};
typedef struct grub_efi_graphics_output_bgrr_pixel
  grub_efi_graphics_output_bgrr_pixel_t;
typedef struct grub_efi_graphics_output_bgrr_pixel
  grub_efi_graphics_output_blt_pixel_t;
typedef struct grub_efi_graphics_output_bgrr_pixel
  grub_efi_uga_pixel_t;

struct grub_efi_graphics_output_rgbr_pixel
{
  grub_efi_uint8_t red;
  grub_efi_uint8_t green;
  grub_efi_uint8_t blue;
  grub_efi_uint8_t reserved;
};
typedef struct grub_efi_graphics_output_rgbr_pixel
  grub_efi_graphics_output_rgbr_pixel_t;

union grub_efi_graphics_output_pixel
{
  grub_efi_graphics_output_rgbr_pixel_t rgbr;
  grub_efi_graphics_output_bgrr_pixel_t bgrr;
  grub_efi_uint32_t raw;
};
typedef union grub_efi_graphics_output_pixel
  grub_efi_graphics_output_pixel_t;

enum grub_efi_graphics_output_blt_operation
{
  GRUB_EFI_BLT_VIDEO_FILL,
  GRUB_EFI_BLT_VIDEO_TO_BLT_BUFFER,
  GRUB_EFI_BLT_BUFFER_TO_VIDEO,
  GRUB_EFI_BLT_VIDEO_TO_VIDEO,
};
typedef enum grub_efi_graphics_output_blt_operation
  grub_efi_graphics_output_blt_operation_t;

struct grub_efi_graphics_output_mode
{
  grub_efi_uint32_t max_mode;
  grub_efi_uint32_t mode;
  grub_efi_graphics_output_mode_information_t *info;
  grub_efi_uintn_t size_of_info;
  grub_efi_physical_address_t frame_buffer_base;
  grub_efi_uintn_t frame_buffer_size;
};
typedef struct grub_efi_graphics_output_mode grub_efi_graphics_output_mode_t;

struct grub_efi_graphics_output
{
  grub_efi_status_t (*query_mode) (struct grub_efi_graphics_output * this,
				   grub_efi_uint32_t mode_number,
				   grub_efi_uintn_t * size_of_info,
				   grub_efi_graphics_output_mode_information_t
				   ** info);
  grub_efi_status_t (*set_mode) (struct grub_efi_graphics_output * this,
				 grub_efi_uint32_t mode_number);

  grub_efi_status_t (*blt) (struct grub_efi_graphics_output * this,
			    grub_efi_graphics_output_blt_pixel_t * blt_buffer,
			    grub_efi_graphics_output_blt_operation_t
			    blt_operation, grub_efi_uintn_t src_x,
			    grub_efi_uintn_t src_y, grub_efi_uintn_t dest_x,
			    grub_efi_uintn_t dest_y, grub_efi_uintn_t width,
			    grub_efi_uintn_t height, grub_efi_uintn_t delta);
  grub_efi_graphics_output_mode_t *mode;
};
typedef struct grub_efi_graphics_output grub_efi_graphics_output_t;

typedef enum {
  EfiUgaVideoFill,
  EfiUgaVideoToBltBuffer,
  EfiUgaBltBufferToVideo,
  EfiUgaVideoToVideo,
  EfiUgaBltMax
} grub_efi_uga_blt_operation_t;

typedef enum {
  grub_efi_pci_io_width_uint8,
  grub_efi_pci_io_width_uint16,
  grub_efi_pci_io_width_uint32,
  grub_efi_pci_io_width_uint64,
  grub_efi_pci_io_width_fifo_uint8,
  grub_efi_pci_io_width_fifo_uint16,
  grub_efi_pci_io_width_fifo_uint32,
  grub_efi_pci_io_width_fifo_uint64,
  grub_efi_pci_io_width_fill_uint8,
  grub_efi_pci_io_width_fill_uint16,
  grub_efi_pci_io_width_fill_uint32,
  grub_efi_pci_io_width_fill_uint64,
  grub_efi_pci_io_width_maximum
} grub_efi_pci_io_width;

struct grub_efi_pci_io;

typedef struct
{
  grub_efi_status_t(*read) (struct grub_efi_pci_io *this,
			    grub_efi_pci_io_width width,
			    grub_efi_uint8_t bar_index,
			    grub_efi_uint64_t offset,
			    grub_efi_uintn_t count,
			    void *buffer);
  grub_efi_status_t(*write) (struct grub_efi_pci_io *this,
			    grub_efi_pci_io_width width,
			    grub_efi_uint8_t bar_index,
			    grub_efi_uint64_t offset,
			    grub_efi_uintn_t count,
			    void *buffer);
} grub_efi_pci_io_access_t;

typedef struct
{
  grub_efi_status_t( *read) (struct grub_efi_pci_io *this,
			     grub_efi_pci_io_width width,
			     grub_efi_uint32_t offset,
			     grub_efi_uintn_t count,
			     void *buffer);
  grub_efi_status_t( *write) (struct grub_efi_pci_io *this,
			     grub_efi_pci_io_width width,
			     grub_efi_uint32_t offset,
			     grub_efi_uintn_t count,
			     void *buffer);
} grub_efi_pci_io_config_access_t;

typedef enum {
  grub_efi_pci_io_operation_bus_masterread,
  grub_efi_pci_io_operation_bus_masterwrite,
  grub_efi_pci_io_operation_bus_master_common_buffer,
  grub_efi_pci_io_operation_maximum
} grub_efi_pci_io_operation_t;

#define GRUB_EFI_PCI_IO_ATTRIBUTE_ISA_IO               0x0002
#define GRUB_EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO       0x0004
#define GRUB_EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY           0x0008
#define GRUB_EFI_PCI_IO_ATTRIBUTE_VGA_IO               0x0010
#define GRUB_EFI_PCI_IO_ATTRIBUTE_IDE_PRIMARY_IO       0x0020
#define GRUB_EFI_PCI_IO_ATTRIBUTE_IDE_SECONDARY_IO     0x0040
#define GRUB_EFI_PCI_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE 0x0080
#define GRUB_EFI_PCI_IO_ATTRIBUTE_IO                   0x0100
#define GRUB_EFI_PCI_IO_ATTRIBUTE_MEMORY               0x0200
#define GRUB_EFI_PCI_IO_ATTRIBUTE_BUS_MASTER           0x0400
#define GRUB_EFI_PCI_IO_ATTRIBUTE_MEMORY_CACHED        0x0800
#define GRUB_EFI_PCI_IO_ATTRIBUTE_MEMORY_DISABLE       0x1000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE      0x2000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM         0x4000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_DUAL_ADDRESS_CYCLE   0x8000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_ISA_IO_16            0x10000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_VGA_PALETTE_IO_16    0x20000
#define GRUB_EFI_PCI_IO_ATTRIBUTE_VGA_IO_16            0x40000

typedef enum {
  grub_efi_pci_io_attribute_operation_get,
  grub_efi_pci_io_attribute_operation_set,
  grub_efi_pci_io_attribute_operation_enable,
  grub_efi_pci_io_attribute_operation_disable,
  grub_efi_pci_io_attribute_operation_supported,
  grub_efi_pci_io_attribute_operation_maximum
} grub_efi_pci_io_attribute_operation_t;

struct grub_efi_pci_io {
  grub_efi_status_t (*poll_mem) (struct grub_efi_pci_io *this,
				 grub_efi_pci_io_width  width,
				 grub_efi_uint8_t bar_ndex,
				 grub_efi_uint64_t offset,
				 grub_efi_uint64_t mask,
				 grub_efi_uint64_t value,
				 grub_efi_uint64_t delay,
				 grub_efi_uint64_t *result);
  grub_efi_status_t (*poll_io) (struct grub_efi_pci_io *this,
				grub_efi_pci_io_width  Width,
				grub_efi_uint8_t bar_index,
				grub_efi_uint64_t offset,
				grub_efi_uint64_t mask,
				grub_efi_uint64_t value,
				grub_efi_uint64_t delay,
				grub_efi_uint64_t *result);
  grub_efi_pci_io_access_t mem;
  grub_efi_pci_io_access_t io;
  grub_efi_pci_io_config_access_t pci;
  grub_efi_status_t (*copy_mem) (struct grub_efi_pci_io *this,
				 grub_efi_pci_io_width width,
				 grub_efi_uint8_t dest_bar_index,
				 grub_efi_uint64_t dest_offset,
				 grub_efi_uint8_t src_bar_index,
				 grub_efi_uint64_t src_offset,
				 grub_efi_uintn_t count);
  grub_efi_status_t (*map) ( struct grub_efi_pci_io *this,
			     grub_efi_pci_io_operation_t operation,
			     void *host_address,
			     grub_efi_uintn_t *number_of_bytes,
			     grub_efi_uint64_t *device_address,
			     void **mapping);
  grub_efi_status_t (*unmap) (struct grub_efi_pci_io *this,
			      void *mapping);
  grub_efi_status_t (*allocate_buffer) (struct grub_efi_pci_io *this,
					grub_efi_allocate_type_t type,
					grub_efi_memory_type_t memory_type,
					grub_efi_uintn_t pages,
					void **host_address,
					grub_efi_uint64_t attributes);
  grub_efi_status_t (*free_buffer) (struct grub_efi_pci_io *this,
					grub_efi_allocate_type_t type,
					grub_efi_memory_type_t memory_type,
					grub_efi_uintn_t pages,
					void **host_address,
					grub_efi_uint64_t attributes);
  grub_efi_status_t (*flush) (struct grub_efi_pci_io *this);
  grub_efi_status_t (*get_location) (struct grub_efi_pci_io *this,
				     grub_efi_uintn_t *segment_number,
				     grub_efi_uintn_t *bus_number,
				     grub_efi_uintn_t *device_number,
				     grub_efi_uintn_t *function_number);
  grub_efi_status_t (*attributes) (struct grub_efi_pci_io *this,
				   grub_efi_pci_io_attribute_operation_t operation,
				   grub_efi_uint64_t attributes,
				   grub_efi_uint64_t *result);
  grub_efi_status_t (*get_bar_attributes) (struct grub_efi_pci_io *this,
					   grub_efi_uint8_t bar_index,
					   grub_efi_uint64_t *supports,
					   void **resources);
  grub_efi_status_t (*set_bar_attributes) (struct grub_efi_pci_io *this,
					   grub_efi_uint64_t attributes,
					   grub_efi_uint8_t bar_index,
					   grub_efi_uint64_t *offset,
					   grub_efi_uint64_t *length);
  grub_efi_uint64_t rom_size;
  void *rom_image;
};
typedef struct grub_efi_pci_io grub_efi_pci_io_t;

struct grub_efi_pci_root_io;

typedef struct
{
  grub_efi_status_t(*read) (struct grub_efi_pci_root_io *this,
			    grub_efi_pci_io_width width,
			    grub_efi_uint64_t address,
			    grub_efi_uintn_t count,
			    void *buffer);
  grub_efi_status_t(*write) (struct grub_efi_pci_root_io *this,
			    grub_efi_pci_io_width width,
			    grub_efi_uint64_t address,
			    grub_efi_uintn_t count,
			    void *buffer);
} grub_efi_pci_root_io_access_t;

typedef enum {
  grub_efi_pci_root_io_operation_bus_master_read,
  grub_efi_pci_root_io_operation_bus_master_write,
  grub_efi_pci_root_io_operation_bus_master_common_buffer,
  grub_efi_pci_root_io_operation_bus_master_read_64,
  grub_efi_pci_root_io_operation_bus_master_write_64,
  grub_efi_pci_root_io_operation_bus_master_common_buffer_64,
  grub_efi_pci_root_io_operation_maximum
} grub_efi_pci_root_io_operation_t;

struct grub_efi_pci_root_io {
  grub_efi_handle_t parent;
  grub_efi_status_t (*poll_mem) (struct grub_efi_pci_root_io *this,
				 grub_efi_pci_io_width  width,
				 grub_efi_uint64_t address,
				 grub_efi_uint64_t mask,
				 grub_efi_uint64_t value,
				 grub_efi_uint64_t delay,
				 grub_efi_uint64_t *result);
  grub_efi_status_t (*poll_io) (struct grub_efi_pci_root_io *this,
				grub_efi_pci_io_width  Width,
				grub_efi_uint64_t address,
				grub_efi_uint64_t mask,
				grub_efi_uint64_t value,
				grub_efi_uint64_t delay,
				grub_efi_uint64_t *result);
  grub_efi_pci_root_io_access_t mem;
  grub_efi_pci_root_io_access_t io;
  grub_efi_pci_root_io_access_t pci;
  grub_efi_status_t (*copy_mem) (struct grub_efi_pci_root_io *this,
				 grub_efi_pci_io_width width,
				 grub_efi_uint64_t dest_offset,
				 grub_efi_uint64_t src_offset,
				 grub_efi_uintn_t count);
  grub_efi_status_t (*map) ( struct grub_efi_pci_root_io *this,
			     grub_efi_pci_root_io_operation_t operation,
			     void *host_address,
			     grub_efi_uintn_t *number_of_bytes,
			     grub_efi_uint64_t *device_address,
			     void **mapping);
  grub_efi_status_t (*unmap) (struct grub_efi_pci_root_io *this,
			      void *mapping);
  grub_efi_status_t (*allocate_buffer) (struct grub_efi_pci_root_io *this,
					grub_efi_allocate_type_t type,
					grub_efi_memory_type_t memory_type,
					grub_efi_uintn_t pages,
					void **host_address,
					grub_efi_uint64_t attributes);
  grub_efi_status_t (*free_buffer) (struct grub_efi_pci_root_io *this,
				    grub_efi_uintn_t pages,
				    void **host_address);
  grub_efi_status_t (*flush) (struct grub_efi_pci_root_io *this);
  grub_efi_status_t (*get_attributes) (struct grub_efi_pci_root_io *this,
				       grub_efi_uint64_t *supports,
				       void **resources);
  grub_efi_status_t (*set_attributes) (struct grub_efi_pci_root_io *this,
				       grub_efi_uint64_t attributes,
				       grub_efi_uint64_t *offset,
				       grub_efi_uint64_t *length);
  grub_efi_status_t (*configuration) (struct grub_efi_pci_root_io *this,
				      void **resources);
};

typedef struct grub_efi_pci_root_io grub_efi_pci_root_io_t;

struct grub_efi_uga_draw
{
  grub_efi_status_t (*get_mode) (struct grub_efi_uga_draw * this,
                                 grub_efi_uint32_t *horizontal_resolution,
                                 grub_efi_uint32_t *vertical_resolution,
                                 grub_efi_uint32_t *color_depth,
                                 grub_efi_uint32_t *refresh_rate);
  grub_efi_status_t (*set_mode) (struct grub_efi_uga_draw * this,
                                 grub_efi_uint32_t horizontal_resolution,
                                 grub_efi_uint32_t vertical_resolution,
                                 grub_efi_uint32_t color_depth,
                                 grub_efi_uint32_t refresh_rate);
  grub_efi_status_t (*blt) (struct grub_efi_uga_draw * this,
                            grub_efi_uga_pixel_t *blt_buffer,
                            grub_efi_uga_blt_operation_t blt_operation,
                            grub_efi_uintn_t source_x,
                            grub_efi_uintn_t source_y,
                            grub_efi_uintn_t destination_x,
                            grub_efi_uintn_t destination_y,
                            grub_efi_uintn_t width,
                            grub_efi_uintn_t height,
                            grub_efi_uintn_t delta);
};
typedef struct grub_efi_uga_draw grub_efi_uga_draw_t;

typedef grub_uint32_t grub_uga_status_t;

typedef enum {
  UgaDtParentBus = 1,
  UgaDtGraphicsController,
  UgaDtOutputController,
  UgaDtOutputPort,
  UgaDtOther
} grub_uga_device_type_t;

typedef grub_efi_uint32_t grub_uga_device_id_t;

struct grub_uga_device_data {
  grub_uga_device_type_t device_type;
  grub_uga_device_id_t device_id;
  grub_efi_uint32_t device_context_size;
  grub_efi_uint32_t shared_context_size;
};
typedef struct grub_uga_device_data grub_uga_device_data_t;

struct grub_uga_device {
  void *device_context;
  void *shared_context;
  void *runtime_context;
  struct grub_uga_device *parent_device;
  void *bus_io_services;
  void *stdio_services;
  grub_uga_device_data_t device_data;
};
typedef struct grub_uga_device grub_uga_device_t;

typedef enum {
  UgaIoGetVersion = 1,
  UgaIoGetChildDevice,
  UgaIoStartDevice,
  UgaIoStopDevice,
  UgaIoFlushDevice,
  UgaIoResetDevice,
  UgaIoGetDeviceState,
  UgaIoSetDeviceState,
  UgaIoSetPowerState,
  UgaIoGetMemoryConfiguration,
  UgaIoSetVideoMode,
  UgaIoCopyRectangle,
  UgaIoGetEdidSegment,
  UgaIoDeviceChannelOpen,
  UgaIoDeviceChannelClose,
  UgaIoDeviceChannelRead,
  UgaIoDeviceChannelWrite,
  UgaIoGetPersistentDataSize,
  UgaIoGetPersistentData,
  UgaIoSetPersistentData,
  UgaIoGetDevicePropertySize,
  UgaIoGetDeviceProperty,
  UgaIoBtPrivateInterface
} grub_uga_io_request_code_t;

struct grub_uga_io_request {
  grub_uga_io_request_code_t io_request_code;
  void *in_buffer;
  grub_efi_uint64_t in_buffer_size;
  grub_efi_uint64_t bytes_returned;
};
typedef struct grub_uga_io_request grub_uga_io_request_t;

struct grub_efi_uga_io
{
  grub_efi_status_t (*create_device) (struct grub_efi_uga_io * this,
                                      grub_uga_device_t *parent_device,
                                      grub_uga_device_data_t *device_data,
                                      void *runtime_context,
                                      grub_uga_device_t **device);
  grub_efi_status_t (*delete_device) (struct grub_efi_uga_io * this,
                                      grub_uga_device_t *device);
  grub_uga_status_t (*dispatch_service) (grub_uga_device_t *device,
                                         grub_uga_io_request_t *io_request);
};
typedef struct grub_efi_uga_io grub_efi_uga_io_t;

/* XXX PJFIX add uga driver handoff stuff */

// File Open Modes
#define GRUB_EFI_FILE_MODE_READ		0x0000000000000001ULL
#define GRUB_EFI_FILE_MODE_WRITE	0x0000000000000002ULL
#define GRUB_EFI_FILE_MODE_CREATE	0x8000000000000000ULL

// File Attributes
#define GRUB_EFI_FILE_READ_ONLY		0x0000000000000001ULL
#define GRUB_EFI_FILE_HIDDEN		0x0000000000000002ULL
#define GRUB_EFI_FILE_SYSTEM		0x0000000000000004ULL
#define GRUB_EFI_FILE_RESERVED		0x0000000000000008ULL
#define GRUB_EFI_FILE_DIRECTORY		0x0000000000000010ULL
#define GRUB_EFI_FILE_ARCHIVE		0x0000000000000020ULL
#define GRUB_EFI_FILE_VALID_ATTR	0x0000000000000037ULL

struct grub_efi_file
{
  grub_efi_uint64_t revision;
  grub_efi_status_t (*open) (struct grub_efi_file * this,
			     struct grub_efi_file ** new_handle,
			     grub_efi_char16_t * file_name,
			     grub_efi_uint64_t open_mode,
			     grub_efi_uint64_t attributes);
  grub_efi_status_t (*close) (struct grub_efi_file * this);
  grub_efi_status_t (*delete) (struct grub_efi_file * this);
  grub_efi_status_t (*read) (struct grub_efi_file * this,
			     grub_efi_uintn_t * buffer_size,
			     void * buffer);
  grub_efi_status_t (*write) (struct grub_efi_file * this,
			      grub_efi_uintn_t * buffer_size,
			      void * buffer);
  grub_efi_status_t (*get_position) (struct grub_efi_file * this,
				     grub_efi_uint64_t * position);
  grub_efi_status_t (*set_position) (struct grub_efi_file * this,
				     grub_efi_uint64_t position);
  grub_efi_status_t (*get_info) (struct grub_efi_file * this,
				 grub_efi_guid_t * information_type,
				 grub_efi_uintn_t * buffer_size,
				 void * buffer);
  grub_efi_status_t (*set_info) (struct grub_efi_file * this,
				 grub_efi_guid_t * information_type,
				 grub_efi_uintn_t buffer_size,
				 void * buffer);
  grub_efi_status_t (*flush) (struct grub_efi_file * this);
};
typedef struct grub_efi_file grub_efi_file_t;

struct grub_efi_simple_file_system
{
  grub_efi_uint64_t revision;
  grub_efi_status_t (*open_volume) (struct grub_efi_simple_file_system * this,
				    grub_efi_file_t ** root);
};
typedef struct grub_efi_simple_file_system grub_efi_simple_file_system_t;

struct grub_efi_serial_io_mode
{
  grub_efi_uint32_t control_mask;
  grub_efi_uint32_t timeout;
  grub_efi_uint64_t baud_rate;
  grub_efi_uint32_t receive_fifo_depth;
  grub_efi_uint32_t data_bits;
  grub_efi_uint32_t parity;
  grub_efi_uint32_t stop_bits;
};
typedef struct grub_efi_serial_io_mode grub_efi_serial_io_mode_t;

enum grub_efi_parity
{
  GRUB_EFI_DEFAULT_PARITY,
  GRUB_EFI_NO_PARITY,
  GRUB_EFI_EVEN_PARITY,
  GRUB_EFI_ODD_PARITY,
  GRUB_EFI_MARK_PARITY,
  GRUB_EFI_SPACE_PARITY
};
typedef enum grub_efi_parity grub_efi_parity_t;

enum grub_efi_stop_bits
{
  GRUB_EFI_DEFAULT_STOP_BITS,
  GRUB_EFI_ONE_STOP_BIT,
  GRUB_EFI_ONE_FIVE_STOP_BITS,
  GRUB_EFI_TWO_STOP_BITS
};
typedef enum grub_efi_stop_bits grub_efi_stop_bits_t;

#define GRUB_EFI_SERIAL_CLEAR_TO_SEND			0x0010
#define GRUB_EFI_SERIAL_DATA_SET_READY			0x0020
#define GRUB_EFI_SERIAL_RING_INDICATE			0x0040
#define GRUB_EFI_SERIAL_CARRIER_DETECT			0x0080
#define GRUB_EFI_SERIAL_REQUEST_TO_SEND			0x0002
#define GRUB_EFI_SERIAL_DATA_TERMINAL_READY		0x0001
#define GRUB_EFI_SERIAL_INPUT_BUFFER_EMPTY		0x0100
#define GRUB_EFI_SERIAL_OUTPUT_BUFFER_EMPTY		0x0200
#define GRUB_EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE	0x1000
#define GRUB_EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE	0x2000
#define GRUB_EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE	0x4000

struct grub_efi_serial_io
{
  grub_efi_uint32_t revision;
  grub_efi_status_t (*reset) (struct grub_efi_serial_io * this);
  grub_efi_status_t (*set_attributes) (struct grub_efi_serial_io * this,
				       grub_efi_uint64_t baud_rate,
				       grub_efi_uint32_t receive_fifo_depth,
				       grub_efi_uint32_t timeout,
				       grub_efi_parity_t parity,
				       grub_efi_uint8_t data_bits,
				       grub_efi_stop_bits_t stop_bits);
  grub_efi_status_t (*set_control_bits) (struct grub_efi_serial_io * this,
					 grub_efi_uint32_t control);
  grub_efi_status_t (*get_control_bits) (struct grub_efi_serial_io * this,
					 grub_efi_uint32_t * control);
  grub_efi_status_t (*write) (struct grub_efi_serial_io * this,
			      grub_efi_uintn_t * buffer_size,
			      void * buffer);
  grub_efi_status_t (*read) (struct grub_efi_serial_io * this,
			     grub_efi_uintn_t * buffer_size,
			     void * buffer);
  grub_efi_serial_io_mode_t *mode;
};
typedef struct grub_efi_serial_io grub_efi_serial_io_t;

#endif /* ! GRUB_EFI_API_HEADER */
