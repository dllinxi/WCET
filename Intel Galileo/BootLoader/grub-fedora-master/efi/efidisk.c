/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2006  Free Software Foundation, Inc.
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

struct grub_efidisk_data
{
  grub_efi_handle_t handle;
  grub_efi_device_path_t *device_path;
  grub_efi_device_path_t *last_device_path;
  grub_efi_block_io_t *block_io;
  grub_efi_disk_io_t *disk_io;
  struct grub_efidisk_data *next;
};

/* GUIDs.  */
static grub_efi_guid_t disk_io_guid = GRUB_EFI_DISK_IO_GUID;
static grub_efi_guid_t block_io_guid = GRUB_EFI_BLOCK_IO_GUID;
static grub_efi_guid_t device_path_from_text_guid = GRUB_EFI_DEVICE_PATH_FROM_TEXT_GUID;

static struct grub_efidisk_data *fd_devices;
static struct grub_efidisk_data *hd_devices;
static struct grub_efidisk_data *cd_devices;

static int get_device_sector_bits(struct grub_efidisk_data *device);
static int get_device_sector_size(struct grub_efidisk_data *device);
static struct grub_efidisk_data *get_device_from_drive (int drive);

static struct grub_efidisk_data *
make_devices (void)
{
  grub_efi_uintn_t num_handles;
  grub_efi_handle_t *handles;
  grub_efi_handle_t *handle;
  struct grub_efidisk_data *devices = 0;

  /* Find handles which support the disk io interface.  */
  handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL, &disk_io_guid,
				    0, &num_handles);
  if (! handles)
    return 0;

  /* Make a linked list of devices.  */
  for (handle = handles; num_handles--; handle++)
    {
      grub_efi_device_path_t *dp;
      grub_efi_device_path_t *ldp;
      struct grub_efidisk_data *d;
      grub_efi_block_io_t *bio;
      grub_efi_disk_io_t *dio;

      dp = grub_efi_get_device_path (*handle);
      if (! dp)
	continue;

      ldp = find_last_device_path (dp);
      if (! ldp)
	/* This is empty. Why?  */
	continue;

      bio = grub_efi_open_protocol (*handle, &block_io_guid,
				    GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
      dio = grub_efi_open_protocol (*handle, &disk_io_guid,
				    GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);
      if (! bio || ! dio)
	/* This should not happen... Why?  */
	continue;

      d = grub_malloc (sizeof (*d));
      if (! d)
	{
	  /* Uggh.  */
	  grub_free (handles);
	  return 0;
	}

      d->handle = *handle;
      d->device_path = dp;
      d->last_device_path = ldp;
      d->block_io = bio;
      d->disk_io = dio;
      d->next = devices;
      devices = d;
    }

  grub_free (handles);

  return devices;
}

static int
iterate_child_devices (struct grub_efidisk_data *devices,
		       struct grub_efidisk_data *d,
		       int (*hook) (struct grub_efidisk_data *child))
{
  struct grub_efidisk_data *p;

  for (p = devices; p; p = p->next)
    {
      grub_efi_device_path_t *dp, *ldp;

      dp = duplicate_device_path (p->device_path);
      if (! dp)
	return 0;

      ldp = find_last_device_path (dp);
      ldp->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
      ldp->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;
      ldp->length[0] = sizeof (*ldp);
      ldp->length[1] = 0;

      if (compare_device_paths (dp, d->device_path) == 0)
	if (hook (p))
	  {
	    grub_free (dp);
	    return 1;
	  }

      grub_free (dp);
    }

  return 0;
}

/* Add a device into a list of devices in an ascending order.  */
static void
add_device (struct grub_efidisk_data **devices, struct grub_efidisk_data *d)
{
  struct grub_efidisk_data **p;
  struct grub_efidisk_data *n;

  for (p = devices; *p; p = &((*p)->next))
    {
      int ret;

      ret = compare_device_paths (find_last_device_path ((*p)->device_path),
				  find_last_device_path (d->device_path));
      if (ret == 0)
	ret = compare_device_paths ((*p)->device_path,
				    d->device_path);
      if (ret == 0)
	return;
      else if (ret > 0)
	break;
    }

  n = grub_malloc (sizeof (*n));
  if (! n)
    return;

  grub_memcpy (n, d, sizeof (*n));
  n->next = (*p);
  (*p) = n;
}

