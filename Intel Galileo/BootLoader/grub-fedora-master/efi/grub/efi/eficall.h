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

#ifndef _EFI_CALL_INCLUDE_
#define _EFI_CALL_INCLUDE_ 1

#include <grub/types.h>

#ifndef __x86_64__
#ifdef EFI_FUNCTION_WRAPPER
#undef EFI_FUNCTION_WRAPPER
#endif
#endif

#ifdef  EFI_FUNCTION_WRAPPER
typedef long EFI_STATUS;

EFI_STATUS x64_call0 (unsigned long func);
EFI_STATUS x64_call1 (unsigned long func, unsigned long a);
EFI_STATUS x64_call2 (unsigned long func, unsigned long a, unsigned long b);
EFI_STATUS x64_call3 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c);
EFI_STATUS x64_call4 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c, unsigned long d);
EFI_STATUS x64_call5 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e);
EFI_STATUS x64_call6 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e, unsigned long f);
EFI_STATUS x64_call7 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e,
		      unsigned long f, unsigned long g);
EFI_STATUS x64_call8 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e,
		      unsigned long f, unsigned long g,
		      unsigned long h);
EFI_STATUS x64_call9 (unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e,
		      unsigned long f, unsigned long g,
		      unsigned long h, unsigned long i);
EFI_STATUS x64_call10(unsigned long func, unsigned long a,
		      unsigned long b, unsigned long c,
		      unsigned long d, unsigned long e,
		      unsigned long f, unsigned long g,
		      unsigned long h, unsigned long i,
		      unsigned long j);

#define Call_Service(func)                      x64_call0((unsigned long)func)

#define Call_Service_1(func,a)                  x64_call1((unsigned long)func, \
							  (unsigned long)a)

#define Call_Service_2(func,a,b)                x64_call2((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b)

#define Call_Service_3(func,a,b,c)              x64_call3((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c)

#define Call_Service_4(func,a,b,c,d)            x64_call4((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d)

#define Call_Service_5(func,a,b,c,d,e)          x64_call5((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e)

#define Call_Service_6(func,a,b,c,d,e,f)        x64_call6((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e,    \
							  (unsigned long)f)

#define Call_Service_7(func,a,b,c,d,e,f,g)      x64_call7((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e,    \
							  (unsigned long)f,    \
							  (unsigned long)g)

#define Call_Service_8(func,a,b,c,d,e,f,g,h)    x64_call8((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e,    \
							  (unsigned long)f,    \
							  (unsigned long)g,    \
							  (unsigned long)h)

#define Call_Service_9(func,a,b,c,d,e,f,g,h,i)  x64_call9((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e,    \
							  (unsigned long)f,    \
							  (unsigned long)g,    \
							  (unsigned long)h,    \
							  (unsigned long)i)

#define Call_Service_10(func,a,b,c,d,e,f,g,h,i,j) \
					       x64_call10((unsigned long)func, \
							  (unsigned long)a,    \
							  (unsigned long)b,    \
							  (unsigned long)c,    \
							  (unsigned long)d,    \
							  (unsigned long)e,    \
							  (unsigned long)f,    \
							  (unsigned long)g,    \
							  (unsigned long)h,    \
							  (unsigned long)i,    \
							  (unsigned long)j)

#else

typedef long EFI_STATUS;
#define Call_Service(func)                      func()
#define Call_Service_1(func,a)                  func(a)
#define Call_Service_2(func,a,b)                func(a,b)
#define Call_Service_3(func,a,b,c)              func(a,b,c)
#define Call_Service_4(func,a,b,c,d)            func(a,b,c,d)
#define Call_Service_5(func,a,b,c,d,e)          func(a,b,c,d,e)
#define Call_Service_6(func,a,b,c,d,e,f)        func(a,b,c,d,e,f)
#define Call_Service_7(func,a,b,c,d,e,f,g)      func(a,b,c,d,e,f,g)
#define Call_Service_8(func,a,b,c,d,e,f,g,h)    func(a,b,c,d,e,f,g,h)
#define Call_Service_9(func,a,b,c,d,e,f,g,h,i)  func(a,b,c,d,e,f,g,h,i)
#define Call_Service_10(func,a,b,c,d,e,f,g,h,i,j)  func(a,b,c,d,e,f,g,h,i,j)
#endif

#endif
