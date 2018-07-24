#ifndef GRUB_XPM_H
#define GRUB_XPM_H

#include <shared.h>

#define XPM_MAX_COLORS 31
#define XPM_MAX_WIDTH 640
#define XPM_MAX_HEIGHT 480

struct xpm_pixel {
    unsigned char reserved;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};
typedef struct xpm_pixel xpm_pixel_t;

struct xpm {
    int colors;
    xpm_pixel_t palette[32];

    int width;
    int height;
    unsigned char image[XPM_MAX_WIDTH * XPM_MAX_HEIGHT];
};

extern struct xpm *xpm_open(char *path);
extern void xpm_free(struct xpm *xpm);

extern unsigned char xpm_get_pixel_idx(struct xpm *xpm, int x, int y);
extern void xpm_set_pixel_idx(struct xpm *xpm, int x, int y, unsigned char idx);
extern int xpm_get_pixel(struct xpm *xpm, int x, int y, xpm_pixel_t *pixel);

extern void xpm_get_idx(struct xpm *xpm, unsigned char idx, xpm_pixel_t *pixel);

#endif /* GRUB_XPM_H */
