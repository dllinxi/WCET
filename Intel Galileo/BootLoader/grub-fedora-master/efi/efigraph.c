/* efigraph.c - EFI "graphics output" support for GRUB/EFI */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright 2007 Red Hat, Inc.
 *  Copyright (C) 2007 Intel Corp.
 *  Copyright (C) 2001,2002  Red Hat, Inc.
 *  Portions copyright (C) 2000  Conectiva, Inc.
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

#ifdef SUPPORT_GRAPHICS

#include <grub/misc.h>
#include <grub/types.h>
#include <grub/cpu/linux.h>
#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/efi/misc.h>

#include <term.h>
#include <shared.h>
#include <graphics.h>

#include "graphics.h"
#include "xpm.h"

#define dbgdelay(_f, _l) ({\
	if (debug_graphics) {				\
  		do {					\
			grub_efi_stall(1000);		\
		} while (console_getkey() < 0);		\
	}						\
	})

#define dprintf(format, args...) ({			\
	if (debug_graphics) {				\
		struct term_entry *_tt = current_term;	\
		current_term = term_table;		\
		grub_printf(format, ##args);		\
		current_term = _tt;			\
	}						\
	})

struct grub_pixel_info
{
  char depth_bits;
  char depth_bytes;
  unsigned char red_size;
  unsigned char red_pos;
  unsigned char green_size;
  unsigned char green_pos;
  unsigned char blue_size;
  unsigned char blue_pos;
  unsigned char reserved_size;
  unsigned char reserved_pos;
  int line_length;
};

typedef struct grub_pixel_info grub_pixel_info_t;


static grub_efi_guid_t graphics_output_guid = GRUB_EFI_GRAPHICS_OUTPUT_GUID;
static grub_efi_guid_t pci_io_guid = GRUB_EFI_PCI_IO_GUID;

#ifndef MIN
#define MIN(x,y) ( ((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ( ((x) < (y)) ? (y) : (x))
#endif

#define pixel_equal(x,y) ((x).bgrr.red == (y).bgrr.red && \
                          (x).bgrr.green == (y).bgrr.green && \
                          (x).bgrr.blue == (y).bgrr.blue && \
                          (x).bgrr.reserved == (y).bgrr.reserved)

struct video_mode {
    grub_efi_uint32_t number;
    grub_efi_uintn_t size;
    grub_efi_graphics_output_mode_information_t *info;
};

#define MAX_PALETTE 16
struct eg {
    struct graphics_backend *backend;
    grub_efi_graphics_output_t *output_intf;
    struct video_mode **modes;
    int max_mode;
    grub_efi_uint32_t text_mode;
    grub_efi_uint32_t graphics_mode;
    grub_pixel_info_t pixel_info;
    enum { TEXT, GRAPHICS } current_mode;

    position_t screen_size;
    position_t screen_pos;

    struct bltbuf *background;

    grub_efi_graphics_output_pixel_t palette[MAX_PALETTE + 1];
};

#define RGB(r,g,b) { .bgrr.red = r, .bgrr.green = g, .bgrr.blue = b }

static grub_efi_graphics_output_pixel_t cga_colors[] = {
    RGB(0x00,0x00,0x00), //  0 Black
    RGB(0x7f,0x00,0x00), //  1 Dark Red
    RGB(0x00,0x7f,0x00), //  2 Dark Green
    RGB(0x7f,0x7f,0x00), //  3 Dark Yellow
    RGB(0x00,0x00,0x7f), //  4 Dark Blue
    RGB(0x7f,0x00,0x7f), //  5 Dark Magenta
    RGB(0x00,0x7f,0x7f), //  6 Dark Cyan
    RGB(0xc0,0xc0,0xc0), //  7 Light Grey
    RGB(0x7f,0x7f,0x7f), //  8 Dark Grey
    RGB(0xff,0x00,0x00), //  9 Red
    RGB(0x00,0xff,0x00), // 10 Green
    RGB(0xff,0xff,0x00), // 11 Yellow
    RGB(0x00,0x00,0xff), // 12 Blue
    RGB(0xff,0x00,0xff), // 13 Magenta
    RGB(0x00,0xff,0xff), // 14 Cyan
    RGB(0xff,0xff,0xff), // 15 White
    RGB(0xff,0xff,0xff), // 16 Also white ;)
};

static const int n_cga_colors = sizeof (cga_colors) / sizeof (cga_colors[0]);

static void
find_bits (unsigned long mask, unsigned char *first,
	   unsigned char* len)
{
  unsigned char bit_pos = 0, bit_len = 0;
  *first =0;
  *len = 0;
  if (mask == 0)
    return;
  while (! (mask & 0x1)) {
    mask = mask >> 1;
    bit_pos++;
  }
  while (mask & 0x1) {
    mask = mask >> 1;
    bit_len++;
  }
  *first = bit_pos;
  *len = bit_len;
}

static grub_efi_graphics_output_mode_information_t *
get_graphics_mode_info_for_mode(struct eg *eg, int mode)
{
	int i;

	for (i = 0; i < eg->max_mode; i++) {
		if (eg->modes[i] == NULL)
			continue;
		if (eg->modes[i]->number == mode)
			return eg->modes[i]->info;
	}
	return NULL;
}

static grub_efi_graphics_output_mode_information_t *
get_graphics_mode_info(struct eg *eg)
{
	return get_graphics_mode_info_for_mode(eg, eg->graphics_mode);
}

static void
print_mode_info(struct video_mode *mode)
{
	grub_efi_graphics_output_mode_information_t *info = mode->info;
	dprintf("mode %d (%dx%d, pitch %d, ",
		mode->number,
		info->horizontal_resolution,
		info->vertical_resolution,
		info->pixels_per_scan_line);
	switch(info->pixel_format) {
		case GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR:
			dprintf("rgbr 8bpc");
			break;
		case GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR:
			dprintf("bgrr 8bpc");
			break;
		case GRUB_EFI_PIXEL_BIT_MASK:
			dprintf("bitmask color");
			break;
		case GRUB_EFI_PIXEL_BLT_ONLY:
			dprintf("blt only");
			break;
	}
	dprintf(")\n");
	if (info->pixel_format == GRUB_EFI_PIXEL_BIT_MASK) {
		dprintf("red: %08x green: %08x blue: %08x res: %08x\n",
		info->pixel_information.red_mask,
		info->pixel_information.green_mask,
		info->pixel_information.blue_mask,
		info->pixel_information.reserved_mask);
	}
}