/* Name the devices.  */
static void
name_devices (struct grub_efidisk_data *devices)
{
  struct grub_efidisk_data *d;

  /* Let's see what can be added more.  */
  for (d = devices; d; d = d->next)
    {
      grub_efi_device_path_t *dp;
      grub_efi_block_io_media_t *m;

      dp = d->last_device_path;
      if (! dp)
	continue;

      m = d->block_io->media;
      if (GRUB_EFI_DEVICE_PATH_TYPE(dp) == GRUB_EFI_MESSAGING_DEVICE_PATH_TYPE)
	{
	  /* XXX FIXME this won't work if we see write-protected disks with
	   * 4k sectors */
	  if (m->read_only && m->block_size > 0x200)
	    {
	      add_device (&cd_devices, d);
	    } else
	    {
	      add_device (&hd_devices, d);
	    }
	}
      if (GRUB_EFI_DEVICE_PATH_TYPE(dp) == GRUB_EFI_ACPI_DEVICE_PATH_TYPE)
	{
	  add_device (&fd_devices, d);
	}
    }
}

static void
free_devices (struct grub_efidisk_data *devices)
{
  struct grub_efidisk_data *p, *q;

  for (p = devices; p; p = q)
    {
      q = p->next;
      grub_free (p);
    }
}

/* Enumerate all disks to name devices.  */
static void
enumerate_disks (void)
{
  struct grub_efidisk_data *devices;

  devices = make_devices ();
  if (! devices)
    return;

  name_devices (devices);
  free_devices (devices);
}

static struct grub_efidisk_data *
get_device (struct grub_efidisk_data *devices, int num)
{
  struct grub_efidisk_data *d;

  for (d = devices; d && num; d = d->next, num--)
    ;

  if (num == 0)
    return d;

  return 0;
}

static int
grub_efidisk_read (struct grub_efidisk_data *d, grub_disk_addr_t sector,
		   grub_size_t size, char *buf)
{
  /* For now, use the disk io interface rather than the block io's.  */
  grub_efi_disk_io_t *dio;
  grub_efi_block_io_t *bio;
  grub_efi_status_t status;
  grub_efi_uint64_t sector_size = get_device_sector_size(d);

  dio = d->disk_io;
  bio = d->block_io;

  status = Call_Service_5 (dio->read,
			   dio, bio->media->media_id,
			   sector * sector_size,
			   size * sector_size,
			   buf);
  if (status != GRUB_EFI_SUCCESS)
    return -1;

  return 0;
}

static int
grub_efidisk_write (struct grub_efidisk_data *d, grub_disk_addr_t sector,
		    grub_size_t size, const char *buf)
{
  /* For now, use the disk io interface rather than the block io's.  */
  grub_efi_disk_io_t *dio;
  grub_efi_block_io_t *bio;
  grub_efi_status_t status;
  grub_efi_uint64_t sector_size = get_device_sector_size(d);

  dio = d->disk_io;
  bio = d->block_io;

  grub_dprintf ("efidisk",
		"writing 0x%x sectors at the sector 0x%x to ??\n",
		(unsigned) size, (unsigned int) sector);

  status = Call_Service_5 (dio->write ,
			   dio, bio->media->media_id,
			   sector * sector_size,
			   size * sector_size,
			   (void *) buf);
  if (status != GRUB_EFI_SUCCESS)
    return -1;

  return 0;
}

void
grub_efidisk_init (void)
{
  enumerate_disks ();
}

void
grub_efidisk_fini (void)
{
  free_devices (fd_devices);
  free_devices (hd_devices);
  free_devices (cd_devices);
}

static int
get_device_sector_size(struct grub_efidisk_data *device)
{
	return device->block_io->media->block_size;
}

int
get_sector_size(int drive)
{
	struct grub_efidisk_data *device = get_device_from_drive(drive);
	return get_device_sector_size(device);
}

/*
 * ffz = Find First Zero in word. Undefined if no zero exists,
 * so code should check against ~0UL first..
 */
static __inline__ unsigned int
ffz (unsigned int word)
{
  __asm__ ("bsfl %1,%0"
:	   "=r" (word)
:	   "r" (~word));
  return word;
}
#define log2(n) ffz(~(n))

