/*
 * GRUB  --  GRand Unified Bootloader
 * Copyright (C) 2010 Free Software Foundation, Inc.
 *
 *  GRUB is free software; you can redistribute it and/or modify
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
 *  along with GRUB; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>

#include <shared.h>

/* Duplicate a device path.  */
grub_efi_device_path_t *
duplicate_device_path (const grub_efi_device_path_t *dp)
{
  grub_efi_device_path_t *p;
  grub_size_t total_size = 0;

  for (p = (grub_efi_device_path_t *) dp;
       ;
       p = GRUB_EFI_NEXT_DEVICE_PATH (p))
    {
      total_size += GRUB_EFI_DEVICE_PATH_LENGTH (p);
      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (p))
	break;
    }

  p = grub_malloc (total_size);
  if (! p)
    return 0;

  grub_memcpy (p, dp, total_size);
  return p;
}

/* Return the device path node right before the end node.  */
grub_efi_device_path_t *
find_last_device_path (const grub_efi_device_path_t *dp)
{
  grub_efi_device_path_t *next, *p;

  if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (dp))
    return 0;

  for (p = (grub_efi_device_path_t *) dp, next = GRUB_EFI_NEXT_DEVICE_PATH (p);
       ! GRUB_EFI_END_ENTIRE_DEVICE_PATH (next);
       p = next, next = GRUB_EFI_NEXT_DEVICE_PATH (next))
    ;

  return p;
}

/* Return the parent device path node. Must be freed */
grub_efi_device_path_t *
find_parent_device_path (const grub_efi_device_path_t *dp)
{
  grub_efi_device_path_t *final, *dup;

  dup = duplicate_device_path(dp);
  final = find_last_device_path(dup);

  final->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
  final->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;

  return dup;
}

/* Compare device paths.  */
int
compare_device_paths (const grub_efi_device_path_t *dp1,
		      const grub_efi_device_path_t *dp2)
{
  if (! dp1 || ! dp2)
    /* Return non-zero.  */
    return 1;

  while (1)
    {
      grub_efi_uint8_t type1, type2;
      grub_efi_uint8_t subtype1, subtype2;
      grub_efi_uint16_t len1, len2;
      int ret;

      type1 = GRUB_EFI_DEVICE_PATH_TYPE (dp1);
      type2 = GRUB_EFI_DEVICE_PATH_TYPE (dp2);

      if (type1 != type2)
	return (int) type2 - (int) type1;

      subtype1 = GRUB_EFI_DEVICE_PATH_SUBTYPE (dp1);
      subtype2 = GRUB_EFI_DEVICE_PATH_SUBTYPE (dp2);

      if (subtype1 != subtype2)
	return (int) subtype1 - (int) subtype2;

      len1 = GRUB_EFI_DEVICE_PATH_LENGTH (dp1);
      len2 = GRUB_EFI_DEVICE_PATH_LENGTH (dp2);

      if (len1 != len2)
	return (int) len1 - (int) len2;

      ret = grub_memcmp ((char *)dp1, (char *)dp2, len1);
      if (ret != 0)
	return ret;

      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (dp1))
	break;

      dp1 = (grub_efi_device_path_t *) ((char *) dp1 + len1);
      dp2 = (grub_efi_device_path_t *) ((char *) dp2 + len2);
    }

  return 0;
}

