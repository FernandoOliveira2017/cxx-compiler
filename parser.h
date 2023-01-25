#ifndef _parser_h_
#define _parser_h_

extern token kind;
extern const char * filename;
extern unsigned int line_number;

extern char get_next_char(void);
extern token lexan(void);

extern char * dup(void);
extern node * decl(node * block, table * o);

extern node * expr(node * block, table * o);

extern graph * create_graph(const char * file);
extern node * create_node(unsigned id);
extern void create_edge(node * u, node * v);
extern node * create_block(node * prev);
extern node * create_if(node * block, table * o);
extern node * parse_if_then(node * block, table * o);
extern node * create_goto(node * block);
extern node * create_return(node * block, table * o);
extern node * parse_branch(node * block, table * o);
extern void reuse_block(node * u, node * v);
extern void print_graph(graph * g);

extern table * create_table(table * prev);

extern void error(const char * format, ...);

#endif
