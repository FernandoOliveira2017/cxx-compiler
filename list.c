#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token.h"
#include "graph.h"
#include "symbol.h"
#include "list.h"

list *
create_list(void)
{
	list * o;
	
	o = cc_alloc(sizeof(list));
	
	return o;
}

static void
redeclared(symbol * a, symbol * b)
{
	if (!strcmp(a->name, b->name))
		error("'%s' redeclared", b->name);
}

static bool
declared(symbol * s, type * t)
{
	if (!strcmp(s->name, identifier)) {
		kind = lexan();
		s->type = t;
		return true;
	} else
		return false;
}

static bool
declared_no_type(symbol * s)
{
	if (!strcmp(s->name, identifier))
		return true;
	else
		return false;
}

static void
o_list_insert_1(node * o, node * s)
{
	if (o->next == NULL)
		o->next = s;
	else
		o_list_insert_1(o->next, s);
}

static void
o_list_insert_2(node * o, node * s)
{
	redeclared(o->data, s->data);
	if (o->next == NULL)
		o->next = s;
	else
		o_list_insert_2(o->next, s);
}

void
o_list_insert(list * o, node * s)
{
	if (o->head == NULL)
		o->head = s;
	else if (s == o->head)
		o_list_insert_1(o->head, s);
	else
		o_list_insert_2(o->head, s);
}

static void
h_list_insert_1(node * o, node * s)
{
	redeclared(o->data, s->data);
	if (o->prev == NULL)
		o->prev = s;
	else
		h_list_insert_1(o->prev, s);
}

void
h_list_insert(list * o, node * s)
{
	if (o->head == NULL)
		o->head = s;
	else
		h_list_insert_1(o->head, s);
}

node *
list_lookup(node * o, type * t)
{
	if (o != NULL) {
		if (declared(o->data, t))
			return o;
		return list_lookup(o->next, t);
	} else
		error("%s undeclared", identifier);
}

static node *
h_list_lookup_1(node * o)
{
	if (declared_no_type(o->data))
		return o;
	if (o->prev == NULL)
		return NULL;
	return h_list_lookup_1(o->prev);
}

node *
h_list_lookup(list * o)
{
	if (o->head != NULL)
		return h_list_lookup_1(o->head);
	else
		return NULL;
}