/* Print the chain of Device Path nodes. This is mainly for debugging. */
void
grub_efi_print_device_path (grub_efi_device_path_t *dp)
{
  while (1)
    {
      grub_efi_uint8_t type = GRUB_EFI_DEVICE_PATH_TYPE (dp);
      grub_efi_uint8_t subtype = GRUB_EFI_DEVICE_PATH_SUBTYPE (dp);
      grub_efi_uint16_t len = GRUB_EFI_DEVICE_PATH_LENGTH (dp);

      switch (type)
	{
	case GRUB_EFI_END_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE:
	      grub_printf ("/EndEntire\n");
	      //grub_putchar ('\n');
	      break;
	    case GRUB_EFI_END_THIS_DEVICE_PATH_SUBTYPE:
	      grub_printf ("/EndThis\n");
	      //grub_putchar ('\n');
	      break;
	    default:
	      grub_printf ("/EndUnknown(%x)\n", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_HARDWARE_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_PCI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_pci_device_path_t pci;
		grub_memcpy (&pci, dp, len);
		grub_printf ("/PCI(%x,%x)",
			     (unsigned) pci.function, (unsigned) pci.device);
	      }
	      break;
	    case GRUB_EFI_PCCARD_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_pccard_device_path_t pccard;
		grub_memcpy (&pccard, dp, len);
		grub_printf ("/PCCARD(%x)",
			     (unsigned) pccard.function);
	      }
	      break;
	    case GRUB_EFI_MEMORY_MAPPED_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_memory_mapped_device_path_t mmapped;
		grub_memcpy (&mmapped, dp, len);
		grub_printf ("/MMap(%x,%llx,%llx)",
			     (unsigned) mmapped.memory_type,
			     mmapped.start_address,
			     mmapped.end_address);
	      }
	      break;
	    case GRUB_EFI_VENDOR_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    case GRUB_EFI_CONTROLLER_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_controller_device_path_t controller;
		grub_memcpy (&controller, dp, len);
		grub_printf ("/Ctrl(%x)",
			     (unsigned) controller.controller_number);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownHW(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_ACPI_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_ACPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_acpi_device_path_t acpi;
		grub_memcpy (&acpi, dp, len);
		grub_printf ("/ACPI(%x,%x)",
			     (unsigned) acpi.hid,
			     (unsigned) acpi.uid);
	      }
	      break;
	    case GRUB_EFI_EXPANDED_ACPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_expanded_acpi_device_path_t eacpi;
		grub_memcpy (&eacpi, dp, sizeof (eacpi));
		grub_printf ("/ACPI(");

		if (GRUB_EFI_EXPANDED_ACPI_HIDSTR (dp)[0] == '\0')
		  grub_printf ("%x,", (unsigned) eacpi.hid);
		else
		  grub_printf ("%s,", GRUB_EFI_EXPANDED_ACPI_HIDSTR (dp));

		if (GRUB_EFI_EXPANDED_ACPI_UIDSTR (dp)[0] == '\0')
		  grub_printf ("%x,", (unsigned) eacpi.uid);
		else
		  grub_printf ("%s,", GRUB_EFI_EXPANDED_ACPI_UIDSTR (dp));

		if (GRUB_EFI_EXPANDED_ACPI_CIDSTR (dp)[0] == '\0')
		  grub_printf ("%x)", (unsigned) eacpi.cid);
		else
		  grub_printf ("%s)", GRUB_EFI_EXPANDED_ACPI_CIDSTR (dp));
	      }
	      break;
	    default:
	      grub_printf ("/UnknownACPI(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_MESSAGING_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_ATAPI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_atapi_device_path_t atapi;
		grub_memcpy (&atapi, dp, len);
		grub_printf ("/ATAPI(%x,%x,%x)",
			     (unsigned) atapi.primary_secondary,
			     (unsigned) atapi.slave_master,
			     (unsigned) atapi.lun);
	      }
	      break;
	    case GRUB_EFI_SCSI_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_scsi_device_path_t scsi;
		grub_memcpy (&scsi, dp, len);
		grub_printf ("/SCSI(%x,%x)",
			     (unsigned) scsi.pun,
			     (unsigned) scsi.lun);
	      }
	      break;
	    case GRUB_EFI_FIBRE_CHANNEL_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_fibre_channel_device_path_t fc;
		grub_memcpy (&fc, dp, len);
		grub_printf ("/FibreChannel(%llx,%llx)",
			     fc.wwn, fc.lun);
	      }
	      break;
	    case GRUB_EFI_1394_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_1394_device_path_t firewire;
		grub_memcpy (&firewire, dp, len);
		grub_printf ("/1394(%llx)", firewire.guid);
	      }
	      break;
	    case GRUB_EFI_USB_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_usb_device_path_t usb;
		grub_memcpy (&usb, dp, len);
		grub_printf ("/USB(%x,%x)",
			     (unsigned) usb.parent_port_number,
			     (unsigned) usb.interface);
	      }
	      break;
	    case GRUB_EFI_USB_CLASS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_usb_class_device_path_t usb_class;
		grub_memcpy (&usb_class, dp, len);
		grub_printf ("/USBClass(%x,%x,%x,%x,%x)",
			     (unsigned) usb_class.vendor_id,
			     (unsigned) usb_class.product_id,
			     (unsigned) usb_class.device_class,
			     (unsigned) usb_class.device_subclass,
			     (unsigned) usb_class.device_protocol);
	      }
	      break;
	    case GRUB_EFI_I2O_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_i2o_device_path_t i2o;
		grub_memcpy (&i2o, dp, len);
		grub_printf ("/I2O(%x)", (unsigned) i2o.tid);
	      }
	      break;
	    case GRUB_EFI_MAC_ADDRESS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_mac_address_device_path_t mac;
		grub_memcpy (&mac, dp, len);
		grub_printf ("/MacAddr(%x:%x:%x:%x:%x:%x,%x)",
			     (unsigned) mac.mac_address[0],
			     (unsigned) mac.mac_address[1],
			     (unsigned) mac.mac_address[2],
			     (unsigned) mac.mac_address[3],
			     (unsigned) mac.mac_address[4],
			     (unsigned) mac.mac_address[5],
			     (unsigned) mac.if_type);
	      }
	      break;
	    case GRUB_EFI_IPV4_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_ipv4_device_path_t ipv4;
		grub_memcpy (&ipv4, dp, len);
		grub_printf ("/IPv4(%u.%u.%u.%u,%u.%u.%u.%u,%u,%u,%x,%x)",
			     (unsigned) ipv4.local_ip_address[0],
			     (unsigned) ipv4.local_ip_address[1],
			     (unsigned) ipv4.local_ip_address[2],
			     (unsigned) ipv4.local_ip_address[3],
			     (unsigned) ipv4.remote_ip_address[0],
			     (unsigned) ipv4.remote_ip_address[1],
			     (unsigned) ipv4.remote_ip_address[2],
			     (unsigned) ipv4.remote_ip_address[3],
			     (unsigned) ipv4.local_port,
			     (unsigned) ipv4.remote_port,
			     (unsigned) ipv4.protocol,
			     (unsigned) ipv4.static_ip_address);
	      }
	      break;
	    case GRUB_EFI_IPV6_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_ipv6_device_path_t ipv6;
		grub_memcpy (&ipv6, dp, len);
		grub_printf ("/IPv6(%x:%x:%x:%x:%x:%x:%x:%x,%x:%x:%x:%x:%x:%x:%x:%x,%u,%u,%x,%x)",
			     (unsigned) ipv6.local_ip_address[0],
			     (unsigned) ipv6.local_ip_address[1],
			     (unsigned) ipv6.local_ip_address[2],
			     (unsigned) ipv6.local_ip_address[3],
			     (unsigned) ipv6.local_ip_address[4],
			     (unsigned) ipv6.local_ip_address[5],
			     (unsigned) ipv6.local_ip_address[6],
			     (unsigned) ipv6.local_ip_address[7],
			     (unsigned) ipv6.remote_ip_address[0],
			     (unsigned) ipv6.remote_ip_address[1],
			     (unsigned) ipv6.remote_ip_address[2],
			     (unsigned) ipv6.remote_ip_address[3],
			     (unsigned) ipv6.remote_ip_address[4],
			     (unsigned) ipv6.remote_ip_address[5],
			     (unsigned) ipv6.remote_ip_address[6],
			     (unsigned) ipv6.remote_ip_address[7],
			     (unsigned) ipv6.local_port,
			     (unsigned) ipv6.remote_port,
			     (unsigned) ipv6.protocol,
			     (unsigned) ipv6.static_ip_address);
	      }
	      break;
	    case GRUB_EFI_INFINIBAND_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_infiniband_device_path_t ib;
		grub_memcpy (&ib, dp, len);
		grub_printf ("/InfiniBand(%x,%llx,%llx,%llx)",
			     (unsigned) ib.port_gid[0], /* XXX */
			     ib.remote_id,
			     ib.target_port_id,
			     ib.device_id);
	      }
	      break;
	    case GRUB_EFI_UART_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_uart_device_path_t uart;
		grub_memcpy (&uart, dp, len);
		grub_printf ("/UART(%llu,%u,%x,%x)",
			     uart.baud_rate,
			     uart.data_bits,
			     uart.parity,
			     uart.stop_bits);
	      }
	      break;
	    case GRUB_EFI_VENDOR_MESSAGING_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_messaging_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownMessaging(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_MEDIA_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_hard_drive_device_path_t hd;
		grub_memcpy (&hd, dp, len);
		grub_printf ("/HD(%u,%llx,%llx,%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     hd.partition_number,
			     hd.partition_start,
			     hd.partition_size,
			     (unsigned) hd.partition_signature[3],
			     (unsigned) hd.partition_signature[2],
			     (unsigned) hd.partition_signature[1],
			     (unsigned) hd.partition_signature[0],
			     (unsigned) hd.partition_signature[5],
			     (unsigned) hd.partition_signature[4],
			     (unsigned) hd.partition_signature[7],
			     (unsigned) hd.partition_signature[6],
			     (unsigned) hd.partition_signature[9],
			     (unsigned) hd.partition_signature[8],
			     (unsigned) hd.partition_signature[10],
			     (unsigned) hd.partition_signature[11],
			     (unsigned) hd.partition_signature[12],
			     (unsigned) hd.partition_signature[13],
			     (unsigned) hd.partition_signature[14],
			     (unsigned) hd.partition_signature[15]);
	      }
	      break;
	    case GRUB_EFI_CDROM_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_cdrom_device_path_t cd;
		grub_memcpy (&cd, dp, len);
		grub_printf ("/CD(%u,%llx,%llx)",
			     cd.boot_entry,
			     cd.partition_start,
			     cd.partition_size);
	      }
	      break;
	    case GRUB_EFI_VENDOR_MEDIA_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_vendor_media_device_path_t vendor;
		grub_memcpy (&vendor, dp, sizeof (vendor));
		grub_printf ("/Vendor(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) vendor.vendor_guid.data1,
			     (unsigned) vendor.vendor_guid.data2,
			     (unsigned) vendor.vendor_guid.data3,
			     (unsigned) vendor.vendor_guid.data4[0],
			     (unsigned) vendor.vendor_guid.data4[1],
			     (unsigned) vendor.vendor_guid.data4[2],
			     (unsigned) vendor.vendor_guid.data4[3],
			     (unsigned) vendor.vendor_guid.data4[4],
			     (unsigned) vendor.vendor_guid.data4[5],
			     (unsigned) vendor.vendor_guid.data4[6],
			     (unsigned) vendor.vendor_guid.data4[7]);
	      }
	      break;
	    case GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_file_path_device_path_t *fp;
		grub_uint8_t buf[(len - 4) * 2 + 1];
		fp = (grub_efi_file_path_device_path_t *) dp;
		*grub_utf16_to_utf8 (buf, fp->path_name,
				     (len - 4) / sizeof (grub_efi_char16_t))
		  = '\0';
		grub_printf ("/File(%s)", buf);
	      }
	      break;
	    case GRUB_EFI_PROTOCOL_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_protocol_device_path_t proto;
		grub_memcpy (&proto, dp, sizeof (proto));
		grub_printf ("/Protocol(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)",
			     (unsigned) proto.guid.data1,
			     (unsigned) proto.guid.data2,
			     (unsigned) proto.guid.data3,
			     (unsigned) proto.guid.data4[0],
			     (unsigned) proto.guid.data4[1],
			     (unsigned) proto.guid.data4[2],
			     (unsigned) proto.guid.data4[3],
			     (unsigned) proto.guid.data4[4],
			     (unsigned) proto.guid.data4[5],
			     (unsigned) proto.guid.data4[6],
			     (unsigned) proto.guid.data4[7]);
	      }
	      break;
	    default:
	      grub_printf ("/UnknownMedia(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	case GRUB_EFI_BIOS_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_BIOS_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_bios_device_path_t bios;
		grub_memcpy (&bios, dp, sizeof (bios));
		grub_printf ("/BIOS(%x,%x,%s)",
			     (unsigned) bios.device_type,
			     (unsigned) bios.status_flags,
			     (char *) (dp + 1));
	      }
	      break;
	    default:
	      grub_printf ("/UnknownBIOS(%x)", (unsigned) subtype);
	      break;
	    }
	  break;

	default:
	  grub_printf ("/UnknownType(%x,%x)\n",
		       (unsigned) type,
		       (unsigned) subtype);
	  return;
	  break;
	}

      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (dp))
	break;

      dp = (grub_efi_device_path_t *) ((char *) dp + len);
    }
}

