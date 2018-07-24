/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000,2001,2002  Free Software Foundation, Inc.
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

/* Based on "src/misc.c" in etherboot-5.0.5.  */

#define GRUB	1
#include <etherboot.h>
#include <stdarg.h>

void
sleep (int secs)
{
  unsigned long tmo = currticks () + secs;

  while (currticks () < tmo)
    ;
}

void
twiddle (void)
{
  static unsigned long lastticks = 0;
  static int count = 0;
  static const char tiddles[]="-\\|/";
  unsigned long ticks;

  if (debug)
    {
      if ((ticks = currticks ()) == lastticks)
	return;
      
      lastticks = ticks;
      grub_putchar (tiddles[(count++) & 3]);
      grub_putchar ('\b');
    }
}

/* Because Etherboot uses its own formats for the printf family,
   define separate definitions from GRUB.  */
/**************************************************************************
PRINTF and friends

	Formats:
		%[#]x	- 4 bytes long (8 hex digits, lower case)
		%[#]X	- 4 bytes long (8 hex digits, upper case)
		%[#]hx	- 2 bytes int (4 hex digits, lower case)
		%[#]hX	- 2 bytes int (4 hex digits, upper case)
		%[#]hhx	- 1 byte int (2 hex digits, lower case)
		%[#]hhX	- 1 byte int (2 hex digits, upper case)
			- optional # prefixes 0x or 0X
		%d	- decimal int
		%c	- char
		%s	- string
		%@	- Internet address in ddd.ddd.ddd.ddd notation
		%!	- Ethernet address in xx:xx:xx:xx:xx:xx notation
	Note: width specification not supported
**************************************************************************/
static int
etherboot_vsprintf (char *buf, const char *fmt, va_list args)
{
  char *p, *s;
  int n;
  
  s = buf;
  for ( ; *fmt != '\0'; ++fmt)
    {
      if (*fmt != '%')
	{
	  buf ? *s++ = *fmt : grub_putchar (*fmt);
	  continue;
	}
      
      if (*++fmt == 's')
	{
	  for (p = va_arg (args, char *); *p != '\0'; p++)
	    buf ? *s++ = *p : grub_putchar (*p);
	}
      else
	{
	  /* Length of item is bounded */
	  char tmp[20], *q = tmp;
	  int alt = 0;
	  int shift = 28;
	  
	  if (*fmt == '#')
	    {
	      alt = 1;
	      fmt++;
	    }
	  
	  if (*fmt == 'h')
	    {
	      shift = 12;
	      fmt++;
	    }
	  
	  if (*fmt == 'h')
	    {
	      shift = 4;
	      fmt++;
	    }
	  
	  /*
	   * Before each format q points to tmp buffer
	   * After each format q points past end of item
	   */
	  if ((*fmt | 0x20) == 'x')
	    {
	      int ncase = (*fmt & 0x20);

	      n = va_arg (args, int);
	      
	      if (alt)
		{
		  *q++ = '0';
		  *q++ = 'X' | ncase;
		}
	      for (; shift >= 0; shift -= 4)
		*q++ = "0123456789ABCDEF"[(n >> shift) & 0xF] | ncase;
	    }
	  else if (*fmt == 'd')
	    {
	      char *r;

	      n = va_arg (args, int);
	      if (n < 0)
		{
		  *q++ = '-';
		  n = -n;
		}
	      
	      p = q;		/* save beginning of digits */
	      do
		{
		  *q++ = '0' + (n % 10);
		  n /= 10;
		}
	      while (n);
	      
	      /* reverse digits, stop in middle */
	      r = q;		/* don't alter q */
	      while (--r > p)
		{
		  n = *r;
		  *r = *p;
		  *p++ = n;
		}
	    }
	  else if (*fmt == '@')
	    {
	      unsigned char *r;
	      union
	      {
		int		l;
		unsigned char	c[4];
	      }
	      u;
	      
	      u.l = va_arg (args, int);
	      
	      for (r = &u.c[0]; r < &u.c[4]; ++r)
		q += etherboot_sprintf (q, "%d.", *r);
	      
	      --q;
	    }
	  else if (*fmt == '!')
	    {
	      char *r;

	      p = va_arg (args, char *);
	      for (r = p + ETH_ALEN; p < r; ++p)
		q += etherboot_sprintf (q, "%hhX:", *p);
	      
	      --q;
	    }
	  else if (*fmt == 'c')
	    *q++ = va_arg (args, int);
	  else
	    *q++ = *fmt;
	  
	  /* now output the saved string */
	  for (p = tmp; p < q; ++p)
	    buf ? *s++ = *p : grub_putchar (*p);
	}
    }
  
  if (buf)
    *s = '\0';
  
  return (s - buf);
}

int
etherboot_sprintf (char *buf, const char *fmt, ...)
{
  va_list ap;
  int ret;

  va_start (ap, fmt);
  ret = etherboot_vsprintf (buf, fmt, ap);
  va_end (ap);
  return ret;
}

void
etherboot_printf (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  (void) etherboot_vsprintf (0, fmt, ap);
  va_end (ap);
}

int
inet_aton (char *p, in_addr *addr)
{
  unsigned long ip = 0;
  int val;
  int i;
  
  for (i = 0; i < 4; i++)
    {
      val = getdec (&p);
      
      if (val < 0 || val > 255)
	return 0;
      
      if (i != 3 && *p++ != '.')
	return 0;
      
      ip = (ip << 8) | val;
    }

  addr->s_addr = htonl (ip);

  return 1;
}

int
getdec (char **ptr)
{
  char *p = *ptr;
  int ret = 0;
  
  if (*p < '0' || *p > '9')
    return -1;
  
  while (*p >= '0' && *p <= '9')
    {
      ret = ret * 10 + (*p - '0');
      p++;
    }
  
  *ptr = p;
  
  return ret;
}
