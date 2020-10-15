#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot2.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

/* import our font that's in the object file we've created above */
extern char _binary_font_psfu_start;
extern char _binary_font_psfu_end;

/* the linear framebuffer */
char *fb;
/* number of bytes in each line, it's possible it's not screen width * bytesperpixel! */
int scanline;
/* import our font that's in the object file we've created above */
// extern char _binary_font_start;

uint16_t *unicode;

/**
 * Writes the specified character to the framebuffer
 *
 * @param c  - An int representing the unicode character
 * @param cx - X position (in characters)
 * @param cy - Y position (in characters)
 * @param fg - Foreground color
 * @param bg - Background color
 */
void putchar(unsigned short int c, int cx, int cy, uint32_t fg, uint32_t bg) {
        /* cast the address to PSF header struct */
        PSF_font *font = (PSF_font*)&_binary_font_psfu_start;
        /* we need to know how many bytes encode one row */
        int bytesperline=(font->width+7)/8;
        /* unicode translation */
        if(unicode != NULL) {
                c = unicode[c];
        }
        /* get the glyph for the character. If there's no
         *        glyph for a given character, we'll display the first glyph. */
        unsigned char *glyph =
                (unsigned char*)&_binary_font_psfu_start +
                font->headersize +
                (c>0&&c<font->numglyph?c:0)*font->bytesperglyph;
        /* calculate the upper left corner on screen where we want to display.
         *        we only do this once, and adjust the offset later. This is faster. */
        int offs =
                (cy * font->height * scanline) +
                (cx * (font->width+1) * 4);
        /* finally display pixels according to the bitmap */
        int x,y, line,mask;
        for(y=0;y<font->height;y++){
                /* save the starting position of the line */
                line=offs;
                mask=1<<(font->width-1);
                /* display a row */
                for(x=0;x<font->width;x++){
                        *((uint32_t*)(&fb + line)) = ((int)*glyph) & (mask) ? fg : bg;
                        /* adjust to the next pixel */
                        mask >>= 1;
                        line += 4;
                }
                /* adjust to the next line */
                glyph += bytesperline;
                offs  += scanline;
        }
}

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

size_t strlen(const char* str) {
        size_t len = 0;
        while (str[len])
                len++;
        return len;
}

void kernel_main(unsigned long magic, unsigned long addr) {
    struct multiboot_tag *tag;
    unsigned size;

    /* Am I booted by a Multiboot-compliant boot loader?  */
    // if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    //   printf("Invalid magic number");
    //   return;
    // }
    //
    // if (addr & 7) {
    //     printf("Unaligned mbi");
    //     return;
    // }

    size = *(unsigned *) addr;
    for (tag = (struct multiboot_tag *) (addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7)))
    {
        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            struct multiboot_tag_framebuffer *tagfb
	           = (struct multiboot_tag_framebuffer *) tag;
            fb = tagfb->common.framebuffer_addr;
            scanline = tagfb->common.framebuffer_width * tagfb->common.framebuffer_bpp;
        }
    }

    putchar(65, 0, 0, 0xFFFFFFFF, 0x00000000);  // Place an A in the first position
    putchar(66, 0, 1, 0xFFFFFFFF, 0x00000000);  // Place an B next
    putchar(67, 0, 2, 0xFFFFFFFF, 0x00000000);  // Place an C next
}