static inline int
dpname_matches(char *str, char *candidate)
{
  grub_size_t clen = grub_strlen(candidate);
  char scratch[clen + 2];
  int rc;

  grub_strncpy(scratch, candidate, clen);
  scratch[clen+1] = '\0';
  if (scratch[clen-1] == '$')
    {
      scratch[--clen] = '\0';
      rc = !grub_strncasecmp(str, scratch, clen);
      return rc;
    }

    grub_strncpy(scratch+clen, "(", 2);
    clen = grub_strlen(scratch);
    rc = !grub_strncasecmp(str, scratch, clen);
    return rc;
}

static void
finish_param_parse(char *pos, char **end, char *tmp)
{
  if (!pos || !end || !tmp)
    return;

  if (*end)
    **end = *tmp;
}

static char *
get_next_param(char *pos, char **end, char *tmp)
{
  char *comma = NULL;
  char *openparen = NULL;
  char *closeparen = NULL;

  if (!pos || !end || !tmp)
    return NULL;

  if (*end)
    **end = *tmp;

  openparen = grub_strchr(pos, '(');
  if (openparen && *openparen)
    {
      pos = grub_strnchr(openparen + 1, ' ');
      comma = grub_strchr(pos, ',');
      closeparen = grub_strchr(pos, ')');

      if (comma && *comma)
	{
	  *tmp = *comma;
	  *comma = '\0';
	  *end = comma;
	}
      else if (closeparen && *closeparen)
	{
	  *tmp = *closeparen;
	  *closeparen = '\0';
	  *end = closeparen;
	}
      return pos;
    }

  comma = grub_strchr(pos, ',');
  if (comma && *comma)
    {
      pos = grub_strnchr(comma + 1, ' ');
      comma = grub_strchr(pos, ',');
      closeparen = grub_strchr(pos, ')');

      if (comma && *comma)
	{
	  *tmp = *comma;
	  *comma = '\0';
	  *end = comma;
	}
      else if (closeparen && *closeparen)
	{
	  *tmp = *closeparen;
	  *closeparen = '\0';
	  *end = closeparen;
	}
      return pos;
    }

  closeparen = grub_strchr(pos, ')');
  if (closeparen && *closeparen)
    pos = grub_strnchr(closeparen + 1, ' ');

  return pos;
}