static void
set_kernel_params(struct graphics_backend *backend,
            struct linux_kernel_params *params)
{
    struct eg *eg;

    if (!backend || !backend->priv)
        return;

    eg = backend->priv;
    grub_efi_graphics_output_t *gop_intf = NULL;
    grub_efi_graphics_output_mode_t *gop_mode = NULL;
    grub_efi_status_t efi_status = GRUB_EFI_SUCCESS;
    grub_efi_graphics_output_mode_information_t *gop_info = NULL;
    grub_efi_uintn_t size;

    gop_intf = grub_efi_locate_protocol (&graphics_output_guid, NULL);
    if (gop_intf == NULL)
        return;

    gop_mode = gop_intf->mode;

    efi_status = Call_Service_4 (gop_intf->query_mode,
			       gop_intf, gop_mode->mode, &size, &gop_info);


    if (efi_status == GRUB_EFI_SUCCESS) {
        /* No VBE on EFI.  */
        params->lfb_width = gop_info->horizontal_resolution;
        params->lfb_height = gop_info->vertical_resolution;
        params->lfb_base = gop_mode->frame_buffer_base;
        params->lfb_size = gop_mode->frame_buffer_size;
        params->lfb_pages = 1;
        params->vesapm_segment = 0;
        params->vesapm_offset = 0;
        params->vesa_attrib = 0;
        if (gop_info->pixel_format == GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR) {
            params->lfb_depth = 32;
            params->red_mask_size = 8;
            params->red_field_pos = 0;
            params->green_mask_size = 8;
            params->green_field_pos = 8;
            params->blue_mask_size = 8;
            params->blue_field_pos = 16;
            params->reserved_mask_size = 8;
            params->reserved_field_pos = 24;
            params->lfb_line_len = gop_info->pixels_per_scan_line * 4;
        } else if (gop_info->pixel_format ==
                GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR) {
            params->lfb_depth = 32;
            params->red_mask_size = 8;
            params->red_field_pos = 16;
            params->green_mask_size = 8;
            params->green_field_pos = 8;
            params->blue_mask_size = 8;
            params->blue_field_pos = 0;
            params->reserved_mask_size = 8;
            params->reserved_field_pos = 24;
	    params->lfb_line_len = gop_info->pixels_per_scan_line * 4;
        } else if (gop_info->pixel_format == GRUB_EFI_PIXEL_BIT_MASK) {
            find_bits (gop_info->pixel_information.red_mask,
      		 &params->red_field_pos, &params->red_mask_size);
            find_bits (gop_info->pixel_information.green_mask,
      		 &params->green_field_pos, &params->green_mask_size);
            find_bits (gop_info->pixel_information.blue_mask,
      		 &params->blue_field_pos, &params->blue_mask_size);
            find_bits (gop_info->pixel_information.reserved_mask,
      		 &params->reserved_field_pos, &params->reserved_mask_size);
            params->lfb_depth = params->red_mask_size
                                + params->green_mask_size
                                + params->blue_mask_size
                                + params->reserved_mask_size;
            params->lfb_line_len =
                (gop_info->pixels_per_scan_line * params->lfb_depth) / 8;
        } else  {
            params->lfb_depth = 4;
            params->red_mask_size = 0;
            params->red_field_pos = 0;
            params->green_mask_size = 0;
            params->green_field_pos = 0;
            params->blue_mask_size = 0;
            params->blue_field_pos = 0;
            params->reserved_mask_size = 0;
            params->reserved_field_pos = 0;
            params->lfb_line_len = params->lfb_width / 2;
        }
#if 0
        params->video_cursor_x = 0;
        params->video_cursor_y = 0;
        params->video_page = 0;
        params->video_mode = 0;
        params->video_width = 0;
        params->video_ega_bx = 0;
        params->video_height = 0;
        params->have_vga = 0x70;
        params->font_size = 0;
#else
        params->video_cursor_x = grub_efi_system_table->con_out->mode->cursor_column;
        params->video_cursor_y = grub_efi_system_table->con_out->mode->cursor_row;
        params->video_page = 0; /* ??? */
        params->video_mode = grub_efi_system_table->con_out->mode->mode;
        params->video_width = (grub_console_getwh () >> 8);
        params->video_ega_bx = 0;
        params->video_height = (grub_console_getwh () & 0xff);
        params->have_vga = VIDEO_TYPE_EFI;
        params->font_size = 16; /* XXX */
#endif
    }
}

static void
pixel_to_rgb(grub_efi_graphics_output_pixel_t *pixel,
             int *red, int *green, int *blue)
{
    *red = pixel->bgrr.red;
    *green = pixel->bgrr.green;
    *blue = pixel->bgrr.blue;
}

static void
rgb_to_pixel(int red, int green, int blue,
             grub_efi_graphics_output_pixel_t *pixel)
{
    pixel->bgrr.red = red;
    pixel->bgrr.green = green;
    pixel->bgrr.blue = blue;
}

static void
position_to_phys(struct eg *eg, position_t *virt, position_t *phys)
{
    phys->x = virt->x + eg->screen_pos.x;
    phys->y = virt->y + eg->screen_pos.y;
}

static int
abs_paddr(struct eg *eg, position_t *virt)
{
    position_t phys;
    position_to_phys(eg, virt, &phys);
    return phys.x + phys.y * eg->screen_size.x;
}

struct bltbuf {
    grub_efi_uintn_t width;
    grub_efi_uintn_t height;
    grub_efi_graphics_output_pixel_t pixbuf[];
};

static struct bltbuf *alloc_bltbuf(grub_efi_uintn_t width,
					   grub_efi_uintn_t height)
{
	struct bltbuf *buf = NULL;
	grub_efi_uintn_t pixbuf_size = width * height *
		sizeof (grub_efi_graphics_output_pixel_t);

	if (!(buf = grub_malloc(sizeof(buf->width) + sizeof(buf->height) +
				pixbuf_size)))
		return NULL;

	buf->width = width;
	buf->height = height;
	grub_memset(buf->pixbuf, '\0', pixbuf_size);
	return buf;
}


static void
hw_blt_pos_to_screen_pos(struct eg *eg, struct bltbuf *bltbuf,
                      position_t *bltpos, position_t *bltsz, position_t *pos)
{
    position_t phys;

    position_to_phys(eg, pos, &phys);

    Call_Service_10(eg->output_intf->blt, eg->output_intf, (void *)bltbuf->pixbuf,
                    GRUB_EFI_BLT_BUFFER_TO_VIDEO,
                    bltpos->x, bltpos->y,
                    phys.x, phys.y,
                    bltsz->x, bltsz->y,
                    0);
}

