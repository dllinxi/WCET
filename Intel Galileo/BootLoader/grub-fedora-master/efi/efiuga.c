/* efiuga.c - "univeral graphics adapter" support for GRUB/EFI */
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
#include <grub/misc.h>

#include <term.h>
#include <shared.h>
#include <graphics.h>

#include "graphics.h"
#include "xpm.h"

static grub_efi_guid_t draw_guid = GRUB_EFI_UGA_DRAW_GUID;
static grub_efi_guid_t pci_io_guid = GRUB_EFI_PCI_IO_GUID;

#if 0
#define UGA
#include "ugadebug.h"
#endif

#ifndef MIN
#define MIN(x,y) ( ((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ( ((x) < (y)) ? (y) : (x))
#endif

#define pixel_equal(x,y) ((x).red == (y).red && \
                          (x).green == (y).green && \
                          (x).blue == (y).blue)

struct video_mode {
    grub_efi_uint32_t horizontal_resolution;
    grub_efi_uint32_t vertical_resolution;
    grub_efi_uint32_t color_depth;
    grub_efi_uint32_t refresh_rate;
};

static void
xpnd_6_to_8(grub_efi_uga_pixel_t *pixel)
{
    if (!((pixel->red | pixel->green | pixel->blue) & 0xc0)) {
        pixel->red <<= 2;
        pixel->red |= ((pixel->red & 0xc0) >> 6);
        pixel->red &= 0xff;
        pixel->green <<= 2;
        pixel->green |= ((pixel->green & 0xc0) >> 6);
        pixel->green &= 0xff;
        pixel->blue <<= 2;
        pixel->blue |= ((pixel->blue & 0xc0) >> 6);
        pixel->blue &= 0xff;
    }
}



#define MAX_PALETTE 16
struct uga {
    grub_efi_uga_draw_t *draw_intf;
    struct video_mode graphics_mode;
    struct video_mode text_mode;
    enum { TEXT, GRAPHICS } current_mode;

    position_t screen_size;
    position_t screen_pos;

    struct bltbuf *background;

    grub_efi_uga_pixel_t palette[MAX_PALETTE + 1];
};

#define RGB(r,g,b) { .red = r, .green = g, .blue = b }

