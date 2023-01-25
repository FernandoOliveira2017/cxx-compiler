#include <string.h>
#include "graph.h"
#include "types.h"
#include "symbol.h"
#include "hash.h"

#define HASH 2048

table *
create_table(table * prev)
{
	table * o;
	
	o = cc_alloc(sizeof(table));
	if (prev != NULL)
		o->scope = prev->scope+1;
	o->hash = cc_alloc(sizeof(list * [HASH]));
	for (int i = 0; i < HASH; i++)
		o->hash[i] = create_list();
	o->prev = prev;
	
	return o;
}

static u_long
hash(char * name)
{
	unsigned length;
	u_long value = 0;
	
	length = strlen(name);
	for (int i = 0; i < length; i++) {
		value <<= 8;
		value += name[i];
	}
	
	return value % HASH;
}

static char *
get_name(symbol * s)
{
	return s->name;
}

static void
parameter_name(symbol * s)
{
	if (s->name == NULL)
		error("parameter name omitted");
}

void
o_table_insert(table * o, node * s)
{
	u_long value;
	
	value = hash(get_name(s->data));
	h_list_insert(o->hash[value], s);
}

node *
o_table(table * o, u_long value)
{
	node * s;
	
	if (o != NULL) {
		s = h_list_lookup(o->hash[value]);
		if (s != NULL)
			return s;
		else
			return o_table(o->prev, value);
	} else
		error("%s undeclared", identifier);
}

node *
table_lookup(table * o)
{
	u_long value;
	
	value = hash(identifier);
	return o_table(o, value);
}

void
to_from(list * src, table * dest)
{
	if (src != NULL)
		for (node * i = src->head;
			 i != NULL; i = i->next) {
			parameter_name(i->data);
			o_table_insert(dest, i);
			if (i->next == src->head)
				break;
		}
}
