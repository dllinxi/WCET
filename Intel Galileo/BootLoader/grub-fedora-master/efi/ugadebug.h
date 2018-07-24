#ifndef UGA_DEBUG_H
#define UGA_DEBUG_H

extern void dc(char *file, int line);
extern void gm(char *file, int line, char *fmt, ...);

#ifdef UGA

extern const unsigned char font8x16[];

extern grub_efi_guid_t draw_guid;

static void *tmp_term;

#define SCREEN_WIDTH 80

#define LINE_SIZE_X ( 8 * 100 )
#define LINE_START_X(x) (1280 + (x))
#define LINE_END_X(x) (LINE_START_X(0) + LINE_SIZE_X - 1)

#define POS_X(x) (LINE_START_X(x) + (8 * (x)))

#define LINE_SIZE_Y 16
#define LINE_START_Y(y) ((y) * LINE_SIZE_Y)
#define LINE_END_Y(y) ( (((y)+1) * LINE_SIZE_Y ) - 1)

#define POS_Y(y) LINE_START_Y(y)

#define FIRST_LINE 0
#define LAST_LINE 75

#define format_ascii(buf, val, is_hex, is_cap) ({                   \
        int _n = sizeof ((buf)) - 2;                                \
        typeof(val) _nval = (val);                                  \
        int _negative = 0;                                          \
        int _mult = is_hex ? 16 : 10;                               \
        char _a = is_cap ? 'A' : 'a';                               \
        grub_memset((buf), '\0', sizeof ((buf)));                        \
        if (!(_nval > 0LL))                                         \
            _negative = 1;                                          \
        if (_nval == 0LL)                                           \
            _negative = 0;                                          \
        if (_negative)                                              \
            _nval = (_nval ^ -1);                                   \
        do {                                                        \
            int _dig = _nval % _mult;                               \
            (buf)[_n--] = ((_dig > 9) ? _dig + _a - 10 : '0'+_dig); \
        } while (_nval /= _mult);                                   \
        if (_negative)                                              \
            (buf)[_n--] = '-';                                      \
        _mult = 0;                                                  \
        _n++;                                                       \
        while (_n < sizeof ((buf)))                                 \
            (buf)[_mult++] = (buf)[_n++];                           \
        if (_negative && _mult > 1)                                 \
            ((buf)[_mult-2])++;                                     \
    })

static grub_efi_uga_draw_t *debug_draw_intf;
static void debug_putchar(int i, int j, int ch)
{
    grub_efi_uga_pixel_t pixbuf[8 * 16];
    grub_efi_uga_pixel_t black = { .red = 0x00, .green = 0x00, .blue = 0x00 },
                         white = { .red = 0xff, .green = 0xff, .blue = 0xff };
    const unsigned char *pat;
    int x, y;

    if (!debug_draw_intf)
	debug_draw_intf = grub_efi_locate_protocol(&draw_guid, NULL);
    if (!debug_draw_intf)
        return;

    pat = font8x16 + (ch << 4);

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 8; x++) {
            int bit = pat[y] & (1 << (7 - x));

            grub_memmove(&pixbuf[x + y * 8], bit ? &white : &black,
                    sizeof (pixbuf[0]));
        }
    }

    Call_Service_10(debug_draw_intf->blt, debug_draw_intf, pixbuf,
    		    EfiUgaBltBufferToVideo,
                    0, 0,
		    POS_X(i), POS_Y(j),
                    8, 16,
                    0);

}

static void dputs(const char *s)
{
    static int row = -1;
    int col = 89;
    return;

    if (s && *s) {
        do {
            if (col > 190) {
                row++;
                col = 89;
            }
            if (row > 50) {
                row = 0;
                col = 89;
            }
            col++;

            if (*s == '\n') {
                col = 1;
                row++;
            } else {
                debug_putchar(col, row, *s);
            }
        } while (*(++s));
    }
    row++;
}

static int dbg_row = 0;
static int dbg_col = 0;

static void dbg_scroll(int new_dbg_col)
{
    grub_efi_uga_pixel_t black[1] ={{.red = 0x00, .green = 0x00, .blue = 0x00}};

    if (dbg_row == LAST_LINE) {
        dbg_col = new_dbg_col;
        Call_Service_10(debug_draw_intf->blt, debug_draw_intf, NULL,
    		    EfiUgaVideoToVideo,
                    640, LINE_START_Y(1),
                    640, LINE_START_Y(0),
                    LINE_SIZE_X, LINE_END_Y(LAST_LINE)+1,
                    0);
        Call_Service_10(debug_draw_intf->blt, debug_draw_intf, black,
    		    EfiUgaVideoFill,
                    0, 0,
                    640, LINE_START_Y(LAST_LINE),
                    LINE_END_X(SCREEN_WIDTH)+1, LINE_END_Y(LAST_LINE)+1,
                    0);
    } else {
        dbg_row++;
        dbg_col = new_dbg_col;
    }
//    grub_efi_stall(1000000);
}

static void dc_backend(char *file, int line)
{
    char linebuf[9] = "        ";
    int pos;

    linebuf[8] = '\0';

    format_ascii(linebuf, line, 0, 0);
    for (dbg_col=0, pos=0; linebuf[pos]; dbg_col++, pos++)
        debug_putchar(dbg_col, dbg_row, linebuf[pos]);

    for (dbg_col=5, pos=0; file[pos]; dbg_col++, pos++) {
        if (dbg_col > 15)
            dbg_scroll(0);

        debug_putchar(dbg_col, dbg_row, file[pos]);
    }
}

void dc(char *file, int line)
{
	dc_backend(file, line);
	dbg_scroll(0);
}

void
gm(char *file, int line, char *fmt, ...)
{
    va_list ap;
    char buf[1024]="";
    int pos;

    va_start(ap, fmt);
    grub_vsprintf(buf, fmt, ap);
    va_end(ap);

    dc_backend(file, line);
    for (dbg_col=16, pos=0; buf[pos]; dbg_col++, pos++) {
        if (buf[pos] == '\n')
            continue;
        if (dbg_col > SCREEN_WIDTH-1)
            dbg_scroll(16);
        debug_putchar(dbg_col, dbg_row, buf[pos]);
    }
    dbg_scroll(0);
}

#define dm() ({ tmp_term = current_term; current_term = term_table; grub_dprintf(__func__, "got here\n"); current_term = tmp_term; })

#endif

#endif /* UGA_DEBUG_H */