static grub_efi_uga_pixel_t cga_colors[] = {
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
set_kernel_params(struct graphics_backend *backend,
            struct linux_kernel_params *params)
{
    struct uga *uga;
    
    if (!backend || !backend->priv)
        return;

    uga = backend->priv;

    params->lfb_width = uga->graphics_mode.horizontal_resolution;
    params->lfb_height = uga->graphics_mode.vertical_resolution;
    params->lfb_depth = 32;
    params->red_mask_size = 8;
    params->red_field_pos = 16;
    params->green_mask_size = 8;
    params->green_field_pos = 8;
    params->blue_mask_size = 8;
    params->blue_field_pos = 0;
    params->reserved_mask_size = 8;
    params->reserved_field_pos = 24;
    params->have_vga = VIDEO_TYPE_EFI;
}

static void
pixel_to_rgb(grub_efi_uga_pixel_t *pixel, int *red, int *green, int *blue)
{
    *red = pixel->red;
    *green = pixel->green;
    *blue = pixel->blue;
}

static void
rgb_to_pixel(int red, int green, int blue, grub_efi_uga_pixel_t *pixel)
{
    pixel->red = red;
    pixel->green = green;
    pixel->blue = blue;
}

static void
position_to_phys(struct uga *uga, position_t *virt, position_t *phys)
{
    phys->x = virt->x + uga->screen_pos.x;
    phys->y = virt->y + uga->screen_pos.y;
}

static int
abs_paddr(struct uga *uga, position_t *virt)
{
    position_t phys;
    position_to_phys(uga, virt, &phys);
    return phys.x + phys.y * uga->screen_size.x;
}

struct bltbuf {
    grub_efi_uintn_t width;
    grub_efi_uintn_t height;
    grub_efi_uga_pixel_t pixbuf[];
};

static struct bltbuf *alloc_bltbuf(grub_efi_uintn_t width,
					   grub_efi_uintn_t height)
{
	struct bltbuf *buf = NULL;
	grub_efi_uintn_t pixbuf_size = width * height *
		sizeof (grub_efi_uga_pixel_t);

	if (!(buf = grub_malloc(sizeof(buf->width) + sizeof(buf->height) +
				pixbuf_size)))
		return NULL;

	buf->width = width;
	buf->height = height;
	grub_memset(buf->pixbuf, '\0', pixbuf_size);
	return buf;
}

static void
blt_to_screen(struct uga *uga, struct bltbuf *bltbuf)
{
    position_t addr = {0, 0};

    position_to_phys(uga, &addr, &addr);

    Call_Service_10(uga->draw_intf->blt, uga->draw_intf, bltbuf->pixbuf,
    		    EfiUgaBltBufferToVideo,
                    0, 0,
                    addr.x, addr.y,
                    bltbuf->width, bltbuf->height,
                    0);
}

static void
blt_pos_to_screen_pos(struct uga *uga, struct bltbuf *bltbuf,
                      position_t *bltpos, position_t *bltsz, position_t *pos)
{
    position_t phys;

    position_to_phys(uga, pos, &phys);

    Call_Service_10(uga->draw_intf->blt, uga->draw_intf, bltbuf->pixbuf,
                    EfiUgaBltBufferToVideo,
                    bltpos->x, bltpos->y,
                    phys.x, phys.y,
                    bltsz->x, bltsz->y,
                    0);
}

static void
blt_to_screen_pos(struct uga *uga, struct bltbuf *bltbuf, position_t *pos)
{
    position_t bltpos = {0, 0};
    position_t bltsz = { bltbuf->width, bltbuf->height };
    blt_pos_to_screen_pos(uga, bltbuf, &bltpos, &bltsz, pos);
}

static int
blt_from_screen_pos(struct uga *uga, struct bltbuf **retbuf,
    position_t *pos, position_t *size)
{
    struct bltbuf *bltbuf = NULL;
    position_t phys;

    if (!retbuf)
        return 0;

    if (*retbuf)
        grub_free(*retbuf);

    bltbuf = alloc_bltbuf(size->x, size->y);
    if (!bltbuf)
        return 0;

    position_to_phys(uga, pos, &phys);

    Call_Service_10(uga->draw_intf->blt, uga->draw_intf, bltbuf->pixbuf,
            EfiUgaVideoToBltBuffer,
            phys.x, phys.y,
            0, 0,
            size->x, size->y, 0);
    *retbuf = bltbuf;
    return 1;
}

static int
save_video_mode(struct uga *uga, struct video_mode *mode)
{
	grub_efi_status_t status;

	status = Call_Service_5(uga->draw_intf->get_mode, uga->draw_intf,
		&mode->horizontal_resolution, &mode->vertical_resolution,
		&mode->color_depth, &mode->refresh_rate);
	return status == GRUB_EFI_SUCCESS;
}

static grub_efi_status_t
set_video_mode(struct uga *uga, struct video_mode *mode)
{
    grub_efi_status_t status;

    status = Call_Service_5(uga->draw_intf->set_mode, uga->draw_intf,
                            mode->horizontal_resolution,
                            mode->vertical_resolution,
                            mode->color_depth,
                            mode->refresh_rate);
    return status;
}

static int
cmp_video_modes(struct video_mode *vm0, struct video_mode *vm1)
{
    if (vm0->horizontal_resolution == vm1->horizontal_resolution &&
            vm0->vertical_resolution == vm1->vertical_resolution)
        return 0;
    if (vm0->horizontal_resolution >= vm1->horizontal_resolution &&
            vm0->vertical_resolution >= vm1->vertical_resolution)
        return 1;
    return -1;
}

static int
blt_from_screen(struct uga *uga, struct bltbuf **retbuf)
{
    struct bltbuf *bltbuf = NULL;
    position_t pos = {0 ,0};
    position_t size = {
            .x = uga->graphics_mode.horizontal_resolution,
            .y = uga->graphics_mode.vertical_resolution
        };

    return blt_from_screen_pos(uga, retbuf, &pos, &size);
}

static void 
bltbuf_set_pixel(struct bltbuf *bltbuf, position_t *pos,
                             grub_efi_uga_pixel_t *pixel)
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
                 grub_efi_uga_pixel_t *pixel)
{
    if (bltbuf && pos->x < bltbuf->width && pos->y < bltbuf->height) {
    	grub_memmove(pixel, &bltbuf->pixbuf[pos->x + pos->y * bltbuf->width],
            sizeof *pixel);
    } else {
	pixel->red = 0x00;
	pixel->green = 0x00;
	pixel->blue = 0x00;
    }
}