static int
get_device_sector_bits(struct grub_efidisk_data *device)
{
	int sector_size = get_device_sector_size(device);
	return log2(sector_size);
}

int
get_sector_bits(int drive)
{
	int sector_size = get_sector_size(drive);
	return log2(sector_size);
}

static struct grub_efidisk_data *
get_device_from_drive (int drive)
{
#ifdef SUPPORT_NETBOOT
  /* Not supported */
  if (drive == NETWORK_DRIVE)
    return NULL;
#endif
  if (drive == GRUB_INVALID_DRIVE)
    return NULL;
  if (drive == cdrom_drive)
    return get_device (cd_devices, 0);
  /* Hard disk */
  if (drive & 0x80)
    return get_device (hd_devices, drive - 0x80);
  /* Floppy disk */
  else
    return get_device (fd_devices, drive);
}

/* Low-level disk I/O.  Our stubbed version just returns a file
   descriptor, not the actual geometry. */
int
get_diskinfo (int drive, struct geometry *geometry)
{
  struct grub_efidisk_data *d;

  d = get_device_from_drive (drive);
  if (!d)
    return -1;
  geometry->total_sectors = d->block_io->media->last_block+1;
  geometry->sector_size = d->block_io->media->block_size;
  geometry->flags = BIOSDISK_FLAG_LBA_EXTENSION;
  geometry->sectors = 63;
  if (geometry->total_sectors / 63 < 255)
    geometry->heads = 1;
  else
    geometry->heads = 255;
  geometry->cylinders = geometry->total_sectors / 63 / geometry->heads;
  return 0;
}

int
biosdisk (int subfunc, int drive, struct geometry *geometry,
	  int sector, int nsec, int segment)
{
  char *buf;
  struct grub_efidisk_data *d;
  int ret;

  d = get_device_from_drive (drive);
  if (!d)
    return -1;
  buf = (char *) ((unsigned long) segment << 4);
  switch (subfunc)
    {
    case BIOSDISK_READ:
      ret = grub_efidisk_read (d, sector, nsec, buf);
      break;
    case BIOSDISK_WRITE:
      ret = grub_efidisk_write (d, sector, nsec, buf);
      break;
    default:
      return -1;
    }

  return 0;
}

/* Some utility functions to map GRUB devices with EFI devices.  */
grub_efi_handle_t
grub_efidisk_get_current_bdev_handle (void)
{
  struct grub_efidisk_data *d;

  d = get_device_from_drive (current_drive);
  if (d == NULL)
    return NULL;

  if (current_drive == GRUB_INVALID_DRIVE)
    return NULL;

  if (current_drive == cdrom_drive)
    return d->handle;

  if (! (current_drive & 0x80))
    return d->handle;
  /* If this is the whole disk, just return its own data.  */
  else if (current_partition == 0xFFFFFF)
    return d->handle;
  /* Otherwise, we must query the corresponding device to the firmware.  */
  else
    {
      struct grub_efidisk_data *devices;
      grub_efi_handle_t handle = 0;
      auto int find_partition (struct grub_efidisk_data *c);

      int find_partition (struct grub_efidisk_data *c)
	{
	  grub_efi_hard_drive_device_path_t hd;

	  grub_memcpy (&hd, c->last_device_path, sizeof (hd));

	  if ((GRUB_EFI_DEVICE_PATH_TYPE (c->last_device_path)
	       == GRUB_EFI_MEDIA_DEVICE_PATH_TYPE)
	      && (GRUB_EFI_DEVICE_PATH_SUBTYPE (c->last_device_path)
		  == GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE)
	      && (part_start == hd.partition_start))
	    {
	      handle = c->handle;
	      return 1;
	    }

	  return 0;
	}

      devices = make_devices ();
      iterate_child_devices (devices, d, find_partition);
      free_devices (devices);

      if (handle != 0)
	return handle;
    }

  return 0;
}

