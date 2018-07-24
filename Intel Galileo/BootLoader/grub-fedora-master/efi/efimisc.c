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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301, USA.
 */

#include <config.h>
#include <grub/efi/efi.h>
#include <grub/efi/time.h>
#include <grub/efi/misc.h>
#include <grub/misc.h>

#include <shared.h>

unsigned long install_partition = 0x20000;
unsigned long boot_drive = 0x80;
int saved_entryno = 0;
char version_string[] = VERSION;
char config_file[128] = "/boot/grub/menu.lst";
unsigned short io_map[IO_MAP_SIZE];
struct apm_info apm_bios_info;
/* The key map.  */
unsigned short bios_key_map[KEY_MAP_SIZE + 1];
unsigned short ascii_key_map[KEY_MAP_SIZE + 1];

void
grub_efi_init (void)
{
  /* First of all, initialize the console so that GRUB can display
     messages.  */
  grub_console_init ();
  /* Initialize the memory management system.  */
  grub_efi_mm_init ();
  grub_efidisk_init ();
}

void
grub_efi_fini (void)
{
  grub_efidisk_fini ();
  grub_efi_mm_fini ();
  grub_console_fini ();
}

void *
grub_malloc (grub_size_t size)
{
  return grub_efi_allocate_pool (size);
}

void
grub_free (void *p)
{
  grub_efi_free_pool (p);
}

char *
grub_stpcpy (char *dest, const char *src)
{
  char *d = dest;
  const char *s = src;

  do
    *d++ = *s;
  while (*s++ != '\0');

  return d - 1;
}

void
grub_fatal (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  grub_vsprintf (NULL, fmt, ap);
  va_end (ap);

  grub_exit ();
}

void
grub_real_dprintf (const char *file, const int line, const char *condition,
		   const char *fmt, ...)
{
  va_list args;

  if (! debug)
    return;

  grub_printf ("%s:%d: ", file, line);
  va_start (args, fmt);
  grub_vsprintf (0, fmt, args);
  va_end (args);
}

grub_size_t
grub_utf8_char_len(grub_uint8_t ch)
{
  return ((0xe5000000 >> ((ch >> 3) & 0x1e)) & 3) + 1;
}

#define UTF8_SHIFT_AND_MASK(unicode, byte)  (unicode)<<=6; (unicode) |= (0x3f & (byte))

/* convert utf8 to utf32 */
grub_uint32_t
grub_utf8_to_utf32(const grub_uint8_t *src, grub_size_t length)
{
  grub_uint32_t unicode;

  switch (length)
    {
    case 1:
      return src[0];
    case 2:
      unicode = src[0] & 0x1f;
      UTF8_SHIFT_AND_MASK(unicode, src[1]);
      return unicode;
    case 3:
      unicode = src[0] & 0x0f;
      UTF8_SHIFT_AND_MASK(unicode, src[1]);
      UTF8_SHIFT_AND_MASK(unicode, src[2]);
      return unicode;
    case 4:
      unicode = src[0] & 0x07;
      UTF8_SHIFT_AND_MASK(unicode, src[1]);
      UTF8_SHIFT_AND_MASK(unicode, src[2]);
      UTF8_SHIFT_AND_MASK(unicode, src[3]);
      return unicode;
    default:
      return 0xffff;
    }
}

/* convert utf8 to utf16 */
void
grub_utf8_to_utf16(const grub_uint8_t *src, grub_size_t srclen,
		   grub_uint16_t *dst, grub_size_t dstlen)
{
  const grub_uint8_t *end = src + srclen;
  grub_efi_char16_t *dstend = dst + dstlen;

  while (src < end && dst < dstend)
    {
      grub_size_t len = grub_utf8_char_len(*src);
      /* get the utf32 codepoint */
      grub_uint32_t codepoint = grub_utf8_to_utf32(src, len);

      /* convert that codepoint to utf16 codepoints */
      if (codepoint <= 0xffff)
	{
	  /* it's a single utf16 character */
	  *dst++ = (grub_efi_char16_t) codepoint;
	}
      else
	{
	  /* it's multiple utf16 characters, with surrogate pairs */
	  codepoint = codepoint - 0x10000;
	  *dst++ = (grub_efi_char16_t) ((codepoint >> 10) + 0xd800);
	  *dst++ = (grub_efi_char16_t) ((codepoint & 0x3ff) + 0xdc00);
	}

	src += len;
    }

  if (dst < dstend)
    *dst = 0;
}