static void
bltbuf_set_pixel_rgb(struct bltbuf *bltbuf, position_t *pos,
                     int red, int green, int blue)
{
    grub_efi_uga_pixel_t pixel;
    rgb_to_pixel(red, green, blue, &pixel);
    bltbuf_set_pixel(bltbuf, pos, &pixel);
}

static void
bltbuf_set_pixel_idx(struct uga *uga, struct bltbuf *bltbuf,
                     position_t *pos, int idx)
{
    bltbuf_set_pixel(bltbuf, pos, &uga->palette[idx]);
}

static void
bltbuf_get_pixel_idx(struct bltbuf *bltbuf, position_t *pos, int *idx)
{
    grub_efi_uga_pixel_t pixel;
    
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
    struct uga *uga;
    int ch, invert;
    unsigned short *text;
    position_t fpos, screensz;
    int offset;
    
    uga = backend->priv;
    
    if (set && !graphics_get_scroll())
        return;

    text = graphics_get_text_buf();
    graphics_get_font_position(&fpos);
    graphics_get_screen_rowscols(&screensz);

    offset = fpos.y * screensz.x + fpos.x;

    if (set)
        text[offset] |= 0x200;

    graphics_clbl(fpos.x, fpos.y, 1, 1, 1);

    if (set)
        text[offset] &= 0xfdff;
}

static void blank(struct graphics_backend *backend);

static void
reset_screen_geometry(struct graphics_backend *backend)
{
    struct uga *uga = backend->priv;
    struct xpm *xpm = graphics_get_splash_xpm();
    position_t screensz = { .x = 640, .y = 480 };

    if (xpm) {
        screensz.x = xpm->width;
        screensz.y = xpm->height;
    }

    uga->screen_pos.x =
        (uga->graphics_mode.horizontal_resolution - screensz.x) / 2;
    uga->screen_pos.y =
        (uga->graphics_mode.vertical_resolution - screensz.y) / 2;

    blank(backend);
    graphics_get_screen_rowscols(&screensz);
    graphics_clbl(0, 0, screensz.x, screensz.y, 0);
    graphics_clbl(0, 0, screensz.x, screensz.y, 1);
}