int
grub_get_drive_partition_from_bdev_handle (grub_efi_handle_t handle,
					   unsigned long *drive,
					   unsigned long *partition)
{
  grub_efi_device_path_t *dp, *dp1;
  struct grub_efidisk_data *d, *devices;
  int drv;
  unsigned long part;
  grub_efi_hard_drive_device_path_t hd;
  int found;
  int part_type, part_entry;
  unsigned long partition_start, partition_len, part_offset, part_extoffset;
  unsigned long gpt_offset;
  int gpt_count, gpt_size;
  auto int find_bdev (struct grub_efidisk_data *c);

  int find_bdev (struct grub_efidisk_data *c)
    {
      if (! compare_device_paths (c->device_path, dp))
	{
	  grub_memcpy (&hd, c->last_device_path, sizeof (hd));
	  found = 1;
	  return 1;
	}
      return 0;
    }

  dp = grub_efi_get_device_path (handle);
  if (! dp)
    return 0;

  dp1 = dp;
  while (1)
    {
      grub_efi_uint8_t type = GRUB_EFI_DEVICE_PATH_TYPE (dp1);
      grub_efi_uint8_t subtype = GRUB_EFI_DEVICE_PATH_SUBTYPE(dp1);
      grub_efi_uint16_t len = GRUB_EFI_DEVICE_PATH_LENGTH (dp);

      if (type == GRUB_EFI_MEDIA_DEVICE_PATH_TYPE &&
	      subtype == GRUB_EFI_CDROM_DEVICE_PATH_SUBTYPE)
	{
	  dp1->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
	  dp1->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;
	  dp1->length[0] = 4;
	  dp1->length[1] = 0;
	}

      if (type == GRUB_EFI_MEDIA_DEVICE_PATH_TYPE &&
	  subtype == GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE)
	{
	  grub_efi_hard_drive_device_path_t temp_hd;
	  grub_memcpy (&temp_hd, dp1, len);
	  if (temp_hd.signature_type == 0 && temp_hd.mbr_type >= 0x10) {
	    /*
	     * Apple Parttion Map CDs appear as hard drives with non-spec
	     * partition type fields. Fix them up.
	     */
	    dp1->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
	    dp1->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;
	    dp1->length[0] = 4;
	    dp1->length[1] = 0;
	  }
	}

      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH (dp1))
	break;

      dp1 = GRUB_EFI_NEXT_DEVICE_PATH(dp1);
    }

  drv = 0;
  for (d = fd_devices; d; d = d->next, drv++)
    {
      if (! compare_device_paths (d->device_path, dp))
	{
	  *partition = 0xFFFFFF;
	  *drive = drv;
	  return 1;
	}
    }

  drv = cdrom_drive;
  if (cd_devices  && ! compare_device_paths (cd_devices->device_path, dp))
    {
      *partition = 0xFFFFFF;
      *drive = drv;
      return 1;
    }

  drv = 0x80;
  for (d = hd_devices; d; d = d->next, drv++)
    {
      if (! compare_device_paths (d->device_path, dp))
	{
	  *partition = 0xFFFFFF;
	  *drive = drv;
	  return 1;
	}
    }

  devices = make_devices ();

  drv = 0x80;
  found = 0;
  for (d = hd_devices; d; d = d->next, drv++)
    {
      iterate_child_devices (devices, d, find_bdev);
      if (found)
	break;
    }

  free_devices (devices);

  if (! found)
    return 0;

  char buf[get_sector_size(drv)];
  part = 0xFFFFFF;
  while (next_partition (drv, 0, &part, &part_type,
			 &partition_start, &partition_len,
			 &part_offset, &part_entry,
			 &part_extoffset, &gpt_offset, &gpt_count,
			 &gpt_size, buf))
    {
      if (part_type
	  && partition_start == hd.partition_start)
	{
	  *drive = drv;
	  *partition = part;
	  return 1;
	}
    }

  return 0;
}

int
check_device (const char *device)
{
  grub_efi_device_path_t *dp;

  dp = device_path_from_utf8(device);
  if (dp == NULL)
    return 0;

  grub_free(dp);
  return 1;
}

static void
swap_devices (struct grub_efidisk_data *d0,
	      struct grub_efidisk_data *d1)
{
  struct grub_efidisk_data tmp;

  if (!d0 || !d1)
    return;

  memcpy(&tmp, d1, sizeof(*d1));

