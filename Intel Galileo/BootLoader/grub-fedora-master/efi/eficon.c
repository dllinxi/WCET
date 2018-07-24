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

#include <grub/misc.h>
#include <grub/types.h>
#include <grub/efi/efi.h>
#include <grub/efi/api.h>
#include <grub/efi/misc.h>

#include <shared.h>
#include <term.h>

static int
grub_console_standard_color = GRUB_EFI_TEXT_ATTR (GRUB_EFI_YELLOW,
						  GRUB_EFI_BACKGROUND_BLACK);
static int
grub_console_normal_color = GRUB_EFI_TEXT_ATTR (GRUB_EFI_LIGHTGRAY,
						GRUB_EFI_BACKGROUND_BLACK);
static int
grub_console_highlight_color = GRUB_EFI_TEXT_ATTR (GRUB_EFI_BLACK,
						   GRUB_EFI_BACKGROUND_LIGHTGRAY);

static int read_key = -1;

void
console_putchar (int c)
{
  grub_efi_char16_t str[2];
  grub_efi_simple_text_output_interface_t *o;

  o = grub_efi_system_table->con_out;

  switch (c)
    {
    case DISP_LEFT:
      c = '<';
      break;

    case DISP_UP:
      c = '^';
      break;

    case DISP_RIGHT:
      c = '>';
      break;

    case DISP_DOWN:
      c = 'v';
      break;

    case DISP_HORIZ:
      c = '-';
      break;

    case DISP_VERT:
      c = '|';
      break;

    case DISP_UL:
    case DISP_UR:
    case DISP_LL:
    case DISP_LR:
      c = '+';
      break;
    }

  /* For now, do not try to use a surrogate pair.  */
  if (c > 0xffff)
    c = '?';

  str[0] = (grub_efi_char16_t)  (c & 0xffff);
  str[1] = 0;

  /* Should this test be cached?  */
  if (c > 0x7f && Call_Service_2 (o->test_string, o, str) != GRUB_EFI_SUCCESS)
    return;

  Call_Service_2 (o->output_string, o, str);
}

int
console_checkkey (void)
{
  grub_efi_simple_input_interface_t *i;
  grub_efi_input_key_t key;
  grub_efi_status_t status;

  if (read_key >= 0)
    return 1;

  i = grub_efi_system_table->con_in;
  status = Call_Service_2 (i->read_key_stroke ,i, &key);
#if 0
  switch (status)
    {
    case GRUB_EFI_SUCCESS:
      {
	int xy;

	xy = console_getxy ();
	console_gotoxy (0, 0);
	grub_printf ("scan_code=%x,unicode_char=%x  ",
		     (unsigned) key.scan_code,
		     (unsigned) key.unicode_char);
	console_gotoxy (xy >> 8, xy & 0xff);
      }
      break;

    case GRUB_EFI_NOT_READY:
      grub_printf ("not ready   ");
      break;

    default:
      grub_printf ("device error   ");
      break;
    }
#endif

  if (status == GRUB_EFI_SUCCESS)
    {
      switch (key.scan_code)
	{
	case 0x00:
	  read_key = key.unicode_char;
	  break;
	case 0x01:
	  read_key = 16;
	  break;
	case 0x02:
	  read_key = 14;
	  break;
	case 0x03:
	  read_key = 6;
	  break;
	case 0x04:
	  read_key = 2;
	  break;
	case 0x05:
	  read_key = 1;
	  break;
	case 0x06:
	  read_key = 5;
	  break;
	case 0x07:
	  break;
	case 0x08:
	  read_key = 4;
	  break;
	case 0x09:
	  break;
	case 0x0a:
	  break;
	case 0x17:
	  read_key = '\e';
	  break;
	default:
	  break;
	}
    }

  return read_key;
}

int
console_getkey (void)
{
  grub_efi_simple_input_interface_t *i;
  grub_efi_boot_services_t *b;
  grub_efi_uintn_t index;
  grub_efi_status_t status;
  int key;

  if (read_key >= 0)
    {
      key = read_key;
      read_key = -1;
      return key;
    }

  i = grub_efi_system_table->con_in;
  b = grub_efi_system_table->boot_services;

  do
    {
      status = Call_Service_3 (b->wait_for_event , 1, &(i->wait_for_key), &index);
      if (status != GRUB_EFI_SUCCESS)
      	return -1;

      console_checkkey ();
    }
  while (read_key < 0);

  key = read_key;
  read_key = -1;
  return key;
}

int
console_keystatus (void)
{
  /* Doesn't look like EFI can support this... */
  return 0;
}

int
console_getxy (void)
{
  grub_efi_simple_text_output_interface_t *o;

  o = grub_efi_system_table->con_out;
  return ((o->mode->cursor_column << 8) | o->mode->cursor_row);
}

void
console_gotoxy (int x, int y)
{
  grub_efi_simple_text_output_interface_t *o;

  o = grub_efi_system_table->con_out;
  Call_Service_3 (o->set_cursor_position , o, x, y);
}

void
console_cls (void)
{
  grub_efi_simple_text_output_interface_t *o;
  grub_efi_int32_t orig_attr;

  o = grub_efi_system_table->con_out;
  orig_attr = o->mode->attribute;
  Call_Service_2 (o->set_attributes, o, GRUB_EFI_BACKGROUND_BLACK);
  Call_Service_1 (o->clear_screen , o);
  Call_Service_2 (o->set_attributes , o, orig_attr);
}

void
console_setcolorstate (color_state state)
{
  grub_efi_simple_text_output_interface_t *o;

  o = grub_efi_system_table->con_out;

  switch (state) {
    case COLOR_STATE_STANDARD:
      Call_Service_2 (o->set_attributes, o, grub_console_standard_color);
      break;
    case COLOR_STATE_NORMAL:
      Call_Service_2 (o->set_attributes, o, grub_console_normal_color);
      break;
    case COLOR_STATE_HIGHLIGHT:
      Call_Service_2 (o->set_attributes, o, grub_console_highlight_color);
      break;
    default:
      break;
  }
}

void
console_setcolor (int normal_color, int highlight_color)
{
  grub_console_normal_color = normal_color;
  grub_console_highlight_color = highlight_color;
}

int
console_setcursor (int on)
{
  grub_efi_simple_text_output_interface_t *o;

  o = grub_efi_system_table->con_out;
  Call_Service_2 (o->enable_cursor, o, on);
  return on;
}

void
grub_console_init (void)
{
  /* FIXME: it is necessary to consider the case where no console control
     is present but the default is already in text mode.  */
  if (! grub_efi_set_text_mode (1))
    {
      grub_printf ("cannot set text mode");
      return;
    }
}

void
grub_console_fini (void)
{
}
