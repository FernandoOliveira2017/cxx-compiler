#ifndef _ssa_h_
#define _ssa_h_

extern FILE * source;
extern unsigned int block_index;
extern unsigned int var_index;

extern void expect(unsigned char c);

extern node * expr(node * block, table * o);

extern void * cc_alloc(size_t size);

extern void error(const char * format, ...);

#endif