static void
blt_pos_to_screen_pos(struct eg *eg, struct bltbuf *bltbuf,
        position_t *bltpos, position_t *bltsz, position_t *pos)
{
    grub_efi_graphics_output_mode_information_t *info = get_graphics_mode_info(eg);
    grub_efi_graphics_output_pixel_t *pixel;
    position_t phys;
    const int pxlstride = info->pixels_per_scan_line;
    const int bytestride = pxlstride * sizeof (*pixel);

    position_to_phys(eg, pos, &phys);

    if (info->pixel_format == GRUB_EFI_PIXEL_BLT_ONLY || 1) {
        hw_blt_pos_to_screen_pos(eg, bltbuf, bltpos, bltsz, pos);
#if 0
    } else if (info->pixel_format == GRUB_EFI_PIXEL_BIT_MASK) {
        int y;
        grub_pixel_info_t *pinfo = &eg->pixel_info;
        const int maxpixels =
            MIN(info->horizontal_resolution - pos->x, bltsz->x);

        for (y = bltpos->y; y < bltpos->y + bltsz->y; y++, phys.y++) {
            char raw_pixels[maxpixels * pinfo->depth_bytes];
            char *fb = (char *)(unsigned long)eg->output_intf->mode->frame_buffer_base;
            int x;

            pixel = (void *)&bltbuf->pixbuf[y * bltbuf->width + bltpos->x];
            fb += phys.y * pinfo->line_length + phys.x * pinfo->depth_bytes;

            for (x = 0; x < maxpixels; x++) {
                char depth_bytes = pinfo->depth_bytes;
                char *raw_pixel = raw_pixels + x * depth_bytes;

                int red, green, blue, color;
                char *colorp;

#if 0
                red = pixel[x].bgrr.red & 0x3f;
                green = pixel[x].bgrr.green & 0x3f;
                blue = pixel[x].bgrr.blue & 0x3f;
                red = red * ((1 << pinfo->red_size) - 1) / 0x3f;
                green = green * ((1 << pinfo->green_size) - 1) / 0x3f;
                blue = blue * ((1 << pinfo->blue_size) - 1) / 0x3f;
#else
                red = pixel[x].bgrr.red;
                green = pixel[x].bgrr.green;
                blue = pixel[x].bgrr.blue;

                red >>= 8 - pinfo->red_size;
                green >>= 8 - pinfo->green_size;
                blue >>= 8 - pinfo->blue_size;
#endif

                color = (red << pinfo->red_pos) |
                        (green << pinfo->green_pos) |
                        (blue << pinfo->blue_pos);
                colorp = (void *)&color;
                while (depth_bytes--)
                    *raw_pixel++ = *colorp++;
            }
            memmove(fb, raw_pixels, maxpixels * pinfo->depth_bytes);
        }
#endif
    } else {
        int y;
        grub_pixel_info_t *pinfo = &eg->pixel_info;
        const int maxpixels =
            MIN(info->horizontal_resolution - pos->x, bltsz->x);

	//char *line = &fb[phys.y * bytestride + phys.x * sizeof(*pixel)];
        for (y = bltpos->y; y < bltpos->y + bltsz->y; y++, phys.y++) {
	    char raw_pixels[maxpixels * sizeof(*pixel)];
            char *fb = (char *)(unsigned long)eg->output_intf->mode->frame_buffer_base;
            int x;

            pixel = (void *)&bltbuf->pixbuf[y * bltbuf->width + bltpos->x];
            fb += phys.y * pinfo->line_length + phys.x * pinfo->depth_bytes;

            if (info->pixel_format == GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR) {
                memmove(raw_pixels, pixel, maxpixels * sizeof (*pixel));
                continue;
            } else if (info->pixel_format==GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR) {
                grub_efi_graphics_output_pixel_t shadow[maxpixels];
                for (x = 0; x < maxpixels; x++) {
                    shadow[x].rgbr.red = pixel[x].bgrr.red;
                    shadow[x].rgbr.green = pixel[x].bgrr.green;
                    shadow[x].rgbr.blue = pixel[x].bgrr.blue;
                }

                memmove(raw_pixels, shadow, maxpixels * sizeof (*pixel));
            } else if (info->pixel_format == GRUB_EFI_PIXEL_BIT_MASK) {
                for (x = 0; x < maxpixels; x++) {
                    char depth_bytes = pinfo->depth_bytes;
                    char *raw_pixel = raw_pixels + x * depth_bytes;

                    int red, green, blue, color;
                    char *colorp;

#if 0
                    red = pixel[x].bgrr.red & 0x3f;
                    green = pixel[x].bgrr.green & 0x3f;
                    blue = pixel[x].bgrr.blue & 0x3f;
                    red = red * ((1 << pinfo->red_size) - 1) / 0x3f;
                    green = green * ((1 << pinfo->green_size) - 1) / 0x3f;
                    blue = blue * ((1 << pinfo->blue_size) - 1) / 0x3f;
#else
                    red = pixel[x].bgrr.red;
                    green = pixel[x].bgrr.green;
                    blue = pixel[x].bgrr.blue;

                    red >>= 8 - pinfo->red_size;
                    green >>= 8 - pinfo->green_size;
                    blue >>= 8 - pinfo->blue_size;
#endif

                    color = (red << pinfo->red_pos) |
                            (green << pinfo->green_pos) |
                            (blue << pinfo->blue_pos);
                    colorp = (void *)&color;
                    while (depth_bytes--)
                        *raw_pixel++ = *colorp++;
                }
	    }
	    memmove(fb, raw_pixels, maxpixels * pinfo->depth_bytes);
        }
    }
}

static void
blt_to_screen(struct eg *eg, struct bltbuf *bltbuf)
{
#if 0
    position_t addr = {0, 0};

    position_to_phys(eg, &addr, &addr);

    Call_Service_10(eg->output_intf->blt, eg->output_intf, bltbuf->pixbuf,
                    GRUB_EFI_BLT_BUFFER_TO_VIDEO,
                    0, 0,
                    addr.x, addr.y,
                    bltbuf->width, bltbuf->height,
                    0);
#else
    const int pxlstride = eg->output_intf->mode->info->pixels_per_scan_line;
    position_t bltsz, bltpos = { 0, 0 }, pos = { 0, 0 };
    bltsz.x = MIN(bltbuf->width, pxlstride);
    bltsz.y = MIN(bltbuf->height, eg->output_intf->mode->info->vertical_resolution);

    blt_pos_to_screen_pos(eg, bltbuf, &bltpos, &bltsz, &pos);
#endif
}