static void
get_screen_size(struct graphics_backend *backend, position_t *size)
{
    struct uga *uga = backend->priv;
    size->x = uga->graphics_mode.horizontal_resolution;
    size->y = uga->graphics_mode.vertical_resolution;
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
uga_getxy(struct graphics_backend *backend, position_t *pos)
{
    graphics_get_font_position(pos);
}

static void
draw_pixel(struct graphics_backend *backend, position_t *pos, pixel_t *pixel)
{
    struct bltbuf *bltbuf = NULL;
    grub_efi_uga_pixel_t *eup = (grub_efi_uga_pixel_t *)pixel;

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
    static grub_efi_uga_pixel_t pixel;
    struct uga *uga = backend->priv;
    if (idx < 0 || idx > MAX_PALETTE)
        return NULL;
    pixel.red = uga->palette[idx].red;
    pixel.green = uga->palette[idx].green;
    pixel.blue = uga->palette[idx].blue;
    return (pixel_t *)&pixel;
}

static pixel_t *
get_pixel_rgb(struct graphics_backend *backend, int red, int green, int blue)
{
    static grub_efi_uga_pixel_t pixel;
    rgb_to_pixel(red, green, blue, &pixel);
    return &pixel;
}

static void
set_palette(struct graphics_backend *backend, int idx,
            int red, int green, int blue)
{
    grub_efi_uga_pixel_t pixel;
    struct uga *uga = backend->priv;

    if (idx > MAX_PALETTE)
        return;
    rgb_to_pixel(red, green, blue, &pixel);
    grub_memmove(&uga->palette[idx], &pixel, sizeof pixel);
}

static void
blank(struct graphics_backend *backend)
{
    struct uga *uga = backend->priv;
    struct bltbuf *bltbuf;
    position_t pos = {0, 0};
    grub_efi_uintn_t x, y, i, j;
    unsigned char r = 0 ,g = 0;

    x = uga->graphics_mode.horizontal_resolution;
    y = uga->graphics_mode.vertical_resolution;

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

    Call_Service_10(uga->draw_intf->blt, uga->draw_intf, bltbuf->pixbuf,
        EfiUgaBltBufferToVideo, 0, 0, 0, 0, x, y, 0);

    grub_free(bltbuf);
}

static void
bltbuf_cp_bl(struct bltbuf *d, position_t dpos,
             struct bltbuf *s, position_t spos)
{
    grub_efi_uga_pixel_t *dp, *sp;

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
    struct uga *uga = backend->priv;
    position_t blpos = { 0, 0 };

    bltbuf_cp_bl(bltbuf, blpos, uga->background, bgpos);
}

static void
dbg_dump_palette(struct graphics_backend *backend)
{
    struct uga *uga;
    int i;
    if (!backend || !backend->priv)
        return;
    uga = backend->priv;
    if (!uga->palette)
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
    struct uga *uga = backend->priv;
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
                            !uga->background)
                        idx = invert ? 15 : 0;
                }
            } else {
                idx = bit ? 0 : 15;
            }

            if (idx != -1)
                bltbuf_set_pixel_idx(uga, bltbuf, &blpos, idx);
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
    struct uga *uga = backend->priv;
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
    struct uga *uga = backend->priv;
    struct xpm *xpm;

    struct bltbuf *bltbuf;
    position_t fontsz, blpos, blsz, screensz;
    unsigned short *text;

//    blank(backend);
//
    xpm = graphics_get_splash_xpm();
    if (xpm && !uga->background)
        uga->background = xpm_to_bltbuf(xpm);

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

    blt_to_screen_pos(uga, bltbuf, &blpos);
}

static void
setup_cga_palette(void)
{
/*
 * Nov 27 15:53:19 <pjones> ping?
 * Nov 27 15:54:13 <ajax> pong?
 * Nov 27 15:54:16 <pjones> say I've got a color channel in 6 bits, and it's
 *                          stored in an 8-bit value.  let's call it white.
 *                          now let's say I've got hardware with 8 bits per
 *                          channel that I'm blitting to.  What's the right
 *                          way to expand 0x3f to 0xff?
 * Nov 27 15:55:17 <pjones> (my naive idea, which I haven't tried yet, is
 *                          "x <<= 2; if (x) x |= 3;")
 * Nov 27 15:55:35 <ajax> no.
 * Nov 27 15:55:43 <pjones> yeah, figured I'd ask instead of doing that.
 * Nov 27 15:56:15 <ajax>  x <<= 2; x |= ((x & 0xc0) >> 6);
 */
    int i;
    for (i = 0; i < n_cga_colors; i++) {
        grub_efi_uga_pixel_t *pixel = &cga_colors[i];

        if ((pixel->red | pixel->green | pixel->blue) & 0xc0)
            continue;
        pixel->red <<= 2;
        pixel->red |= ((pixel->red & 0xc0) >> 6);
        pixel->red &= 0xff;
        pixel->green <<= 2;
        pixel->green |= ((pixel->green & 0xc0) >> 6);
        pixel->green &= 0xff;
        pixel->blue <<= 2;
        pixel->blue |= ((pixel->blue & 0xc0) >> 6);
        pixel->blue &= 0xff;
    }
}

