/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2007 Intel Corp.
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

#ifdef SUPPORT_SERIAL

#include <grub/types.h>
#include <grub/misc.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>

#include <shared.h>
#include <serial.h>
#include <term.h>

struct grub_efiserial_data
{
  grub_efi_handle_t handle;
  grub_efi_serial_io_t *serial_io;
  struct grub_efiserial_data *next;
};

/* GUIDs.  */
static grub_efi_guid_t serial_io_guid = GRUB_EFI_SERIAL_IO_GUID;

static grub_efi_serial_io_t *serial_device = NULL;

static struct grub_efiserial_data *
make_devices (void)
{
  grub_efi_uintn_t num_handles;
  grub_efi_handle_t *handles;
  grub_efi_handle_t *handle;
  struct grub_efiserial_data *devices = 0;

  /* Find handles which support the serial io interface.  */
  handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL, &serial_io_guid,
				    0, &num_handles);
  if (! handles)
    return 0;

  /* Make a linked list of devices.  */
  for (handle = handles; num_handles--; handle++)
    {
      struct grub_efiserial_data *s;
      grub_efi_serial_io_t *sio;

      sio = grub_efi_open_protocol (*handle, &serial_io_guid,
				    GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
      if (! sio)
	/* This should not happen... Why?  */
	continue;

      s = grub_malloc (sizeof (*s));
      if (! s)
	{
	  /* Uggh.  */
	  grub_free (handles);
	  return 0;
	}

      s->handle = *handle;
      s->serial_io = sio;
      s->next = devices;
      devices = s;
    }

  grub_free (handles);

  return devices;
}

static void
free_devices (struct grub_efiserial_data *devices)
{
  struct grub_efiserial_data *p, *q;

  for (p = devices; p; p = q)
    {
      q = p->next;
      grub_free (p);
    }
}

static struct grub_efiserial_data *
get_device (struct grub_efiserial_data *devices, int num)
{
  struct grub_efiserial_data *s;

  for (s = devices; s && num; s = s->next, num--)
    ;

  if (num == 0)
    return s;

  return 0;
}

/* Fetch a key from a serial device.  */
int
serial_hw_fetch (void)
{
  grub_efi_status_t status;
  char buf[1];
  grub_efi_uintn_t buf_siz = sizeof (buf);
  grub_efi_uint32_t control;

  if (! serial_device)
    return -1;

  status = Call_Service_2 (serial_device->get_control_bits, serial_device, &control);
  if (status != GRUB_EFI_SUCCESS)
    return -1;
  if (control & GRUB_EFI_SERIAL_INPUT_BUFFER_EMPTY)
    return -1;
  status = Call_Service_3 (serial_device->read, serial_device, &buf_siz, buf);
  if (status != GRUB_EFI_SUCCESS)
    return -1;

  return buf[0];
}

/* Put a character to a serial device.  */
void
serial_hw_put (int c)
{
  grub_efi_status_t status;
  char ch = c;
  grub_efi_uintn_t buf_size = sizeof (ch);

  if (! serial_device)
    return;

  status = Call_Service_3 (serial_device->write, serial_device, &buf_size, &ch);
}

void
serial_hw_delay (void)
{
}

/* Get the port number of the unit UNIT, the port number is same as
   serial port unit number in EFI. */
unsigned short
serial_hw_get_port (int unit)
{
  return unit;
}

static inline grub_efi_uint8_t
efi_data_bits_from_8250_LCR (int word_len)
{
  switch (word_len & 0x3)
    {
    case 0:
      return 5;
    case 1:
      return 6;
    case 2:
      return 7;
    case 3:
      return 8;
    default:
      return 0;
    }
}

static inline grub_efi_stop_bits_t
efi_stop_bits_from_8250_LCR (int stop_bit_len)
{
  switch (stop_bit_len & 0x4)
    {
    case 0x0:
      return GRUB_EFI_ONE_STOP_BIT;
    case 0x4:
      return GRUB_EFI_TWO_STOP_BITS;
    default:
      return GRUB_EFI_DEFAULT_STOP_BITS;
    }
}

static inline grub_efi_parity_t
efi_parity_from_8250_LCR (int parity_8250)
{
  if (! (parity_8250 & 0x8))
    return GRUB_EFI_NO_PARITY;

  switch (parity_8250 & 0x30)
    {
    case 0x00:
      return GRUB_EFI_ODD_PARITY;
    case 0x10:
      return GRUB_EFI_EVEN_PARITY;
    case 0x20:
      return GRUB_EFI_MARK_PARITY;
    case 0x30:
      return GRUB_EFI_SPACE_PARITY;
    default:
      return GRUB_EFI_DEFAULT_PARITY;
    }
}

/* Initialize a serial device. In EFI, PORT is used to assign
   serial port unit number.  */
int
serial_hw_init (unsigned short port, unsigned int speed,
		int word_len, int parity, int stop_bit_len)
{
  grub_efi_serial_io_t *sio;
  struct grub_efiserial_data *devices, *s;
  grub_efi_parity_t efi_parity;
  grub_efi_stop_bits_t efi_stop_bits;
  grub_efi_uint8_t efi_data_bits;
  grub_efi_status_t status;
  int i;

  devices = make_devices ();
  s = get_device (devices, port);
  if (!s)
    return 0;
  sio = s->serial_io;
  free_devices (devices);

  efi_data_bits = efi_data_bits_from_8250_LCR (word_len);
  efi_parity = efi_parity_from_8250_LCR (parity);
  efi_stop_bits = efi_stop_bits_from_8250_LCR (stop_bit_len);

  status = Call_Service_1 (sio->reset, sio);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  status = Call_Service_7 (sio->set_attributes, sio, speed, 0, 0,
			   efi_parity, efi_data_bits, efi_stop_bits);
  if (status != GRUB_EFI_SUCCESS)
    return 0;

  serial_device = sio;
  /* Get rid of TERM_NEED_INIT from the serial terminal.  */
  for (i = 0; term_table[i].name; i++)
    if (grub_strcmp (term_table[i].name, "serial") == 0)
      {
	term_table[i].flags &= ~TERM_NEED_INIT;
	break;
      }

  return 1;
}

#endif /* SUPPORT_SERIAL */