static void
blt_to_screen_pos(struct eg *eg, struct bltbuf *bltbuf, position_t *pos)
{
    position_t bltpos = {0, 0};
    position_t bltsz = { bltbuf->width, bltbuf->height };
    blt_pos_to_screen_pos(eg, bltbuf, &bltpos, &bltsz, pos);
}

static int
save_video_mode(struct eg *eg, struct video_mode *mode)
{
	grub_efi_status_t status;



	status = Call_Service_4(eg->output_intf->query_mode, eg->output_intf,
                                mode->number, &mode->size, &mode->info);
	return status == GRUB_EFI_SUCCESS;
}

static void
get_screen_size(struct graphics_backend *backend, position_t *size)
{
    struct eg *eg = backend->priv;
    grub_efi_graphics_output_mode_information_t *info;

    info = get_graphics_mode_info(eg);

    size->x = info->horizontal_resolution;
    size->y = info->vertical_resolution;
}

static void
bltbuf_set_pixel(struct bltbuf *bltbuf, position_t *pos,
                             grub_efi_graphics_output_pixel_t *pixel)
{
    if (pos->x < 0 || pos->x >= bltbuf->width)
        return;
    if (pos->x < 0 || pos->y >= bltbuf->height)
        return;
    grub_memmove(&bltbuf->pixbuf[pos->x + pos->y * bltbuf->width], pixel,
            sizeof *pixel);
}

static void
bltbuf_get_pixel(struct bltbuf *bltbuf, position_t *pos,
                 grub_efi_graphics_output_pixel_t *pixel)
{
    if (bltbuf && pos->x < bltbuf->width && pos->y < bltbuf->height) {
    	grub_memmove(pixel, &bltbuf->pixbuf[pos->x + pos->y * bltbuf->width],
            sizeof *pixel);
    } else {
	pixel->bgrr.red = 0x00;
	pixel->bgrr.green = 0x00;
	pixel->bgrr.blue = 0x00;
    }
}

static void
bltbuf_set_pixel_rgb(struct bltbuf *bltbuf, position_t *pos,
                     int red, int green, int blue)
{
    grub_efi_graphics_output_pixel_t pixel;
    rgb_to_pixel(red, green, blue, &pixel);
    bltbuf_set_pixel(bltbuf, pos, &pixel);
}

static void
bltbuf_set_pixel_idx(struct eg *eg, struct bltbuf *bltbuf,
                     position_t *pos, int idx)
{
    bltbuf_set_pixel(bltbuf, pos, &eg->palette[idx]);
}

static void
bltbuf_get_pixel_idx(struct bltbuf *bltbuf, position_t *pos, int *idx)
{
    grub_efi_graphics_output_pixel_t pixel;

    rgb_to_pixel(0, 0, 0, &pixel);
    bltbuf_get_pixel(bltbuf, pos, &pixel);
    for (*idx = 0; *idx < 16; (*idx)++) {
        if (pixel_equal(cga_colors[*idx], pixel))
            break;
    }
}

static struct bltbuf *
xpm_to_bltbuf(struct xpm *xpm)
{
    struct bltbuf *bltbuf = NULL;
    position_t pos;

    if (!(bltbuf = alloc_bltbuf(xpm->width, xpm->height)))
        return NULL;

    for (pos.y = 0; pos.y < xpm->height; pos.y++) {
        for (pos.x = 0; pos.x < xpm->width; pos.x++) {
            xpm_pixel_t xpl;
            unsigned char idx;

            idx = xpm_get_pixel_idx(xpm, pos.x, pos.y);
            xpm_get_idx(xpm, idx, &xpl);

            bltbuf_set_pixel_rgb(bltbuf, &pos, xpl.red, xpl.green, xpl.blue);
        }
    }

    return bltbuf;
}

static void
cursor(struct graphics_backend *backend, int set)
{
    struct eg *eg;
    int ch, invert;
    unsigned short *text;
    position_t fpos, screensz;
    int offset;

    eg = backend->priv;

    if (set && !graphics_get_scroll())
        return;

    text = graphics_get_text_buf();
    graphics_get_font_position(&fpos);
    graphics_get_screen_rowscols(&screensz);

    offset = fpos.y * screensz.x + fpos.x;

    if (set)
        text[offset] |= 0x0200;

    graphics_clbl(fpos.x, fpos.y, 1, 1, 1);

    if (set)
        text[offset] &= 0xfdff;
}

static void blank(struct graphics_backend *backend);

static void
reset_screen_geometry(struct graphics_backend *backend)
{
    struct eg *eg = backend->priv;
    struct xpm *xpm = graphics_get_splash_xpm();
    grub_efi_graphics_output_mode_information_t *info;
    position_t screensz;

    info = get_graphics_mode_info(eg);

    if (xpm) {
        eg->screen_pos.x =
            (info->horizontal_resolution - xpm->width) / 2;
        eg->screen_pos.y =
            (info->vertical_resolution - xpm->height) / 2;
    } else {
        eg->screen_pos.x = 0;
        eg->screen_pos.y = 0;
    }

    blank(backend);
    graphics_get_screen_rowscols(&screensz);
    graphics_clbl(0, 0, screensz.x, screensz.y, 0);
    graphics_clbl(0, 0, screensz.x, screensz.y, 1);
}

static void
setxy(struct graphics_backend *backend, position_t *pos)
{
    position_t fpos;

    fpos.x = pos->x;
    fpos.y = pos->y;
    graphics_set_font_position(&fpos);
}

static void
eg_getxy(struct graphics_backend *backend, position_t *pos)
{
    graphics_get_font_position(pos);
}

static void
draw_pixel(struct graphics_backend *backend, position_t *pos, pixel_t *pixel)
{
    struct bltbuf *bltbuf = NULL;
    grub_efi_graphics_output_pixel_t *eup = (grub_efi_graphics_output_pixel_t *)pixel;

    bltbuf = alloc_bltbuf(1,1);
    if (!bltbuf)
        return;

    grub_memmove(&bltbuf->pixbuf[0], eup, sizeof (*eup));

    blt_to_screen_pos(backend->priv, bltbuf, pos);

    grub_free(bltbuf);
}

