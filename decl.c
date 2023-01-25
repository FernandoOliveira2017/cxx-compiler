#include <stdbool.h>
#include <string.h>
#include "token.h"
#include "graph.h"
#include "types.h"
#include "ctype.h"
#include "symbol.h"
#include "decl.h"

static void parse_kr_arg(list * o);
static void parse_ansi_args(list * o);

u_long
declspec(bool use_storage)
{
	u_long specs;
	
	specs = 0x0L;
	do if (typespec(kind))
		specs = dtype(specs);
	else if (modifier(kind))
		specs = mods(specs);
	else if (storage(kind))
		if (use_storage == true)
			specs = class(specs);
		else
			error("storage class in declaration");
	else if (qualifier(kind))
		specs = quals(specs);
	while (declaration(kind));
	if (!(basic_type(specs)))
		specs |= cint;
	
	return specs;
}

char *
dup(void)
{
	char * name;
	
	name = strdup(identifier);
	kind = lexan();
	
	return name;
}

static type *
parse_pointer(type * t)
{
	type * p;
	
	p = ptype(pointer, t);
	while (qualifier(kind))
		p->qual = quals(p->qual);
	
	return p;
}

static type *
parse_array(type * t)
{
	type * p;
	
	p = ptype(array, t);
	p->lims = const_expr();
	expect(']');
	
	return p;
}

static type *
parse_function(type * t)
{
	type * p;
	
	p = ptype(function, t);
	if (next_token(')'))
		return p;
	p->args = create_list();
	if (declaration(kind))
		parse_ansi_args(p->args);
	else do
		parse_kr_arg(p->args);
	while (next_token(','));
	expect(')');
	
	return p;
}

static type *
pre_declarator(type * t)
{
	while (next_token('*'))
		t = parse_pointer(t);
	return t;
}

static type *
post_declarator(type * t)
{
	if (next_token('['))
		do t = parse_array(t);
		while (next_token('['));
	else if (next_token('('))
		t = parse_function(t);
	return t;
}

static bool
returning(type * t, type * prev, u_long c_kind)
{
	type * p;
	
	if (t == prev || t->kind != c_kind)
		return false;
	
	for (p = t; p != prev; p = p->next)
		if (p->kind == pointer)
			return false;
	
	return true;
}

static void
type_check(type * t, type * p, type * prev)
{
	if (t->kind == array && t != p) {
		if (void_checking(prev))
			error("array of voids");
		else if (returning(p, prev, function))
			error("function returning array");
	} else if (t->kind == function && t != p) {
		if (returning(p, prev, array))
			error("array of functions");
		else if (returning(p, prev, function))
			error("function returning function");
	} else if (void_checking(t))
		error("void declaration");
}

static void
declarator(symbol * s, type * prev)
{
	type * t;
	type * p;
	
	p = s->type;
	t = post_declarator(p);
	type_check(t, p, prev);
	s->type = t;
}

type *
abstract(type * t)
{
	type * p;
	type * o;
	
	p = pre_declarator(t);
	if (next_token('(')) {
		o = abstract(p);
		expect(')');
	} else
		o = p;
	t = post_declarator(o);
	type_check(t, o, p);
	
	return t;
}

static node *
old_kr(list * o, type * t)
{
	node * s;
	
	t = pre_declarator(t);
	if (next_token('(')) {
		s = old_kr(o, t);
		expect(')');
	} else if (kind == token_ident)
		s = list_lookup(o->head, t);
	else
		error("expected id or '('");
	declarator(s->data, t);
	
	return s;
}

static node *
direct(bool ansi, type * t)
{
	node * s;
	
	t = pre_declarator(t);
	if (next_token('(')) {
		s = direct(ansi, t);
		expect(')');
	} else if (kind == token_ident)
		s = create_symbol(dup(), t);
	else if (ansi == true)
		s = create_symbol(NULL, t);
	else
		error("expected id or '('");
	declarator(s->data, t);
	
	return s;
}

static void
parse_kr_arg(list * o)
{
	node * s;
	
	if (kind != token_ident)
		error("expected id");
	s = create_symbol(dup(), NULL);
	o_list_insert(o, s);
}

static type *
next_arg(list * o)
{
	type * t;
	
	t = NULL;
	if (next_token(','))
		if (next_token(token_etc))
			o_list_insert(o, o->head);
		else if (declaration(kind))
			t = btype(declspec(false));
		else
			error("expected typename");
	
	return t;
}

static void
parse_ansi_args(list * o)
{
	node * s;
	type * t;
	
	t = btype(declspec(false));
	if (is_void_btype(t))
		return;
	else do {
		s = direct(true, t);
		o_list_insert(o, s);
	} while (t = next_arg(o));
}

static void
kr_args(list * o)
{
	node * s;
	type * t;
	
	t = btype(declspec(false));
	do s = old_kr(o, t);
	while (next_token(','));
	expect(';');
}

static bool
is_kr_null_type(symbol * s)
{
	return s->type == NULL;
}

static bool
is_kr(list * o)
{
	if (o != NULL)
		if (o->head != NULL)
			if (is_kr_null_type(o->head->data))
				return true;
	return false;
}

static bool
is_function(table * o, symbol * s)
{
	if (s->type->kind != function)
		return false;
	else if (declaration(kind)) {
		if (!is_kr(s->type->args))
			error("old kr prototype needed");
		do kr_args(s->type->args);
		while (declaration(kind));
		expect('{');
		return true;
	} else if (next_token('{'))
		return true;
	else
		return false;
}

static void
realign(table * o, symbol * s)
{
	to_from(s->type->args, o);
	if (is_kr(s->type->args))
		s->type->args = NULL;
}

static void
verify_kr(symbol * s)
{
	if (s->type->kind == function)
		if (is_kr(s->type->args))
			s->type->args = NULL;
}

static node *
next(table * o, type * t)
{
	node * s;
	
	if (!next_token(',')) {
		expect(';');
		return NULL;
	}
	s = direct(false, t);
	o_table_insert(o, s);
	
	return s;
}

node *
decl(node * block, table * o)
{
	node * s;
	type * t;
	
	if (o->scope == 0x0)
		t = btype(declspec(false));
	else
		t = btype(declspec(true));
	s = direct(false, t);
	o_table_insert(o, s);
	if (is_function(o, s->data)) {
		o = create_table(o);
		realign(o, s->data);
		block = compound(block, o);
	} else do if (next_token('='))
		init(block, o, s);
	else
		verify_kr(s->data);
	while (s = next(o, t));
	
	return block;
}