/* Convert UTF-16 to UTF-8.  */
grub_uint8_t *
grub_utf16_to_utf8 (grub_uint8_t *dest, grub_uint16_t *src,
		    grub_size_t size)
{
  grub_uint32_t code_high = 0;

  while (size--)
    {
      grub_uint32_t code = *src++;

      if (code_high)
	{
	  if (code >= 0xDC00 && code <= 0xDFFF)
	    {
	      /* Surrogate pair.  */
	      code = ((code_high - 0xD800) << 12) + (code - 0xDC00) + 0x10000;

	      *dest++ = (code >> 18) | 0xF0;
	      *dest++ = ((code >> 12) & 0x3F) | 0x80;
	      *dest++ = ((code >> 6) & 0x3F) | 0x80;
	      *dest++ = (code & 0x3F) | 0x80;
	    }
	  else
	    {
	      /* Error...  */
	      *dest++ = '?';
	    }

	  code_high = 0;
	}
      else
	{
	  if (code <= 0x007F)
	    *dest++ = code;
	  else if (code <= 0x07FF)
	    {
	      *dest++ = (code >> 6) | 0xC0;
	      *dest++ = (code & 0x3F) | 0x80;
	    }
	  else if (code >= 0xD800 && code <= 0xDBFF)
	    {
	      code_high = code;
	      continue;
	    }
	  else if (code >= 0xDC00 && code <= 0xDFFF)
	    {
	      /* Error... */
	      *dest++ = '?';
	    }
	  else
	    {
	      *dest++ = (code >> 16) | 0xE0;
	      *dest++ = ((code >> 12) & 0x3F) | 0x80;
	      *dest++ = (code & 0x3F) | 0x80;
	    }
	}
    }

  return dest;
}

/* low-level timing info */
int
getrtsecs (void)
{
  return grub_get_rtc() / GRUB_TICKS_PER_SECOND;
}

void
grub_reboot (void)
{
  grub_efi_runtime_services_t *r;

  r = grub_efi_system_table->runtime_services;
  Call_Service_4 (r->reset_system, GRUB_EFI_RESET_COLD,
		  GRUB_EFI_SUCCESS, 0, NULL);
  for (;;);
}

void
grub_halt (int no_apm)
{
  grub_efi_runtime_services_t *r;

  r = grub_efi_system_table->runtime_services;
  Call_Service_4 (r->reset_system, GRUB_EFI_RESET_SHUTDOWN,
		  GRUB_EFI_SUCCESS, 0, NULL);
  for (;;);
}

void
stop (void)
{
  grub_exit ();
}

/* booting a multiboot executable */
void
multi_boot (int start, int mb_info)
{
  stop ();
}

/* sets it to linear or wired A20 operation */
void
gateA20 (int linear)
{
}

/* Set up the int15 handler.  */
void
set_int15_handler (void)
{
}

/* Restore the original int15 handler.  */
void
unset_int15_handler (void)
{
}

/* Copy MAP to the drive map and set up the int13 handler.  */
void
set_int13_handler (unsigned short *map)
{
}

/* Get the ROM configuration table.  */
unsigned long
get_rom_config_table (void)
{
  return 0;
}

/* Get APM BIOS information.  */
void
get_apm_info (void)
{
  /* Nothing to do in the simulator.  */
}

void
stop_floppy (void)
{
  /* NOTUSED */
}

char *
grub_strndup (const char *s, int n)
{
  int l = grub_strnlen(s, n);
  char *new = grub_malloc(l + 1);

  if (new == NULL)
    return NULL;

  new[l] = '\0';
  return grub_strncpy(new, s, l);
}

int
safe_parse_maxulong (char **str_ptr, unsigned long *myulong_ptr)
{
  char *ptr = *str_ptr;
  unsigned long myulong = 0;
  unsigned int mult = 10, found = 0;

  /*
   *  Is this a hex number?
   */
  if (*ptr == '0' && tolower (*(ptr + 1)) == 'x')
    {
      ptr += 2;
      mult = 16;
    }

  while (1)
    {
      /* A bit tricky. This below makes use of the equivalence:
	 (A >= B && A <= C) <=> ((A - B) <= (C - B))
	 when C > B and A is unsigned.  */
      unsigned int digit;

      digit = tolower (*ptr) - '0';
      if (digit > 9)
	{
	  digit -= 'a' - '0';
	  if (mult == 10 || digit > 5)
	    break;
	  digit += 10;
	}

      found = 1;
      if (myulong > ((~0UL - digit) / mult))
	{
	  errnum = ERR_NUMBER_OVERFLOW;
	  return 0;
	}
      myulong = (myulong * mult) + digit;
      ptr++;
    }

  if (!found)
    {
      errnum = ERR_NUMBER_PARSING;
      return 0;
    }

  *str_ptr = ptr;
  *myulong_ptr = myulong;

  return 1;
}

