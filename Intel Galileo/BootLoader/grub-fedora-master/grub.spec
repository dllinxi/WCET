Name: grub
Version: 0.97
Release: 20%{?dist}
Summary: GRUB - the Grand Unified Boot Loader.
Group: System Environment/Base
License: GPLv2+

ExclusiveArch: i386 x86_64
BuildRequires: binutils >= 2.9.1.0.23, ncurses-devel, ncurses-static, texinfo
BuildRequires: autoconf /usr/lib/crt1.o automake
PreReq: /sbin/install-info
Requires: mktemp
Requires: /usr/bin/cmp
Requires: system-logos
BuildRoot: %{_tmppath}/%{name}-%{version}-root

URL: http://www.gnu.org/software/%{name}/
Source0: ftp://alpha.gnu.org/gnu/%{name}/%{name}-%{version}.tar.gz
Patch0: grub-fedora-9.patch

%description
GRUB (Grand Unified Boot Loader) is an experimental boot loader
capable of booting into most free operating systems - Linux, FreeBSD,
NetBSD, GNU Mach, and others as well as most commercial operating
systems.

%prep
%setup -q
%patch0 -p1 -b .fedora-9

%build
autoreconf
autoconf
GCCVERS=$(gcc --version | head -1 | cut -d\  -f3 | cut -d. -f1)
CFLAGS="-Os -g -fno-strict-aliasing -Wall -Werror -Wno-shadow -Wno-unused"
if [ "$GCCVERS" == "4" ]; then
	CFLAGS="$CFLAGS -Wno-pointer-sign"
fi
export CFLAGS
%ifarch x86_64
%configure --sbindir=/sbin --disable-auto-linux-mem-opt --datarootdir=%{_datadir} --with-platform=efi
make
rm -fr $RPM_BUILD_ROOT
%makeinstall sbindir=${RPM_BUILD_ROOT}/sbin
mv ${RPM_BUILD_ROOT}/sbin/grub ${RPM_BUILD_ROOT}/sbin/grub-efi
make clean
autoreconf
autoconf
CFLAGS="$CFLAGS -static" 
export CFLAGS
%endif
%configure --sbindir=/sbin --disable-auto-linux-mem-opt --datarootdir=%{_datadir}
make

%install
%makeinstall sbindir=${RPM_BUILD_ROOT}/sbin
mkdir -p ${RPM_BUILD_ROOT}/boot/grub

rm -f ${RPM_BUILD_ROOT}/%{_infodir}/dir

%clean
rm -fr $RPM_BUILD_ROOT

%post
if [ "$1" = 1 ]; then
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/grub.info.gz || :
  /sbin/install-info --info-dir=%{_infodir} %{_infodir}/multiboot.info.gz || :
fi

%preun
if [ "$1" = 0 ] ;then
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/grub.info.gz || :
  /sbin/install-info --delete --info-dir=%{_infodir} %{_infodir}/multiboot.info.gz || :
fi

