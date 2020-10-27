#include <string.h>
#include <stdio.h>
#include <kernel/tty.h>

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
