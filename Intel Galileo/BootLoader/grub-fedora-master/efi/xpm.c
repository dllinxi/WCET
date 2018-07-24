
#if defined(SUPPORT_GRAPHICS) || defined(PLATFORM_EFI) || 1

#include <grub/efi/api.h>
#include <grub/efi/efi.h>
#include <grub/misc.h>

#include "xpm.h"

unsigned char
xpm_get_pixel_idx(struct xpm *xpm, int x, int y)
{
    if (x < 0 || x >= xpm->width)
        return -1;
    if (y < 0 || y >= xpm->width)
        return -1;
    return xpm->image[x + (y * xpm->width)];
}

void
xpm_set_pixel_idx(struct xpm *xpm, int x, int y, unsigned char idx)
{
    xpm->image[y * xpm->width + x] = idx;
}

void
xpm_get_idx(struct xpm *xpm, unsigned char idx, xpm_pixel_t *pixel)
{
    xpm_pixel_t *xpl;
    if (!pixel)
        return;
    if (idx >= xpm->colors) {
        pixel->red = pixel->green = pixel->blue = 0;
        return;
    }
    xpl = &xpm->palette[idx];
    memcpy(pixel, xpl, sizeof (*pixel));
//    pixel->red = xpl->red;
//    pixel->green = xpl->green;
//    pixel->blue = xpl->blue;
}

int
xpm_get_pixel(struct xpm *xpm, int x, int y, xpm_pixel_t *pixel)
{
    unsigned char idx;
    idx = xpm_get_pixel_idx(xpm, x, y);
    xpm_get_idx(xpm, idx, pixel);
    return 1;
}

static int
hex_to_int(int v)
{
    if (v >= 'A' && v <= 'F')
        return (v - 'A' + 10);
    if (v >= 'a' && v <= 'f')
        return (v - 'a' + 10);
    return (v - '0');
}

struct xpm *
xpm_open(char *path)
{
    char c, prev, target[]="/* XPM */\n", base;
    char buf[32];
    int pos = 0;
    unsigned int i, idx, len, x, y;
    unsigned char pal[XPM_MAX_COLORS];
    struct xpm *xpm;

    xpm = grub_malloc(sizeof (*xpm));
    if (!xpm)
        return NULL;

    if (!grub_open(path)) {
        grub_free(xpm);
        grub_printf("grub_open(\"%s\") failed\n", path);
        return NULL;
    }

    grub_memset(xpm, '\0', sizeof (*xpm));

    prev = '\n';
    c = 0;
    do {
        if (grub_read(&c, 1) != 1) {
            grub_printf("grub_read() failed\n");
            grub_free(xpm);
            grub_close();
            return NULL;
        }
        if ((pos == 0 && prev == '\n') || pos > 0) {
            if (c == target[pos])
                pos++;
            else
                pos = 0;
        }
        prev = c;
    } while (target[pos]);

    /* parse info */
    while (grub_read(&c, 1)) {
        if (c == '"')
            break;
    }
    while (grub_read(&c, 1) && (c == ' ' || c == '\t'))
        ;

    i = 0;
    xpm->width = c - '0';
    while (grub_read(&c, 1)) {
        if (c >= '0' && c <= '9')
            xpm->width = xpm->width * 10 + c - '0';
        else
            break;
    }
    if (xpm->width > XPM_MAX_WIDTH) {
        grub_printf("xpm->width (%d) was greater than XPM_MAX_WIDTH (%d)\n",
                xpm->width, XPM_MAX_WIDTH);
        grub_free(xpm);
        grub_close();
        return NULL;
    }
    while (grub_read(&c, 1) && (c == ' ' || c == '\t'))
        ;

    xpm->height = c - '0';
    while (grub_read(&c, 1)) {
        if (c >= '0' && c <= '9')
            xpm->height = xpm->height * 10 + c - '0';
        else
            break;
    }
    if (xpm->height > XPM_MAX_HEIGHT) {
        grub_printf("xpm->height (%d) was greater than XPM_MAX_HEIGHT (%d)\n",
                xpm->height, XPM_MAX_HEIGHT);
        grub_free(xpm);
        grub_close();
        return NULL;
    }

    while (grub_read(&c, 1) && (c == ' ' || c == '\t'))
        ;

    xpm->colors = c - '0';
    while (grub_read(&c, 1)) {
        if (c >= '0' && c <= '9')
            xpm->colors = xpm->colors * 10 + c - '0';
        else
            break;
    }

    base = 0;
    while (grub_read(&c, 1) && c != '"')
        ;

    /* palette */
    for (i = 0, idx = 1; i < xpm->colors; i++) {
        len = 0;

        while (grub_read(&c, 1) && c != '"')
            ;
        grub_read(&c, 1);       /* char */
        base = c;
        grub_read(buf, 4);      /* \t c # */

        while (grub_read(&c, 1) && c != '"') {
            if (len < sizeof(buf))
                buf[len++] = c;
        }

        if (len == 6 && idx < xpm->colors) {
            unsigned char r, g, b;
            
            r = (hex_to_int(buf[0]) << 4) | hex_to_int(buf[1]);
            g = (hex_to_int(buf[2]) << 4) | hex_to_int(buf[3]);
            b = (hex_to_int(buf[4]) << 4) | hex_to_int(buf[5]);

            pal[idx] = base;
            xpm->palette[idx].red = r;
            xpm->palette[idx].green = g;
            xpm->palette[idx].blue = b;
            ++idx;
        }
    }

    /* parse xpm data */
    x = y = 0;
    while (y < xpm->height) {
        xpm_pixel_t *pixel = NULL;
        while (1) {
            if (!grub_read(&c, 1)) {
                grub_printf("%s %s:%d grub_read() failed\n", __FILE__, __func__, __LINE__);
                grub_free(xpm);
                grub_close();
                return NULL;
            }
            if (c == '"')
                break;
        }

        while (grub_read(&c, 1) && c != '"') {
            unsigned char *iaddr = NULL;
            for (i = 1; i < xpm->colors; i++)
                if (pal[i] == c) {
                    idx = i;
                    break;
                }

            xpm_set_pixel_idx(xpm, x, y, idx);
           
            if (++x >= xpm->width) {
                x = 0;

                ++y;
            }
        }
    }
    grub_close();
    return xpm;
}

void
xpm_free(struct xpm *xpm)
{
    grub_free(xpm);
}


#endif /* SUPPORT_GRAPHICS || PLATFORM_EFI */