int
currticks (void)
{
  return grub_get_rtc ();
}

static char *
fix_path_name (char *path_name)
{
  char *p1, *p2;

  p1 = path_name;
  p2 = p1 + 1;
  if (*p1 == '\\')
    *p1 = '/';
  while (*p1)
    {
      switch (*p2)
	{
	case '\0':
	  *++p1 = *p2;
	  break;
	case '.':
	  if (*p1 == '/' && *(p2+1) == '\\')
	    p2 += 2;
	  else
	    *++p1=*p2++;
	  break;
	case '\\':
	  if (*p1 == '/')
	    p2++;
	  else
	    *++p1 = '/';
	  break;
	default:
	  *++p1=*p2++;
	  break;
	}
    }
  return path_name;
}

char *
grub_efi_file_path_to_path_name (grub_efi_device_path_t *file_path)
{
  char *str;
  grub_efi_uint32_t str_len = 16;
  grub_efi_uint32_t str_pos = 0;

  str = grub_malloc (str_len);
  if (! str)
    return NULL;
  str[0] = '\0';

  while (1)
    {
      grub_efi_uint8_t type = GRUB_EFI_DEVICE_PATH_TYPE (file_path);
      grub_efi_uint8_t subtype = GRUB_EFI_DEVICE_PATH_SUBTYPE (file_path);
      grub_efi_uint16_t len = GRUB_EFI_DEVICE_PATH_LENGTH (file_path);

      switch (type)
	{
	case GRUB_EFI_MEDIA_DEVICE_PATH_TYPE:
	  switch (subtype)
	    {
	    case GRUB_EFI_FILE_PATH_DEVICE_PATH_SUBTYPE:
	      {
		grub_efi_file_path_device_path_t *fp;
		grub_uint8_t buf[(len - 4) * 2 + 2];
		grub_uint32_t path_name_len;
		char *tmp_str;

		fp = (grub_efi_file_path_device_path_t *) file_path;
		*grub_utf16_to_utf8 (buf, fp->path_name,
				     (len - 4) / sizeof (grub_efi_char16_t))
		  = '\0';
		path_name_len = strlen ((char *)buf) + 1;
		if ((str_len - str_pos) <= path_name_len)
		  {
		    do
		      str_len *= 2;
		    while ((str_len - str_pos) <= path_name_len);
		    tmp_str = grub_malloc (str_len);
		    if (tmp_str == NULL)
		      goto fail;
		    grub_memmove (tmp_str, str, str_pos);
		    grub_free (str);
		    str = tmp_str;
		  }
		str[str_pos] = '\\';
		strcpy (str + str_pos + 1, (char *)buf);
		str_pos += path_name_len;
	      }
	      break;
	    default:
	      break;
	    }
	  break;
	default:
	  break;
	}

      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (file_path))
	break;

      file_path = (grub_efi_device_path_t *) ((char *) file_path + len);
    }
  return fix_path_name (str);

 fail:
  grub_free (str);
  return NULL;
}

#define DEFAULT_SAVED_DEFAULT_FILE_NAME		"grub.default"
#define DEFAULT_CONFIG_FILE_NAME		"grub.conf"

static char saved_default_file[128] = "/boot/grub/grub.default";