static pixel_t *
get_pixel_idx(struct graphics_backend *backend, int idx)
{
    static grub_efi_graphics_output_pixel_t pixel;
    struct eg *eg = backend->priv;
    if (idx < 0 || idx > MAX_PALETTE)
        return NULL;
    pixel.bgrr.red = eg->palette[idx].bgrr.red;
    pixel.bgrr.green = eg->palette[idx].bgrr.green;
    pixel.bgrr.blue = eg->palette[idx].bgrr.blue;
    return (pixel_t *)&pixel;
}

static pixel_t *
get_pixel_rgb(struct graphics_backend *backend, int red, int green, int blue)
{
    static grub_efi_graphics_output_pixel_t pixel;
    rgb_to_pixel(red, green, blue, &pixel);
    return &pixel;
}

static void
set_palette(struct graphics_backend *backend, int idx,
            int red, int green, int blue)
{
    grub_efi_graphics_output_pixel_t pixel;
    struct eg *eg = backend->priv;

    if (idx > MAX_PALETTE)
        return;
    rgb_to_pixel(red, green, blue, &pixel);
    grub_memmove(&eg->palette[idx], &pixel, sizeof pixel);
}

static void
blank(struct graphics_backend *backend)
{
    struct eg *eg = backend->priv;
    struct bltbuf *bltbuf;
    position_t pos = {0, 0};
    grub_efi_graphics_output_mode_information_t *info;
    grub_efi_uintn_t x, y, i, j;
    unsigned char r = 0 ,g = 0;

    info = get_graphics_mode_info(eg);
    x = info->horizontal_resolution;
    y = info->vertical_resolution;

    if (x == 0 || y == 0)
        return;

    bltbuf = alloc_bltbuf(x, y);
    for (j = 0; j < y; j++) {
        if (debug_graphics && j % 16 == 0) {
            g = g == 0 ? 7 : 0;
            r = g == 0 ? 7 : 0;
        }
        for (i = 0; i < x; i++) {
            if (debug_graphics && i % 16 == 0) {
                g = g == 0 ? 7 : 0;
                r = g == 0 ? 7 : 0;
            }
            pos.x = i;
            pos.y = j;
            bltbuf_set_pixel_rgb(bltbuf, &pos, r * 16, g * 16, 0x0);
        }
    }

    blt_to_screen(eg, bltbuf);

    grub_free(bltbuf);
}


static void
draw_white_box(struct graphics_backend *backend)
{
    struct eg *eg = backend->priv;
    struct bltbuf *bltbuf;
    position_t pos = {0, 0}, bltpos = {0, 0}, bltsz = {100,100};

    bltbuf = alloc_bltbuf(bltsz.x, bltsz.y);
    for (pos.y = 0; pos.y < bltsz.y; pos.y++) {
        for (pos.x = 0; pos.x < bltsz.x; pos.x++) {
            bltbuf_set_pixel_rgb(bltbuf, &pos, 0xff, 0xff, 0xff);
        }
    }

    blt_pos_to_screen_pos(eg, bltbuf, &bltpos, &bltsz, &pos);

#if 0
    Call_Service_10(eg->output_intf->blt, eg->output_intf, bltbuf->pixbuf,
        GRUB_EFI_BLT_BUFFER_TO_VIDEO, 0, 0, 100, 100, x, y, 0);
#endif

    grub_free(bltbuf);
}

static void
bltbuf_cp_bl(struct bltbuf *d, position_t dpos,
             struct bltbuf *s, position_t spos)
{
    grub_efi_graphics_output_pixel_t *dp, *sp;

    const int xavail = MAX(0, s ? s->width - spos.x : 0);
    const int xtotal = MAX(0, d->width - dpos.x);
    const int xcp = MAX(0, MIN(xtotal, xavail));
    const int xcl = MAX(0, xtotal - xcp);

    const int yavail = MAX(0, s ? s->height - spos.y : 0);
    const int ytotal = MAX(0, d->height - dpos.y);
    const int ycp = MAX(0, MIN(ytotal, yavail));
    const int ycl = MAX(0, ytotal - ycp);

    int y, x;

    for (y = 0; y < ytotal; y++) {
        dp = &d->pixbuf[(dpos.y + y) * d->width + dpos.x];

        if (y < yavail) {
            sp = &s->pixbuf[(spos.y + y) * s->width + spos.x];
            memmove(dp, sp, xcp * sizeof (*dp));
            dp = &d->pixbuf[(dpos.y + y) * d->width + dpos.x + xcp];
            memset(dp, '\0', xcl * sizeof (*dp));
        } else {
            memset(dp, '\0', xtotal * sizeof (*dp));
        }
    }
}

/* copy a region the size of bltbuf from the background into bltbuf,
 * starting at offset bgpos
 */
static void
bltbuf_draw_bg(struct graphics_backend *backend, struct bltbuf *bltbuf,
        position_t bgpos)
{
    struct eg *eg = backend->priv;
    position_t blpos = { 0, 0 };

    bltbuf_cp_bl(bltbuf, blpos, eg->background, bgpos);
}

static void
dbg_dump_palette(struct graphics_backend *backend)
{
    struct eg *eg;
    int i;
    if (!backend || !backend->priv)
        return;
    eg = backend->priv;
    if (!eg->palette)
        return;
}

static int
is_shadow_pixel(position_t screensz, position_t textpos, position_t bitpos,
                position_t fontsz)
{
    unsigned short *text = graphics_get_text_buf();
    const unsigned char *glyph;
    position_t glyphpos = { textpos.x, textpos.y };
    position_t glyphbit = { bitpos.x, bitpos.y };
    unsigned short ch;

    if (glyphbit.x == 0) {
        glyphbit.x = fontsz.x;
        glyphpos.x--;
    }
    if (glyphbit.y == 0) {
        glyphbit.y = fontsz.y;
        glyphpos.y--;
    }
    glyphbit.x--;
    glyphbit.y--;

    if (glyphpos.x < 0 || glyphpos.y < 0)
        return 0;

    ch = text[glyphpos.y * screensz.x + glyphpos.x] & 0xff;
    glyph = font8x16 + (ch << 4);
    return glyph[glyphbit.y] & (1 << ((fontsz.x-1) - glyphbit.x));
}

