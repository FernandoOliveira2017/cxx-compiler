#ifndef _parser_h_
#define _parser_h_

extern char identifier[];
extern token kind;

extern token lexan(void);

extern void * cc_alloc(size_t size);

extern void error(const char * format, ...);

#endif