struct generic_device_path
  {
    grub_efi_uint8_t type;
    grub_efi_uint8_t subtype;
    grub_efi_uint16_t length;
  } __attribute__((packed));

struct hd_media_device_path
  {
    grub_efi_uint8_t type;
    grub_efi_uint8_t subtype;
    grub_efi_uint16_t length;
    grub_efi_uint32_t partition;
    grub_efi_uint64_t startlba;
    grub_efi_uint64_t size;
    grub_efi_uint8_t signature[16];
    grub_efi_uint8_t mbr_type;
    grub_efi_uint8_t signature_type;
  } __attribute__((packed));

static inline int
parse_device_path_component(const char *orig_str, void *data)
{
  int orig_str_len = strlen(orig_str) + 1;
  char str[orig_str_len];
  char tmp;
  char *pos = str;
  int ret = 0;

  grub_strcpy(str, orig_str);
  if (dpname_matches(str, "pci"))
    {
    }
  else if (dpname_matches(str, "pccard"))
    {
    }
  else if (dpname_matches(str, "mmap"))
    {
    }
  else if (dpname_matches(str, "ctrl"))
    {
    }
  else if (dpname_matches(str, "acpi"))
    {
    }
    /* XXX what about _ADR? */
  /* messaging device paths */
  else if (dpname_matches(str, "atapi"))
    {
    }
  else if (dpname_matches(str, "scsi"))
    {
    }
  else if (dpname_matches(str, "fibrechannel"))
    {
    }
  else if (dpname_matches(str, "1394"))
    {
    }
  else if (dpname_matches(str, "usb"))
    {
    }
  else if (dpname_matches(str, "sata"))
    {
    }
    /* XXX what about usb-wwid */
    /* XXX what about lun */
  else if (dpname_matches(str, "usbclass"))
    {
    }
  else if (dpname_matches(str, "i2o"))
    {
    }
  else if (dpname_matches(str, "macaddr"))
    {
    }
  else if (dpname_matches(str, "ipv4"))
    {
    }
  else if (dpname_matches(str, "ipv6"))
    {
    }
    /* XXX what about vlan */
  else if (dpname_matches(str, "infiniband"))
    {
    }
  else if (dpname_matches(str, "uart"))
    {
    }
  else if (dpname_matches(str, "uartflowctrl"))
    {
    }
  else if (dpname_matches(str, "sas"))
    {
    }
  else if (dpname_matches(str, "iscsi"))
    {
    }
  /* media device paths */
  else if (dpname_matches(str, "hd"))
    {
      /* these look roughly like:
       *  HD(Partition,Type,Signature,Start, Size)
       * but:
       * - type may be optional.  1 or "MBR" means MBR. 2 or "GPT" means GPT.
       * - start and size are optional
       * - there can be random spaces
       */
      struct hd_media_device_path hddp;
      unsigned long tmpul;
      char *end = NULL, c;
      char tmps[19] = "0x";
      char *tmpsp;

      ret = 42;

      hddp.type = GRUB_EFI_MEDIA_DEVICE_PATH_TYPE;
      hddp.subtype = GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE;
      hddp.length = ret;

      //pos += grub_strcspn(pos, '(');
      pos = get_next_param(pos, &end, &c);
      if (!*pos)
	{
broken_hd:
	  finish_param_parse(pos, &end, &c);
	  return 0;
	}
      grub_strncpy(tmps+2, pos, 16);
      tmps[18] = '\0';
      tmpsp = tmps;
      safe_parse_maxulong(&tmpsp, &tmpul);
      hddp.partition = tmpul;

      pos = get_next_param(pos, &end, &c);
      if (!*pos)
	goto broken_hd;
      grub_strcpy(tmps+2, pos);
      tmpsp = tmps;
      safe_parse_maxulong(&tmpsp, &tmpul);
      hddp.startlba = tmpul;

      pos = get_next_param(pos, &end, &c);
      if (!*pos)
	goto broken_hd;
      grub_strcpy(tmps+2, pos);
      tmpsp = tmps;
      safe_parse_maxulong(&tmpsp, &tmpul);
      hddp.size = tmpul;

      pos = get_next_param(pos, &end, &c);
      if (!*pos)
	goto broken_hd;
      if (!grub_strcmp(pos, "None"))
	{
	  hddp.signature_type = 0;
	  grub_memset(hddp.signature, '\0', sizeof(hddp.signature));
	}
      else if (grub_strnlen(pos, 36) == 8)
        {
	  grub_efi_uint32_t tmpu32;
	  grub_strncpy(tmps+2, pos, 8);
	  tmps[10] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu32 = tmpul;
	  hddp.signature_type = 1;
	  grub_memcpy(hddp.signature, &tmpu32, sizeof(tmpu32));
	}
      else if (grub_strnlen(pos, 36) == 36)
	{
	  grub_efi_uint32_t tmpu32;
	  grub_efi_uint16_t tmpu16;
	  grub_efi_uint8_t tmpu8;

	  grub_strncpy(tmps+2, pos, 8);
	  tmps[10] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu32 = tmpul;
	  grub_memcpy(hddp.signature, &tmpu32, sizeof(tmpu32));

	  grub_strncpy(tmps+2, pos+9, 4);
	  tmps[6] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu16 = tmpul;
	  grub_memcpy(hddp.signature + 4, &tmpu16, sizeof(tmpu16));

	  grub_strncpy(tmps+2, pos+14, 4);
	  tmps[6] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu16 = tmpul;
	  grub_memcpy(hddp.signature + 6, &tmpu16, sizeof(tmpu16));

	  /* these are displayed like a u16, but they're a u8.  thanks. */
	  grub_strncpy(tmps+2, pos+19, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 8, &tmpu8, sizeof(tmpu8));
	  grub_strncpy(tmps+2, pos+21, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 9, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+24, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 10, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+26, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 11, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+28, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 12, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+30, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 13, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+32, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 14, &tmpu8, sizeof(tmpu8));

	  grub_strncpy(tmps+2, pos+34, 2);
	  tmps[4] = '\0';
	  tmpsp = tmps;
	  safe_parse_maxulong(&tmpsp, &tmpul);
	  tmpu8 = tmpul;
	  grub_memcpy(hddp.signature + 15, &tmpu8, sizeof(tmpu8));

	  hddp.signature_type = 2;
	}
      else
	goto broken_hd;

      hddp.mbr_type = hddp.signature_type;

      if (data)
	grub_memcpy(data, &hddp, sizeof(hddp));
    }
  else if (dpname_matches(str, "cd"))
    {
    }
  else if (dpname_matches(str, "file"))
    {
    }
  else if (dpname_matches(str, "protocol"))
    {
    }
    /* what about piwg firmware file? */
    /* what about piwg firmware volume? */
    /* what about relative offset media */
  else if (dpname_matches(str, "bios"))
    {
    }
  /* This is the end beautiful friend */
  else if (dpname_matches(str, "EndEntire$"))
    {
      struct generic_device_path gdp = {
	      .type = GRUB_EFI_END_DEVICE_PATH_TYPE,
	      .subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE,
	      .length = 4
      };
      ret = 4;
      if (data)
	grub_memmove(data, &gdp, sizeof(gdp));
    }
  else if (dpname_matches(str, "EndThis$"))
    {
      struct generic_device_path gdp = {
	      .type = GRUB_EFI_END_DEVICE_PATH_TYPE,
	      .subtype = GRUB_EFI_END_THIS_DEVICE_PATH_SUBTYPE,
	      .length = 4
      };
      ret = 4;
      if (data)
	grub_memmove(data, &gdp, sizeof(gdp));
    }
  else if (dpname_matches(str, "EndUnknown$"))
    {
      struct generic_device_path gdp = {
	      .type = GRUB_EFI_END_DEVICE_PATH_TYPE,
	      .subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE,
	      .length = 4
      };
      ret = 4;
      if (data)
	grub_memmove(data, &gdp, sizeof(gdp));
    }
  /* handle anything we didn't recognize */
  else if (dpname_matches(str, "vendor"))
    {
      /* needs to handle:
       * 1) hw vendor
       * 2) messaging vendor
       * 3) media vendor
       */
    }
  else
    {
    }

  return ret;
}

grub_efi_device_path_t *
device_path_from_utf8 (const char *device)
{
  grub_size_t device_len;
  grub_efi_device_path_t *dp = NULL;

  device_len = parse_device_path_component(device, dp);
  device_len += parse_device_path_component("EndEntire", dp);
  dp = grub_malloc(device_len);
  if (!dp)
    return NULL;
  device_len = parse_device_path_component(device, dp);
  device_len += parse_device_path_component("EndEntire",
				     (void *)((unsigned long)dp + device_len));


  return dp;
}