static void
bltbuf_draw_character(struct graphics_backend *backend,
        struct bltbuf *bltbuf,  /* the bltbuf to draw into */
        position_t target,      /* the position in the bltbuf to draw to */
        position_t fontsz,      /* the size of the font, in pixels */
        position_t charpos,     /* the position of the character in the text
                                   screen buffer */
        position_t screensz,    /* the size of the screen in characters */
        unsigned short ch       /* the character to draw, plus flags */
    )
{
    struct eg *eg = backend->priv;
    position_t blpos;
    position_t glyphpos;

    blpos.y = target.y;
    for (glyphpos.y = 0; glyphpos.y < fontsz.y; glyphpos.y++, blpos.y++) {
        blpos.x = target.x;
        for (glyphpos.x = 0; glyphpos.x < fontsz.x; glyphpos.x++, blpos.x++) {
            int invert = (ch & 0x0100) != 0;
            int set = (ch & 0x0200) != 0;
            const unsigned char *glyph = font8x16 + ((ch & 0xff) << 4);
            int bit = glyph[glyphpos.y] & (1 << ((fontsz.x-1) - glyphpos.x));
            int idx = -1;

            if (!set) {
                if (invert)
                    idx = bit ? 0 : 15;
                else if (bit)
                    idx = 15;

                if (idx == -1) {
                    if (is_shadow_pixel(screensz, charpos, glyphpos, fontsz) ||
                            !eg->background)
                        idx = invert ? 15 : 0;
                }
            } else {
                idx = bit ? 0 : 15;
            }

            if (idx != -1)
                bltbuf_set_pixel_idx(eg, bltbuf, &blpos, idx);
        }
    }
}

static void
bltbuf_draw_text(struct graphics_backend *backend,
        struct bltbuf *bltbuf,  /* the buffer to draw into */
        position_t screensz,    /* the size of the screen in characters */
        position_t fontsz,      /* the size of the font in pixels */
        position_t txtpos,      /* the position of the text on the screen
                                   (in characters) */
        position_t txtsz        /* the size of the block to fill in
                                   (in characters) */
    )
{
    struct eg *eg = backend->priv;
    unsigned short *text = graphics_get_text_buf();
    position_t charpos;

    for (charpos.y = txtpos.y; charpos.y < txtpos.y + txtsz.y; charpos.y++) {
        for (charpos.x = txtpos.x; charpos.x < txtpos.x + txtsz.x; charpos.x++){
            int offset = charpos.y * screensz.x + charpos.x;
            position_t blpos = { (charpos.x-txtpos.x)*fontsz.x,
                                 (charpos.y-txtpos.y)*fontsz.y };

            bltbuf_draw_character(backend, bltbuf, blpos, fontsz, charpos,
                    screensz, text[offset]);
        }
    }
}

static void
clbl(struct graphics_backend *backend, int col, int row, int width, int height,
        int draw_text)
{
    struct eg *eg = backend->priv;
    struct xpm *xpm;

    struct bltbuf *bltbuf;
    position_t fontsz, blpos, blsz, screensz;
    unsigned short *text;

//    blank(backend);
//
    xpm = graphics_get_splash_xpm();
    if (xpm && !eg->background)
        eg->background = xpm_to_bltbuf(xpm);

    graphics_get_screen_rowscols(&screensz);
    width = MIN(width, screensz.x - col);
    height = MIN(height, screensz.y - row);
    graphics_get_font_size(&fontsz);

    blsz.x = width * fontsz.x;
    blsz.y = height * fontsz.y;

    bltbuf = alloc_bltbuf(blsz.x, blsz.y);
    if (!bltbuf)
        return;

    blsz.x = col * fontsz.x;
    blsz.y = row * fontsz.y;

    text = graphics_get_text_buf();
    bltbuf_draw_bg(backend, bltbuf, blsz);

    if (draw_text) {
        blsz.x = width;
        blsz.y = height;
        blpos.x = col;
        blpos.y = row;

        bltbuf_draw_text(backend, bltbuf, screensz, fontsz, blpos, blsz);
    }

    blpos.x = col * fontsz.x;
    blpos.y = row * fontsz.y;

    blt_to_screen_pos(eg, bltbuf, &blpos);
}

static void
setup_cga_palette(struct eg *eg)
{
    rgb_to_pixel(0x00,0x00,0x00, &eg->palette[0]); //  0 Black
    rgb_to_pixel(0x7f,0x00,0x00, &eg->palette[1]); //  1 Dark Red
    rgb_to_pixel(0x00,0x7f,0x00, &eg->palette[2]); //  2 Dark Green
    rgb_to_pixel(0x7f,0x7f,0x00, &eg->palette[3]); //  3 Dark Yellow
    rgb_to_pixel(0x00,0x00,0x7f, &eg->palette[4]); //  4 Dark Blue
    rgb_to_pixel(0x7f,0x00,0x7f, &eg->palette[5]); //  5 Dark Magenta
    rgb_to_pixel(0x00,0x7f,0x7f, &eg->palette[6]); //  6 Dark Cyan
    rgb_to_pixel(0xc0,0xc0,0xc0, &eg->palette[7]); //  7 Light Grey
    rgb_to_pixel(0x7f,0x7f,0x7f, &eg->palette[8]); //  8 Dark Grey
    rgb_to_pixel(0xff,0x00,0x00, &eg->palette[9]); //  9 Red
    rgb_to_pixel(0x00,0xff,0x00, &eg->palette[10]); // 10 Green
    rgb_to_pixel(0xff,0xff,0x00, &eg->palette[11]); // 11 Yellow
    rgb_to_pixel(0x00,0x00,0xff, &eg->palette[12]); // 12 Blue
    rgb_to_pixel(0xff,0x00,0xff, &eg->palette[13]); // 13 Magenta
    rgb_to_pixel(0x00,0xff,0xff, &eg->palette[14]); // 14 Cyan
    rgb_to_pixel(0xff,0xff,0xff, &eg->palette[15]); // 15 White
    rgb_to_pixel(0xff,0xff,0xff, &eg->palette[16]); // 16 Also white ;)
}

static grub_efi_status_t
set_video_mode(struct eg *eg, int mode)
{
	grub_efi_status_t efi_status;
	efi_status = Call_Service_2(eg->output_intf->set_mode, eg->output_intf, mode);
	return efi_status;
}

static void disable(struct graphics_backend *backend)
{
    struct eg *eg;

    if (!backend)
        return;

    eg = backend->priv;
    if (!eg || eg->current_mode != GRAPHICS)
        return;

#if 0
    blank(backend);

    set_video_mode(eg, eg->text_mode);
    grub_efi_set_text_mode(1);
#endif
    eg->current_mode = TEXT;
}

