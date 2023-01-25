#ifndef _hash_h_
#define _hash_h_

extern char identifier[];

extern list * create_list(void);
extern void h_list_insert(list * o, node * s);
extern node * h_list_lookup(list * o);

extern void * cc_alloc(size_t size);

extern void error(const char * format, ...);

#endif
