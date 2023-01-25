#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "ctype.h"
#include "ssaop.h"
#include "graph.h"
#include "symbol.h"

token kind;

extern const char * filename;
extern unsigned int line_number;
unsigned int block_index = 0;
unsigned int var_index = 0;
extern node * create_node(unsigned id);

node * one;

extern void * cc_alloc(size_t size);

void init(node * block, table * o, node * lhs){}

node * const_expr(void){}

node * create_symbol(char * name, type * t){
	symbol * s;
	node * sym;
	
	s = cc_alloc(sizeof(symbol));
	sym = cc_alloc(sizeof(node));
	s->name = name;
	s->type = t;
	sym->id = 0x03;
	sym->data = s;
	
	return sym;
}

void type_check(type * t, char * name)
{
}

node * get_reference(node * lhs)
{
}

void print_type(type* t) {
	if (t != NULL) {
		print_type(t->next);
		if (t->kind == ctype) {
			if (t->type == c_const)
				printf("const ");
			else if (t->type == c_volatile)
				printf("volatile ");
			else if (t->type == c_void)
				printf("void ");
			else if (t->type == c_char)
				printf("char ");
			else if (t->type == c_unsigned_char)
				printf("unsigned char ");
			else if (t->type == c_short_int)
				printf("short int ");
			else if (t->type == c_unsigned_short_int)
				printf("unsigned short int ");
			else if (t->type == c_int)
				printf("int ");
			else if (t->type == c_unsigned_int)
				printf("unsigned int ");
			else if (t->type == c_long_int)
				printf("long int ");
			else if (t->type == c_unsigned_long_int)
				printf("unsigned long int ");
			else if (t->type == c_long_long_int)
				printf("long long int ");
			else if (t->type == c_unsigned_long_long_int)
				printf("unsigned long long int ");
			else if (t->type == c_float)
				printf("float ");
			else if (t->type == c_double)
				printf("double ");
			else if (t->type == c_long_double)
				printf("long double ");
		} else if (t->kind == pointer) {
			printf("* ");
			if (t->qual & cconst)
				printf("const ");
			if (t->qual & cvolatile)
				printf("volatile ");
		} else if (t->kind == array)
			printf("[] ");
	}
}
