#ifndef _decl_h_
#define _decl_h_

extern token kind;
extern char identifier[];

extern token lexan(void);

extern bool next_token(token t);
extern void expect(unsigned char c);
extern node * compound(node * pred, table * o);

extern u_long dtype(u_long specs);
extern u_long mods(u_long specs);
extern u_long class(u_long specs);
extern u_long quals(u_long specs);
extern type * btype(u_long specs);
extern type * ptype(u_int kind, type * next);
extern bool void_checking(type * type);
extern bool is_void_btype(type * type);

extern void init(node * block, table * o, node * lhs);

extern node * const_expr(void);

extern node * create_symbol(char * name, type * t);

extern list * create_list(void);
extern void o_list_insert(list * o, node * s);
extern node * list_lookup(node * o, type * t);

extern table * create_table(table * prev);
extern void o_table_insert(table * o, node * s);
extern void to_from(list * src, table * dest);

extern void error(const char * format, ...);

#endif