static int
fill_pixel_info (grub_pixel_info_t *pixel_info,
		 grub_efi_graphics_output_mode_information_t *mode_info)
{
  if (mode_info->pixel_format == GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR)
    {
      pixel_info->depth_bits = 32;
      pixel_info->depth_bytes = 4;
      pixel_info->red_size = 8;
      pixel_info->red_pos = 0;
      pixel_info->green_size = 8;
      pixel_info->green_pos = 8;
      pixel_info->blue_size = 8;
      pixel_info->blue_pos = 16;
      pixel_info->reserved_size = 8;
      pixel_info->reserved_pos = 24;
      pixel_info->line_length = mode_info->pixels_per_scan_line * 4;
    }
  else if (mode_info->pixel_format == GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR)
    {
      pixel_info->depth_bits = 32;
      pixel_info->depth_bytes = 4;
      pixel_info->red_size = 8;
      pixel_info->red_pos = 16;
      pixel_info->green_size = 8;
      pixel_info->green_pos = 8;
      pixel_info->blue_size = 8;
      pixel_info->blue_pos = 0;
      pixel_info->reserved_size = 8;
      pixel_info->reserved_pos = 24;
      pixel_info->line_length = mode_info->pixels_per_scan_line * 4;
    }
  else if (mode_info->pixel_format == GRUB_EFI_PIXEL_BIT_MASK)
    {
      find_bits (mode_info->pixel_information.red_mask,
		 &pixel_info->red_pos, &pixel_info->red_size);
      find_bits (mode_info->pixel_information.green_mask,
		 &pixel_info->green_pos, &pixel_info->green_size);
      find_bits (mode_info->pixel_information.blue_mask,
		 &pixel_info->blue_pos, &pixel_info->blue_size);
      find_bits (mode_info->pixel_information.reserved_mask,
		 &pixel_info->reserved_pos, &pixel_info->reserved_size);
      pixel_info->depth_bits = pixel_info->red_size + pixel_info->green_size +
	pixel_info->blue_size + pixel_info->reserved_size;
      pixel_info->depth_bytes = (pixel_info->depth_bits + 7) / 8;
      pixel_info->line_length = mode_info->pixels_per_scan_line *
	pixel_info->depth_bytes;
    }
  else
    return 0;
  return 1;
}

/* 1 = prefer a
 * 0 = prefer neither
 * -1 = prefer b
 */
static int
modecmp_helper(struct eg *eg, struct video_mode *amode, struct video_mode *bmode)
{
        grub_efi_graphics_output_mode_information_t *a = amode->info;
        grub_efi_graphics_output_mode_information_t *b = bmode->info;

        if (a != NULL && b == NULL)
                return 1;
        if (a == NULL && b == NULL)
                return 0;
        if (a == NULL && b != NULL)
                return -1;

#if 0
	if (amode->number == eg->graphics_mode && bmode->number != eg->graphics_mode)
		return 1;
	if (amode->number == eg->graphics_mode && bmode->number == eg->graphics_mode)
		return 0;
	if (amode->number != eg->graphics_mode && bmode->number == eg->graphics_mode)
		return -1;
#endif


	/* kernel doesn't deal with blt only modes, so prefer against them. */
        if (a->pixel_format != GRUB_EFI_PIXEL_BLT_ONLY &&
                        b->pixel_format == GRUB_EFI_PIXEL_BLT_ONLY)
                return 1;
        if (b->pixel_format != GRUB_EFI_PIXEL_BLT_ONLY &&
                        a->pixel_format == GRUB_EFI_PIXEL_BLT_ONLY)
                return -1;

	/* XXX PJFIX there's something wrong with what we're passing to the
	 * kernel for stride in the bgrr/rgbr modes, and I haven't figured out
	 * just what yet, so for now, prefer bitmask modes.
	 */
	if (a->pixel_format == GRUB_EFI_PIXEL_BIT_MASK &&
			b->pixel_format != GRUB_EFI_PIXEL_BIT_MASK)
		return 1;
	if (a->pixel_format != GRUB_EFI_PIXEL_BIT_MASK &&
			b->pixel_format == GRUB_EFI_PIXEL_BIT_MASK)
		return -1;

        if (a->pixel_format == GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR &&
			b->pixel_format != GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR)
		return 1;
        if (a->pixel_format != GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR &&
			b->pixel_format == GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR)
		return -1;

        if (a->pixel_format == GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR &&
			b->pixel_format != GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR)
		return 1;
        if (a->pixel_format != GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR &&
			b->pixel_format == GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR)
		return -1;

        if (a->horizontal_resolution > b->horizontal_resolution &&
                        a->vertical_resolution > b->vertical_resolution)
                return 1;
        if (a->horizontal_resolution < b->horizontal_resolution &&
                        a->vertical_resolution < b->vertical_resolution)
                return -1;
        return 0;
}

static int
modecmp(struct eg *eg, struct video_mode *amode, struct video_mode *bmode)
{
        int rc;
#if 0
        grub_efi_graphics_output_mode_information_t *a = amode->info;
        grub_efi_graphics_output_mode_information_t *b = bmode->info;
#endif
        rc = modecmp_helper(eg, amode, bmode);
#if 0
        grub_printf("comparing nodes:\n");
        print_mode_info(amode);
        print_mode_info(bmode);
        if (rc > 0)
                grub_printf("result: a > b\n");
        else if (rc < 0)
                grub_printf("result: a < b\n");
        else
                grub_printf("result: a == b\n");

        //dbgdelay(__FILE__, __LINE__);
#endif
        return rc;
}

static void
modeswap(struct video_mode *amode, struct video_mode *bmode)
{
        struct video_mode tmp;

        memcpy(&tmp, amode, sizeof (tmp));
        memcpy(amode, bmode, sizeof (tmp));
        memcpy(bmode, &tmp, sizeof(tmp));
}

static void
sort_modes(struct eg *eg, int p, int r)
{
	struct video_mode **modes = eg->modes;

        int i, j;
	for (i = 0; i < eg->max_mode; i++) {
		for (j = i + 1; j < eg->max_mode; j++) {
			if (modecmp(eg, modes[j], modes[i]) < 0)
				modeswap(modes[j], modes[i]);
		}
	}
}

