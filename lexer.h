#ifndef _lexer_h_
#define _lexer_h_

typedef struct
{
    token kind;
    
    char * word;
} keyword;

extern void * cc_alloc(size_t size);
extern void * cc_realloc(void * pointer, size_t size);
extern void error(const char * format, ...);

#endif