void
grub_set_config_file (char *path_name)
{
  char *dir_end;
  grub_uint32_t path_name_len;

  path_name_len = strlen (path_name);
  if (path_name_len > 4
      && path_name[path_name_len - 4] == '.'
      && grub_tolower (path_name[path_name_len - 3]) == 'e'
      && grub_tolower (path_name[path_name_len - 2]) == 'f'
      && grub_tolower (path_name[path_name_len - 1]) == 'i')
    {
      /* Bigger than buffer of config_file */
      if (path_name_len + 1 > 127)
	return;
      grub_memmove (config_file, path_name, path_name_len - 4);
      grub_strcpy (config_file + path_name_len - 4, ".conf");
      /* Bigger than buffer of default file */
      if (path_name_len + 4 > 127)
	return;
      grub_memmove (saved_default_file, path_name, path_name_len - 4);
      grub_strcpy (saved_default_file + path_name_len - 4, ".default");
      return;
    }
  dir_end = grub_strrchr (path_name, '/');
  if (! dir_end)
    {
      grub_strcpy (config_file, DEFAULT_CONFIG_FILE_NAME);
      grub_strcpy (saved_default_file, DEFAULT_SAVED_DEFAULT_FILE_NAME);
      return;
    }
  if (strlen(dir_end) == 1) {
    path_name_len = dir_end + 1 - path_name;
    if (path_name_len + sizeof (DEFAULT_CONFIG_FILE_NAME) > 128)
      return;
    grub_memmove (config_file, path_name, path_name_len);
    grub_strcpy (config_file + path_name_len, DEFAULT_CONFIG_FILE_NAME);
  } else {
    grub_memmove (config_file, path_name, path_name_len+1);
  }
  if (path_name_len + sizeof (DEFAULT_SAVED_DEFAULT_FILE_NAME) > 128)
    return;
  path_name_len = dir_end + 1 - path_name;
  grub_memmove (saved_default_file, path_name, path_name_len);
  grub_strcpy (saved_default_file + path_name_len,
	       DEFAULT_SAVED_DEFAULT_FILE_NAME);
}

grub_efi_guid_t simple_file_system_guid = GRUB_EFI_SIMPLE_FILE_SYSTEM_GUID;

static grub_efi_file_t *
simple_open_file(grub_efi_handle_t dev_handle,
		 char *file_name,
		 int for_write)
{
  grub_efi_simple_file_system_t *file_system;
  grub_efi_file_t *root;
  grub_efi_file_t *file = NULL;
  grub_efi_status_t status;
  grub_efi_char16_t *file_name_w = NULL;
  grub_efi_char16_t *chp_w;
  grub_efi_uint64_t open_mode;

  file_system = grub_efi_open_protocol (dev_handle,
					&simple_file_system_guid,
					GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (! file_system)
    return NULL;
  status = Call_Service_2 (file_system->open_volume, file_system, &root);
  if (status != GRUB_EFI_SUCCESS)
    return NULL;
  file_name_w = grub_malloc (2 * strlen(file_name) + 2);
  if (! file_name_w)
    goto done;
  for (chp_w = file_name_w; *file_name; chp_w++, file_name++)
    if (*file_name == '/')
      *chp_w = '\\';
    else
      *chp_w = *file_name;
  *chp_w = '\0';
  open_mode = for_write ? GRUB_EFI_FILE_MODE_READ | GRUB_EFI_FILE_MODE_WRITE \
    | GRUB_EFI_FILE_MODE_CREATE : GRUB_EFI_FILE_MODE_READ;
  status = Call_Service_5 (root->open, root, &file, file_name_w,
			   open_mode, 0);
 done:
  if (file_name_w)
    grub_free (file_name_w);
  Call_Service_1 (root->close, root);
  return file;
}

void
grub_load_saved_default (grub_efi_handle_t dev_handle)
{
  grub_efi_file_t *file;
  char buf[16];
  char *ptr = buf;
  grub_efi_status_t status;
  int val;
  grub_efi_uintn_t buf_size = sizeof(buf);

  file = simple_open_file (dev_handle, saved_default_file, 0);
  if (! file)
    return;
  status = Call_Service_3 (file->read, file, &buf_size, buf);
  if (status != GRUB_EFI_SUCCESS)
    goto done;
  if (buf_size >= sizeof(buf))
    buf_size = sizeof(buf) - 1;
  buf[buf_size] = '\0';
  if (safe_parse_maxint (&ptr, &val))
    saved_entryno = val;
 done:
  Call_Service_1 (file->close, file);
}

int
grub_save_saved_default (int new_default)
{
  grub_efi_loaded_image_t *loaded_image;
  grub_efi_file_t *file;
  char buf[16];
  grub_efi_status_t status;
  grub_efi_uintn_t buf_size;
  int ret = 0;

  loaded_image = grub_efi_get_loaded_image (grub_efi_image_handle);
  file = simple_open_file (loaded_image->device_handle,
			   saved_default_file, 1);
  if (! file)
    {
      errnum = ERR_FILE_NOT_FOUND;
      return 1;
    }
  sprintf (buf, "%d", new_default);
  buf_size = strlen (buf);
  status = Call_Service_3 (file->write, file, &buf_size, buf);
  if (status != GRUB_EFI_SUCCESS)
    {
      errnum = ERR_WRITE;
      ret = 1;
      goto done;
    }
 done:
  Call_Service_1 (file->close, file);
  return ret;
}