%files
%defattr(-,root,root)
%doc AUTHORS ChangeLog NEWS README COPYING TODO docs/menu.lst
/boot/grub
/sbin/grub
/sbin/grub-install
/sbin/grub-terminfo
/sbin/grub-md5-crypt
%{_bindir}/mbchk
%{_infodir}/grub*
%{_infodir}/multiboot*
%{_mandir}/man*/*
%{_datadir}/grub
%ifarch x86_64
/sbin/grub-efi
%endif

%changelog
* Mon Nov 05 2007 Peter Jones <pjones@redhat.com> - 0.97-20
- Add EFI support from Intel on x86_64

* Thu Sep 20 2007 Peter Jones <pjones@redhat.com> - 0.97-19
- Fix dmraid detection on Intel (isw) controllers in grub-install .

* Wed Aug 22 2007 Peter Jones <pjones@redhat.com> - 0.97-18
- Fix license tag.

* Mon Aug 20 2007 Peter Jones <pjones@redhat.com> - 0.97-17
- Use --build-id=none instead of stripping out the build-id notes in the
  first and second stage loaders.

* Tue Aug 7 2007 Peter Jones <pjones@redhat.com> - 0.97-16
- Add ext[23] large inode support (patch from Eric Sandeen)
- Fix auto* breakage that happened when we switched from autoreconf to autoconf
- Move to original tarball + patch generated from git

* Mon Jul 16 2007 Peter Jones <pjones@redhat.com> - 0.97-15
- Support booting from GPT

* Fri Feb 23 2007 Bill Nottingham <notting@redhat.com> - 0.97-14
- fix scriplet errors when installed with --nodocs
- coax grub into building (-ltinfo, autoconf instead of autoreconf)

* Sun Oct 01 2006 Jesse Keating <jkeating@redhat.com> - 0.97-13
- rebuilt for unwind info generation, broken in gcc-4.1.1-21

* Thu Sep 21 2006 Peter Jones <pjones@redhat.com> - 0.97-12
- Reenable patch 505, which fixes #116311

* Tue Aug 15 2006 Peter Jones <pjones@redhat.com> - 0.97-11
- Disable patch 505 (#164497)

* Wed Aug  2 2006 Peter Jones <pjones@redhat.com> - 0.97-10
- Fix grub-install for multipath

* Wed Jul 12 2006 Jesse Keating <jkeating@redhat.com> - 0.97-9.1
- rebuild

* Fri Jul  7 2006 Peter Jones <pjones@redhat.com> - 0.97-9
- fix broken error reporting from helper functions

* Mon Jun 12 2006 Peter Jones <pjones@redhat.com> - 0.97-8
- Fix BIOS keyboard handler to use extended keyboard interrupts, so the
  Mac Mini works.

* Mon Jun  5 2006 Jesse Keating <jkeating@redhat.com> - 0.97-7
- Added BuildRequires on a 32bit library

* Sat May 27 2006 Peter Jones <pjones@redhat.com> - 0.97-6
- Fix mactel keyboard problems, patch from Juergen Keil, forwarded by Linus.

* Mon Mar 13 2006 Peter Jones <pjones@redhat.com> - 0.97-5
- Fix merge error for "bootonce" patch (broken in 0.95->0.97 update)
- Get rid of the 0.97 "default" stuff, since it conflicts with our working
  method.

* Mon Mar  9 2006 Peter Jones <pjones@redhat.com> - 0.97-4
- Fix running "install" multiple times on the same fs in the same invocation
  of grub.  (bz #158426 , patch from lxo@redhat.com)

* Mon Feb 13 2006 Peter Jones <pjones@redhat.com> - 0.97-3
- fix partition names on dmraid

* Tue Feb 07 2006 Jesse Keating <jkeating@redhat.com> - 0.97-2.1
- rebuilt for new gcc4.1 snapshot and glibc changes

* Fri Jan 13 2006 Peter Jones <pjones@redhat.com> - 0.97-2
- add dmraid support

* Wed Dec 14 2005 Peter Jones <pjones@redhat.com> - 0.97-1
- update to grub 0.97

* Mon Dec  5 2005 Peter Jones <pjones@redhat.com> - 0.95-17
- fix configure conftest.c bugs
- add -Wno-unused to defeat gcc41 "unused" checking when there are aliases.

* Mon Aug  1 2005 Peter Jones <pjones@redhat.com> - 0.95-16
- minor fix to the --recheck fix.

* Mon Jul 25 2005 Peter Jones <pjones@redhat.com> 0.95-15
- Make "grub-install --recheck" warn the user about how bad it is,
  and keep a backup file, which it reverts to upon detecting some errors.

* Wed Jul  6 2005 Peter Jones <pjones@redhat.com> 0.95-14
- Fix changelog to be UTF-8

* Thu May 19 2005 Peter Jones <pjones@redhat.com> 0.95-13
- Make the spec work with gcc3 and gcc4, so people can test on existing
  installations.
- don't treat i2o like a cciss device, since its partition names aren't done
  that way. (#158158)

* Wed Mar 16 2005 Peter Jones <pjones@redhat.com> 0.95-12
- Make installing on a partition work again when not using raid

* Thu Mar  3 2005 Peter Jones <pjones@redhat.com> 0.95-11
- Make it build with gcc4

* Sun Feb 20 2005 Peter Jones <pjones@redhat.com> 0.95-10
- Always install in MBR for raid1 /boot/

* Sun Feb 20 2005 Peter Jones <pjones@redhat.com> 0.95-9
- Always use full path for mdadm in grub-install

* Tue Feb  8 2005 Peter Jones <pjones@redhat.com> 0.95-8
- Mark the simulation stack executable
- Eliminate the use of inline functions in stage2/builtins.c

* Wed Jan 11 2005 Peter Jones <pjones@redhat.com> 0.95-7
- Make grub ignore everything before the XPM header in the splash image,
  fixing #143879
- If the boot splash image is missing, use console mode instead 
  of graphics mode.
- Don't print out errors using the graphics terminal code if we're not
  actually in graphics mode.

* Mon Jan  3 2005 Peter Jones <pjones@redhat.com> 0.95-6
- reworked much of how the RAID1 support in grub-install works.  This version
  does not require all the devices in the raid to be listed in device.map,
  as long as you specify a physical device or partition rather than an md
  device.  It should also work with a windows dual-boot on the first partition.

* Fri Dec 17 2004 Peter Jones <pjones@redhat.com> 0.95-5
- added support for RAID1 devices to grub-install, partly based on a
  patch from David Knierim. (#114690)

* Tue Nov 30 2004 Jeremy Katz <katzj@redhat.com> 0.95-4
- add patch from upstream CVS to handle sparse files on ext[23]
- make geometry detection a little bit more robust/correct
- use O_DIRECT when reading/writing from devices.  use aligned buffers as 
  needed for read/write (#125808)
- actually apply the i2o patch
- detect cciss/cpqarray devices better (#123249)

* Thu Sep 30 2004 Jeremy Katz <katzj@redhat.com> - 0.95-3
- don't act on the keypress for the menu (#134029)

* Mon Jun 28 2004 Jeremy Katz <katzj@redhat.com> - 0.95-2
- add patch from Nicholas Miell to make hiddenmenu work more 
  nicely with splashimage mode (#126764)

* Fri Jun 18 2004 Jeremy Katz <katzj@redhat.com> - 0.95-1
- update to 0.95
- drop emd patch, E-MD isn't making forward progress upstream
- fix static build for x86_64 (#121095)

* Tue Jun 15 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Wed Jun  9 2004 Jeremy Katz <katzj@redhat.com>
- require system-logos (#120837)

* Fri Jun  4 2004 Jeremy Katz <katzj@redhat.com>
- buildrequire automake (#125326)

* Thu May 06 2004 Warren Togami <wtogami@redhat.com> - 0.94-5
- i2o patch from Markus Lidel

* Wed Apr 14 2004 Jeremy Katz <katzj@redhat.com> - 0.94-4
- read geometry off of the disk since HDIO_GETGEO doesn't actually 
  return correct data with a 2.6 kernel

* Fri Mar 12 2004 Jeremy Katz <katzj@redhat.com>
- add texinfo buildrequires (#118146)

* Wed Feb 25 2004 Jeremy Katz <katzj@redhat.com> 0.94-3
- don't use initrd_max_address

* Fri Feb 13 2004 Elliot Lee <sopwith@redhat.com> 0.94-2
- rebuilt

* Thu Feb 12 2004 Jeremy Katz <katzj@redhat.com> 0.94-1
- update to 0.94, patch merging and updating as necessary

* Sat Jan  3 2004 Jeremy Katz <katzj@redhat.com> 0.93-8
- new bootonce patch from Padraig Brady so that you don't lose 
  the old default (#112775)

* Mon Nov 24 2003 Jeremy Katz <katzj@redhat.com>
- add ncurses-devel as a buildrequires (#110732)

* Tue Oct 14 2003 Jeremy Katz <katzj@redhat.com> 0.93-7
- rebuild

* Wed Jul  2 2003 Jeremy Katz <katzj@redhat.com> 
- Requires: /usr/bin/cmp (#98325)

* Thu May 22 2003 Jeremy Katz <katzj@redhat.com> 0.93-6
- add patch from upstream to fix build with gcc 3.3

* Wed Apr  2 2003 Jeremy Katz <katzj@redhat.com> 0.93-5
- add patch to fix support for serial terminfo (#85595)

* Wed Jan 22 2003 Tim Powers <timp@redhat.com>
- rebuilt

* Fri Jan 17 2003 Jeremy Katz <katzj@redhat.com> 0.93-3
- add patch from HJ Lu to support large disks (#80980, #63848)
- add patch to make message when ending edit clearer (#53846)

* Sun Dec 29 2002 Jeremy Katz <katzj@redhat.com> 0.93-2
- add a patch to reset the terminal type to console before doing 'boot' from
  the command line (#61069)

* Sat Dec 28 2002 Jeremy Katz <katzj@redhat.com> 0.93-1
- update to 0.93
- update configfile patch
- graphics patch rework to fit in as a terminal type as present in 0.93
- use CFLAGS="-Os -g"
- patch configure.in to allow building if host_cpu=x86_64, include -m32 in
  CFLAGS if building on x86_64
- link glibc static on x86_64 to not require glibc32
- include multiboot info pages
- drop obsolete patches, reorder remaining patches into some semblance of order

* Thu Sep  5 2002 Jeremy Katz <katzj@redhat.com> 0.92-7
- splashscreen is in redhat-logos now

* Tue Sep  3 2002 Jeremy Katz <katzj@redhat.com> 0.92-6
- update splashscreen again

* Mon Sep  2 2002 Jeremy Katz <katzj@redhat.com> 0.92-5
- update splashscreen

* Fri Jun 21 2002 Tim Powers <timp@redhat.com> 0.92-4
- automated rebuild

* Thu May 23 2002 Tim Powers <timp@redhat.com> 0.92-3
- automated rebuild

* Fri May  3 2002 Jeremy Katz <katzj@redhat.com> 0.92-2
- add patch from Grant Edwards to make vga16 + serial happier (#63491)

* Wed May  1 2002 Jeremy Katz <katzj@redhat.com> 0.92-1
- update to 0.92
- back to autoreconf
- make it work with automake 1.6/autoconf 2.53
- use "-falign-jumps=1 -falign-loops=1 -falign-functions=1" instead of
  "-malign-jumps=1 -malign-loops=1 -malign-functions=1"	to not use 
  deprecated gcc options

* Tue Apr  9 2002 Jeremy Katz <katzj@redhat.com> 0.91-4
- new splash screen

* Fri Mar  8 2002 Jeremy Katz <katzj@redhat.com> 0.91-3
- include patch from Denis Kitzmen to fix typo causing several options to 
  never be defined (in upstream CVS)
- include patch from upstream CVS to make displaymem always use hex for 
  consistency
- add patch from GRUB mailing list from Keir Fraser to add a --once flag to
  savedefault function so that you can have the equivalent of lilo -R 
  functionality (use 'savedefault --default=N --once' from the grub shell)
- back to autoconf

* Sun Jan 27 2002 Jeremy Katz <katzj@redhat.com> 
- change to use $grubdir instead of /boot/grub in the symlink patch (#58771)

* Fri Jan 25 2002 Jeremy Katz <katzj@redhat.com> 0.91-2
- don't ifdef out the auto memory passing, use the configure flag instead
- add a patch so that grub respects mem= from the kernel command line when 
  deciding where to place the initrd (#52558)

* Mon Jan 21 2002 Jeremy Katz <katzj@redhat.com> 0.91-1
- update to 0.91 final
- add documentation on splashimage param (#51609)

* Wed Jan  2 2002 Jeremy Katz <katzj@redhat.com> 0.91-0.20020102cvs
- update to current CVS snapshot to fix some of the hangs on boot related
  to LBA probing (#57503, #55868, and others)

* Fri Dec 21 2001 Erik Troan <ewt@redhat.com> 0.90-14
- fixed append patch to not require arguments to begin with
- changed to autoreconf from autoconf

* Wed Oct 31 2001 Jeremy Katz <katzj@redhat.com> 0.90-13
- include additional patch from Erich to add sync calls in grub-install to 
  work around updated images not being synced to disk
- fix segfault in grub shell if 'password --md5' is used without specifying
  a password (#55008)

* Fri Oct 26 2001 Jeremy Katz <katzj@redhat.com> 0.90-12
- Include Erich Boleyn <erich@uruk.org>'s patch to disconnect from the 
  BIOS after APM operations.  Should fix #54375

* Wed Sep 12 2001 Erik Troan <ewt@redhat.com>
- added patch for 'a' option in grub boot menu

* Wed Sep  5 2001 Jeremy Katz <katzj@redhat.com> 0.90-11
- grub-install: if /boot/grub/grub.conf doesn't exist but /boot/grub/menu.lst 
  does, create a symlink

* Fri Aug 24 2001 Jeremy Katz <katzj@redhat.com>
- pull in patch from upstream CVS to fix md5crypt in grub shell (#52220)
- use mktemp in grub-install to avoid tmp races

* Fri Aug  3 2001 Jeremy Katz <katzj@redhat.com>
- link curses statically (#49519)

* Thu Aug  2 2001 Jeremy Katz <katzj@redhat.com>
- fix segfault with using the serial device before initialization (#50219)

* Thu Jul 19 2001 Jeremy Katz <katzj@redhat.com>
- add --copy-only flag to grub-install

* Thu Jul 19 2001 Jeremy Katz <katzj@redhat.com>
- copy files in grub-install prior to device probe

* Thu Jul 19 2001 Jeremy Katz <katzj@redhat.com>
- original images don't go in /boot and then grub-install does the right
  thing

* Thu Jul 19 2001 Jeremy Katz <katzj@redhat.com>
- fix the previous patch
- put the password prompt in the proper location

* Thu Jul 19 2001 Jeremy Katz <katzj@redhat.com>
- reset the screen when the countdown is cancelled so text will disappear 
  in vga16 mode

* Mon Jul 16 2001 Jeremy Katz <katzj@redhat.com>
- change configfile defaults to grub.conf

* Sun Jul 15 2001 Jeremy Katz <katzj@redhat.com>
- updated to grub 0.90 final

* Fri Jul  6 2001 Matt Wilson <msw@redhat.com>
- modifed splash screen to a nice shade of blue

* Tue Jul  3 2001 Matt Wilson <msw@redhat.com>
- added a first cut at a splash screen

* Sun Jul  1 2001 Nalin Dahyabhai <nalin@redhat.com>
- fix datadir mismatch between build and install phases

* Mon Jun 25 2001 Jeremy Katz <katzj@redhat.com>
- update to current CVS 
- forward port VGA16 patch from Paulo César Pereira de 
  Andrade <pcpa@conectiva.com.br>
- add patch for cciss, ida, and rd raid controllers
- don't pass mem= to the kernel

* Wed May 23 2001 Erik Troan <ewt@redhat.com>
- initial build for Red Hat