static int
try_enable(struct graphics_backend *backend)
{
    struct eg *eg = backend->priv;
    grub_efi_status_t efi_status = GRUB_EFI_UNSUPPORTED;
    int i;

    if (eg->text_mode == 0xffffffff) {
        grub_efi_set_text_mode(1);
        eg->text_mode = eg->output_intf->mode->mode;
    }

    if (eg->graphics_mode == 0xffffffff) {
        grub_efi_graphics_output_mode_information_t *info;

        if (!graphics_alloc_text_buf())
            return 0;

        grub_efi_set_text_mode(0);
        eg->graphics_mode = eg->output_intf->mode->mode;
        grub_efi_set_text_mode(1);
#if 0
	dprintf("graphics mode is %d\n", eg->graphics_mode);
	/* this is okay here because we haven't sorted yet.*/
	print_mode_info(eg->modes[eg->graphics_mode]);
	dprintf("text mode is %d\n", eg->text_mode);
	print_mode_info(eg->modes[eg->text_mode]);
#endif

        sort_modes(eg, 0, eg->max_mode-1);

#if 0
        for (i = eg->max_mode - 1; i >= 0; i--)
            print_mode_info(eg->modes[i]);
	dbgdelay(__FILE__, __LINE__);
#endif

	efi_status = GRUB_EFI_UNSUPPORTED;

        for (i = eg->max_mode - 1; i >= 0; i--) {
            if (!eg->modes[i])
                continue;

            info = eg->modes[i]->info;

#if 0
            if (info->pixel_format != GRUB_EFI_PIXEL_RGBR_8BIT_PER_COLOR &&
                 info->pixel_format != GRUB_EFI_PIXEL_BGRR_8BIT_PER_COLOR &&
                 info->pixel_format != GRUB_EFI_PIXEL_BIT_MASK) {
                continue;
            }
#endif

            grub_efi_set_text_mode(0);
            efi_status = set_video_mode(eg, eg->modes[i]->number);
            if (efi_status == GRUB_EFI_SUCCESS) {
#if 0
                grub_efi_set_text_mode(1);
	        dprintf("switched to mode %d successfully\n",
		        eg->modes[i]->number);
	        dbgdelay(__FILE__,__LINE__);
                grub_efi_set_text_mode(0);
#endif
                eg->graphics_mode = eg->modes[i]->number;
	        fill_pixel_info(&eg->pixel_info, info);
                break;
            } else {
#if 0
                set_video_mode(eg, eg->text_mode);
                grub_efi_set_text_mode(1);
		dprintf("return code was %d\n", efi_status);
#endif
            }
        }
        if (efi_status != GRUB_EFI_SUCCESS) {
#if 1
            grub_efi_set_text_mode(1);
            set_video_mode(eg, eg->text_mode);
#endif
            return 0;
        }

    }

    eg->current_mode = GRAPHICS;
    return 1;
}

static int
enable(struct graphics_backend *backend)
{
    struct eg *eg = backend->priv;
    int i;

    if (eg) {
        if (eg->current_mode == GRAPHICS) {
            return 1;
        }
    } else {
        grub_efi_status_t efi_status;
	grub_efi_handle_t *handle, *handles;
	grub_efi_uintn_t num_handles;
	grub_efi_pci_io_t *pci_proto;

        if (!(eg = grub_malloc(sizeof (*eg))))
            return 0;

        grub_memset(eg, '\0', sizeof (*eg));

        eg->backend = backend;
        eg->current_mode = TEXT;

	handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL,
					  &graphics_output_guid,
					  NULL, &num_handles);

	if (!num_handles || !handles)
	  goto fail;

	for (handle = handles; num_handles--; handle++)
	  {
	    pci_proto = grub_efi_open_protocol (*handle, &pci_io_guid,
					 GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	    if (!pci_proto)
	      continue;

	    eg->output_intf = grub_efi_open_protocol (*handle,
		   &graphics_output_guid, GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

	    if (eg->output_intf)
	      {
		grub_efi_setup_gfx_pci(*handle);
		break;
	      }
	  }

	grub_free(handles);

	if (!eg->output_intf)
	    eg->output_intf = grub_efi_locate_protocol(&graphics_output_guid, NULL);

        if (!eg->output_intf)
            goto fail;

        eg->text_mode = eg->graphics_mode = 0xffffffff;

        eg->max_mode = eg->output_intf->mode->max_mode;
        eg->modes = grub_malloc(eg->max_mode * sizeof (void *));
        if (!eg->modes)
            goto fail;
        memset(eg->modes, '\0', eg->max_mode * sizeof (void *));

        for (i = 0; i < eg->max_mode; i++) {
            eg->modes[i] = grub_malloc(sizeof *(eg->modes[0]));
            if (!eg->modes[i])
                goto fail;
            memset(eg->modes[i], '\0', sizeof *(eg->modes[0]));
            eg->modes[i]->number = i;

            efi_status = Call_Service_4(eg->output_intf->query_mode,
                    eg->output_intf, i, &eg->modes[i]->size,
                    &eg->modes[i]->info);
	    if (efi_status == GRUB_EFI_NOT_STARTED) {
		/* The firmware didn't turn on GRAPHICS_OUTPUT_PROTOCOL, so
		 * try to do so ourselves. Thanks, Intel. */
		set_video_mode(eg, eg->output_intf->mode->mode);
            	efi_status = Call_Service_4(eg->output_intf->query_mode,
                    eg->output_intf, i, &eg->modes[i]->size,
                    &eg->modes[i]->info);
	    }
            if (efi_status != GRUB_EFI_SUCCESS) {
                grub_free(eg->modes[i]);
                eg->modes[i] = NULL;
                //eg->max_mode = i;
                break;
            }
        }

        backend->priv = eg;
        setup_cga_palette(eg);
        for (i = 0; i < n_cga_colors; i++) {
            eg->palette[i].bgrr.red = cga_colors[i].bgrr.red;
            eg->palette[i].bgrr.green = cga_colors[i].bgrr.green;
            eg->palette[i].bgrr.blue = cga_colors[i].bgrr.blue;
        }
    }

    if (try_enable(backend)) {
        reset_screen_geometry(backend);
        return 1;
    }

fail:
    backend->priv = NULL;
    if (eg->modes) {
        for (i = 0; i < eg->max_mode; i++) {
            if (eg->modes[i])
                grub_free(eg->modes[i]);
        }
        grub_free(eg->modes);
    }
    grub_free(eg);
    return 0;
}

struct graphics_backend eg_backend = {
    .name = "eg",
    .enable = enable,
    .disable = disable,
    .set_kernel_params = set_kernel_params,
    .clbl = clbl,
    .set_palette = set_palette,
    .get_pixel_idx = get_pixel_idx,
    .get_pixel_rgb = get_pixel_rgb,
    .draw_pixel = draw_pixel,
    .reset_screen_geometry = reset_screen_geometry,
    .get_screen_size = get_screen_size,
    .getxy = eg_getxy,
    .setxy = setxy,
    .gotoxy = NULL,
    .cursor = cursor,
};

#endif /* SUPPORT_GRAPHICS */
