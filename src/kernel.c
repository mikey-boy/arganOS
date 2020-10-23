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

/* Hardware text mode color constants. */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void strcpy(const char * src, char * dest) {
    dest[0] = src[0];
    for (int i = 1; src[i-1]; i++)
        dest[i] = src[i];
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void cls();

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    cls();
}

void cls()
{
    terminal_row = 0;
    terminal_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    if (c == '\n') { return; }
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH || c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

/* ntos - Convert interger to string representation of any base (up to 16)
 *
 * buf - array to store the result
 * num - number to convet
 * base - desired number base
 */
void ntos(char * buf, int num, int base) {
    char conv[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                    'A', 'B', 'C', 'D', 'E', 'F'};

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return;
    }

    /* Divide by div until num == 0 */
    int i = 0;
    while (num > 0){
        buf[i] = conv[num % base];
        num = num / base;
        i++;
    }

    buf[i] = 0;
    i--;

    /* Reverse the buffer */
    int j = 0;
    while (j<i) {
        char c = buf[j];
        buf[j] = buf[i];
        buf[i] = c;
        j++;
        i--;
    }
}

/* printf - Print a format string. Supports %x, %d, %o, %b, and %s.
 *
 * format - the format string
 * ... - values corresponding to format string. Args need to be on the stack.
 */
void printf(const char *format, ...)
{
    char c;
    char buf[20];

    char ** arg = (char **) &format;
    arg++;

    while ((c = *format++) != 0) {
        if (c != '%') {
            terminal_putchar(c);
            continue;
        }

        /* Handle format character */
        c = *format++;

        if (c != 'x' && c != 'd' && c != 'o' && c != 'b' && c != 's')
            continue;
        switch (c) {
            case 'x':
                ntos(buf, *((int *) arg) , 16);
                break;
            case 'd':
                ntos(buf, *((int *) arg) , 10);
                break;
            case 'o':
                ntos(buf, *((int *) arg) , 8);
                break;
            case 'b':
                ntos(buf, *((int *) arg) , 2);
                break;
            case 's':
                strcpy(*arg, buf);
                break;
        }
        terminal_writestring(buf);
        arg++;
    }
}

void kernel_main(unsigned long magic, unsigned long addr)
{
    /* Am I booted by a multiboot2 compliant bootloader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%x\n", magic);
        return;
    }

    /* Initialize terminal interface */
    terminal_initialize();

    /* Newline support is left as an exercise. */
    printf("Hello kernel world!\n");
    cls();
    printf("Hello mikey G!\n");
}
