#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define COLOR_ERROR     "\x1b[1;31m"
#define COLOR_WARNING   "\x1b[1;35m"
#define COLOR_RESET     "\x1b[0m"

extern char * filename;
extern FILE * source;
extern unsigned int line_number;

void
error(const char * format, ...)
{
    va_list args;
    
    va_start(args, format);
    if (filename != NULL)
        printf("%s: %d: ", filename, line_number);
    printf(COLOR_ERROR "error: " COLOR_RESET);
    vprintf(format, args);
    printf("\n");
    va_end(args);
    if (source != NULL)
        fclose(source);
    exit(EXIT_FAILURE);
}

void
warning(const char * format, ...)
{
    va_list args;
    
    va_start(args, format);
	printf("%s: %d: ", filename, line_number);
    printf(COLOR_WARNING "warning: " COLOR_RESET);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}
