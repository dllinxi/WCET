#ifndef GRUB_EFI_GRAPHICS_H
#define GRUB_EFI_GRAPHICS_H

#ifdef SUPPORT_GRAPHICS

#include <term.h>
#include "xpm.h"

#define VIDEO_TYPE_EFI 0x70

typedef void pixel_t;
struct graphics;

struct position {
    int x;
    int y;
};
typedef struct position position_t;

extern void grub_efi_setup_gfx_pci (grub_efi_handle_t handle);

extern grub_uint16_t grub_console_getwh (void);
extern void graphics_set_kernel_params(struct linux_kernel_params *params);
extern void graphics_set_font_position(position_t *pos);
extern void graphics_get_font_position(position_t *pos);
extern void graphics_get_font_size(position_t *size);
extern int graphics_get_screen_rowscols(position_t *size);
extern int graphics_alloc_text_buf(void);
extern unsigned short *graphics_get_text_buf(void);

extern int graphics_get_scroll(void);

extern char *graphics_get_splash(void);
extern void graphics_setxy(int x, int y);
extern void graphics_cls(void);
extern void graphics_clbl(int x, int y, int width, int height, int draw_text);
extern void graphics_setcolorstate (color_state state);
extern void graphics_setcolor (int normal_color, int highlight_color);
extern int graphics_setcursor(int on);
extern int graphics_init(void);
extern void graphics_end(void);
extern struct xpm *graphics_get_splash_xpm(void);
extern void graphics_cursor(int set);
extern void graphics_scroll(void);

struct graphics_backend {
    char *name;
    struct graphics *graphics;
    void *priv;

    int (*enable)(struct graphics_backend *backend);
    void (*disable)(struct graphics_backend *backend);

    void (*set_kernel_params)(struct graphics_backend *backend,
            struct linux_kernel_params *params);

    void (*clbl)(struct graphics_backend *backend, int col, int row,
    						   int width, int height,
                                                   int draw_text);

    void (*set_palette)(struct graphics_backend *backend,
                        int idx, int red, int green, int blue);
    pixel_t *(*get_pixel_idx)(struct graphics_backend *backend, int idx);
    pixel_t *(*get_pixel_rgb)(struct graphics_backend *backend,
                              int red, int green, int blue);
    void (*draw_pixel)(struct graphics_backend *backend,
                       position_t *pos, pixel_t *pixel);

    void (*reset_screen_geometry)(struct graphics_backend *backend);
    void (*get_screen_size)(struct graphics_backend *backend, position_t *size);
    void (*getxy)(struct graphics_backend *backend, position_t *pos);
    void (*setxy)(struct graphics_backend *backend, position_t *pos);
    void (*gotoxy)(struct graphics_backend *backend, position_t *pos);
    void (*cursor)(struct graphics_backend *backend, int set);
//    void (*putchar)(struct graphics_backend *backend, int ch);
};

extern const unsigned char font8x16[];

#endif /* SUPPORT_GRAPHCIS */
#endif /* GRUB_EFI_GRAPHICS_H */
