#ifndef _type_h_
#define _type_h_

extern token kind;

extern bool next_token(token t);

extern void * cc_alloc(size_t size);

extern void error(const char * format, ...);

#endif
