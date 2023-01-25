#ifndef _expr_h_
#define _expr_h_

extern token kind;

extern node * one;

extern token lexan(void);

extern bool next_token(token t);
extern void expect(unsigned char c);

extern type * btype(u_long specs);
extern bool void_checking(type * type);
extern bool is_void_btype(type * type);

extern u_long declspec(bool use_storage);
extern type * abstract(type * t);
extern void type_check(type * t, char * name);

extern node * create_node(unsigned id);
extern void create_edge(node * u, node * v);
extern bool insize(node * block);
extern node * create_value(void);
extern node * create_size(type * t);
extern node * create_store(node * block, node * lhs, u_long op, node * rhs);
extern node * un(u_long op, node * lhs);
extern node * bin(node * lhs, u_long op, node * rhs);
extern node * ter(node * lhs, node * cs, node * rhs);
extern node * ind(node * lhs, u_long op, node * rhs);
extern node * get_reference(node * o);

extern node * table_lookup(table * o);

extern void error(const char * format, ...);

#endif