  memcpy(&d0->handle, &d1->handle, sizeof(d1->handle));
  d0->device_path = d1->device_path;
  d0->last_device_path = d1->last_device_path;
  d0->block_io = d1->block_io;
  d0->disk_io = d1->disk_io;

  memcpy(d1->handle, tmp.handle, sizeof(tmp.handle));
  d1->device_path = tmp.device_path;
  d1->last_device_path = tmp.last_device_path;
  d1->block_io = tmp.block_io;
  d1->disk_io = tmp.disk_io;
}

static int
compare_hd_device_paths(grub_efi_hard_drive_device_path_t *hd0,
			grub_efi_hard_drive_device_path_t *hd1)
{
  grub_efi_uint64_t x;
  int sigsize;

  if ((x = hd1->partition_number - hd0->partition_number))
    return x;

  if ((x = hd1->partition_start - hd0->partition_start))
    return x;


  if ((x = hd1->partition_size - hd0->partition_size))
    return x;

  if ((x = hd1->signature_type - hd0->signature_type))
    return x;

  switch (hd0->signature_type)
    {
    case 1:
      sigsize = 4;
      break;
    case 2:
      sigsize = 16;
      break;
    default:
      sigsize = 0;
      break;
    }
  x = grub_memcmp((char *)hd0->partition_signature,
                  (char *)hd1->partition_signature, sigsize);
  return x;
}

static grub_efi_device_path_t *
get_parent_of_disk(grub_efi_device_path_t *hd)
{
  grub_efi_uintn_t num_handles;
  grub_efi_handle_t *handles;
  grub_efi_handle_t *handle;
  grub_efi_device_path_t *ret;

  handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL,
				     &simple_file_system_guid,
                                    0, &num_handles);
  for (handle = handles; num_handles--; handle++)
    {
      grub_efi_device_path_t *fsdp, *hddp;

      fsdp = grub_efi_get_device_path (*handle);
      if (!fsdp)
	continue;
      hddp = find_last_device_path(fsdp);

      if (compare_hd_device_paths((grub_efi_hard_drive_device_path_t *)hddp,
				   (grub_efi_hard_drive_device_path_t *)hd) == 0)
        {
	  grub_efi_device_path_t *p;
	  ret = duplicate_device_path((grub_efi_device_path_t *)fsdp);
	  if (!ret)
	    return NULL;
	  for (p = ret; ; p = GRUB_EFI_NEXT_DEVICE_PATH(p))
	    {
	      if (GRUB_EFI_END_ENTIRE_DEVICE_PATH(p))
		break;
	      if ((GRUB_EFI_DEVICE_PATH_TYPE(p) ==
			GRUB_EFI_MEDIA_DEVICE_PATH_TYPE)
		      && (GRUB_EFI_DEVICE_PATH_SUBTYPE(p) ==
				GRUB_EFI_HARD_DRIVE_DEVICE_PATH_SUBTYPE))
	        {
		  p->type = GRUB_EFI_END_DEVICE_PATH_TYPE;
		  p->subtype = GRUB_EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE;
		  p->length[0] = 4;
		  p->length[1] = 0;
		  break;
		}
	    }
	  return ret;
	}
    }
  return NULL;
}

void
assign_device_name (int drive, const char *device)
{
  grub_efi_device_path_t *dp0, *dp1;
  struct grub_efidisk_data *devices;
  struct grub_efidisk_data *d, *d0 = NULL, *d1 = NULL;
  int n = -1;

  dp0 = device_path_from_utf8(device);
  if (!dp0)
    return;

  dp1 = get_parent_of_disk(dp0);
  grub_free(dp0);
  if (!dp1)
    return;

  if (drive & 0x80)
    {
      drive -= 0x80;
      devices = hd_devices;
    }
  else
    {
      devices = cd_devices;
      drive -= 0x100;
    }

  for (d = devices; d; d = d->next)
    {
      if (!d->device_path)
	continue;

      if (++n == drive)
	d0 = d;

      int x;
      if (!(x = compare_device_paths(dp1, d->device_path)))
	d1 = d;

      if (d0 && d1)
        {
	  /* if they're the same node, that just means it's already at
	   * the right position. */
	  if (d0 != d1)
	    {
	      swap_devices(d0, d1);
	      grub_free(dp1);
	      return;
	    }
	}
    }
  grub_free(dp1);
}