static int
try_enable(struct graphics_backend *backend)
{
    struct uga *uga = backend->priv;
    struct video_mode modes[] = {
        { 1440, 900, 32, 60 },
        { 640, 480, 32, 60 },
        { 800, 600, 32, 60 },
        { 640, 480, 16, 60 },
        { 800, 600, 16, 60 },
        { 640, 480, 8, 60 },
        { 800, 600, 8, 60 }
    };
    struct video_mode mode;
    position_t font_size;
    int i;

    if (uga->text_mode.horizontal_resolution == 0) {
        grub_efi_set_text_mode(1);
        save_video_mode(uga, &uga->text_mode);
    }

    if (uga->graphics_mode.horizontal_resolution == 0) {
        grub_efi_set_text_mode(0);
        save_video_mode(uga, &uga->graphics_mode);

        if (cmp_video_modes(&uga->graphics_mode, &modes[0]) >= 0) {
            uga->current_mode = GRAPHICS;
            graphics_alloc_text_buf();
            return 1;
        }
        for (i = 0; i < sizeof (modes) / sizeof (modes[0]); i++) {
            set_video_mode(uga, &modes[i]);
            save_video_mode(uga, &mode);
            if (!cmp_video_modes(&modes[i], &mode)) {
                grub_memmove(&uga->graphics_mode, &mode, sizeof (mode));
                uga->current_mode = GRAPHICS;
                graphics_alloc_text_buf();
                return 1;
            }
        }
        set_video_mode(uga, &uga->graphics_mode);
        save_video_mode(uga, &mode);
        if (!cmp_video_modes(&uga->graphics_mode, &mode)) {
            uga->current_mode = GRAPHICS;
            graphics_alloc_text_buf();
            return 1;
        }
        grub_efi_set_text_mode(1);
        return 0;
    }
    set_video_mode(uga, &uga->graphics_mode);

    uga->current_mode = GRAPHICS;
    return 1;
}

static int
enable(struct graphics_backend *backend)
{
    struct uga *uga = backend->priv;
    grub_efi_handle_t *handle, *handles;
    grub_efi_uintn_t num_handles;
    grub_efi_pci_io_t *pci_proto;
    int i;

    if (uga) {
        if (uga->current_mode == GRAPHICS) {
            return 1;
        }
    } else {
        if (!(uga = grub_malloc(sizeof (*uga)))) {
            return 0;
        }
	grub_memset(uga, '\0', sizeof (*uga));

        uga->current_mode = TEXT;

        handles = grub_efi_locate_handle (GRUB_EFI_BY_PROTOCOL,
                                          &draw_guid, NULL, &num_handles);

        if (!num_handles || !handles)
	  {
            grub_free(uga);
	    return 0;
	  }

	for (handle = handles; num_handles--; handle++)
          {
            pci_proto = grub_efi_open_protocol (*handle, &pci_io_guid,
				          GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

            if (!pci_proto)
              continue;

            uga->draw_intf = grub_efi_open_protocol (*handle, &draw_guid,
					  GRUB_EFI_OPEN_PROTOCOL_GET_PROTOCOL);

            if (uga->draw_intf)
              {
                grub_efi_setup_gfx_pci(*handle);
                break;
              }
          }

	grub_free(handles);

	if (!uga->draw_intf)
	    uga->draw_intf = grub_efi_locate_protocol(&draw_guid, NULL);

        if (!uga->draw_intf) {
            grub_free(uga);
            return 0;
        }
        grub_memset(&uga->graphics_mode, '\0', sizeof (uga->graphics_mode));
        grub_memset(&uga->text_mode, '\0', sizeof (uga->text_mode));
        backend->priv = uga;

        setup_cga_palette();
        for (i = 0; i < n_cga_colors; i++) {
            uga->palette[i].red = cga_colors[i].red;
            uga->palette[i].green = cga_colors[i].green;
            uga->palette[i].blue = cga_colors[i].blue;
        }
    }

    if (try_enable(backend)) {
        reset_screen_geometry(backend);
        return 1;
    }
    
    backend->priv = NULL;
    grub_free(uga);
    return 0;
}

static void disable(struct graphics_backend *backend)
{
    struct uga *uga;
    
    if (!backend)
        return;
    
    uga = backend->priv;
    if (!uga || uga->current_mode != GRAPHICS)
        return;

#if 0
    blank(backend);

    set_video_mode(uga, &uga->text_mode);
    grub_efi_set_text_mode(1);
#endif
    uga->current_mode = TEXT;
}

struct graphics_backend uga_backend = {
    .name = "uga",
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
    .getxy = uga_getxy,
    .setxy = setxy,
    .gotoxy = NULL,
    .cursor = cursor,
};

#endif /* SUPPORT_GRAPHICS */
