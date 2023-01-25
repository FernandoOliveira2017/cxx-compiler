#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "graph.h"
#include "types.h"
#include "ctype.h"
#include "symbol.h"
#include "type.h"

type basic_type[] = {
	{.kind = ctype, .type = c_void},
	{.kind = ctype, .type = c_char},
	{.kind = ctype, .type = c_unsigned_char},
	{.kind = ctype, .type = c_short_int},
	{.kind = ctype, .type = c_unsigned_short_int},
	{.kind = ctype, .type = c_int},
	{.kind = ctype, .type = c_unsigned_int},
	{.kind = ctype, .type = c_long_int},
	{.kind = ctype, .type = c_unsigned_long_int},
	{.kind = ctype, .type = c_long_long_int},
	{.kind = ctype, .type = c_unsigned_long_long_int},
	{.kind = ctype, .type = c_float},
	{.kind = ctype, .type = c_double},
	{.kind = ctype, .type = c_long_double}
};

#define get_basic_type(a)	(&(basic_type[a]))

static void
cannot_be_used(u_long specs_1, char * typename_1,
			   u_long specs_2, char * typename_2)
{
	if (specs_1 & specs_2)
		error("'%s' cannot be used with '%s'", typename_1, typename_2);
}

static void
cannot_be_used_1(u_long specs, char * typename)
{
	cannot_be_used(specs, typename, cvoid, "void");
	cannot_be_used(specs, typename, cfloat, "float");
	cannot_be_used(specs, typename, cdouble, "double");
}

static void
cannot_be_used_2(u_long specs, char * typename)
{
	cannot_be_used(specs, typename, cchar, "char");
	cannot_be_used(specs, typename, cint, "int");
}

static void
cannot_be_used_3(u_long specs, char * typename)
{
	cannot_be_used(specs, typename, cshort, "short");
	cannot_be_used(specs, typename, clong, "long");
	cannot_be_used(specs, typename, clonglong, "long long");
}

static void
cannot_be_used_4(u_long specs, char * typename)
{
	cannot_be_used(specs, typename, csigned, "signed");
	cannot_be_used(specs, typename, cunsigned, "unsigned");
}

static void
cannot_be_used_5(u_long specs, char * typename)
{
	cannot_be_used(specs, typename, cstatic, "static");
	cannot_be_used(specs, typename, cextern, "extern");
	cannot_be_used(specs, typename, cauto, "auto");
	cannot_be_used(specs, typename, cregister, "register");
}

u_long
dtype(u_long specs)
{
	if (next_token(token_void)) {
		cannot_be_used_1(specs, "void");
		cannot_be_used_2(specs, "void");
		cannot_be_used_3(specs, "void");
		cannot_be_used_4(specs, "void");
		return specs | cvoid;
	} else if (next_token(token_char)) {
		cannot_be_used_1(specs, "char");
		cannot_be_used_2(specs, "char");
		cannot_be_used_3(specs, "char");
		return specs | cchar;
	} else if (next_token(token_int)) {
		cannot_be_used_1(specs, "int");
		cannot_be_used_2(specs, "int");
		return specs | cint;
	} else if (next_token(token_float)) {
		cannot_be_used_1(specs, "float");
		cannot_be_used_2(specs, "float");
		cannot_be_used_3(specs, "float");
		cannot_be_used_4(specs, "float");
		return specs | cfloat;
	} else if (next_token(token_double)) {
		cannot_be_used_1(specs, "double");
		cannot_be_used_2(specs, "double");
		cannot_be_used(specs, "double", cshort, "short");
		cannot_be_used(specs, "double", clonglong, "long long");
		cannot_be_used_4(specs, "double");
		return specs | cdouble;
	}
}

u_long
mods(u_long specs)
{
	if (next_token(token_short)) {
		cannot_be_used_1(specs, "short");
		cannot_be_used(specs, "short", cchar, "char");
		cannot_be_used_3(specs, "short");
		return specs | cshort;
	} else if (next_token(token_long)) {
		cannot_be_used(specs, "long", cvoid, "void");
		cannot_be_used(specs, "long", cchar, "char");
		cannot_be_used(specs, "long", cfloat, "float");
		cannot_be_used(specs, "long", cshort, "short");
		cannot_be_used(specs, "long", clonglong, "long long");
		return specs & clong ? specs & ~clong |
					clonglong : specs | clong;
	} else if (next_token(token_signed)) {
		cannot_be_used_1(specs, "signed");
		cannot_be_used_4(specs, "signed");
		return specs | csigned;
	} else if (next_token(token_unsigned)) {
		cannot_be_used_1(specs, "unsigned");
		cannot_be_used_4(specs, "unsigned");
		return specs | cunsigned;
	}
}

u_long
class(u_long specs)
{
	if (next_token(token_static)) {
		cannot_be_used_5(specs, "static");
		return specs | cstatic;
	} else if (next_token(token_extern)) {
		cannot_be_used_5(specs, "extern");
		return specs | cextern;
	} else if (next_token(token_auto)) {
		cannot_be_used_5(specs, "auto");
		return specs | cauto;
	} else if (next_token(token_register)) {
		cannot_be_used_5(specs, "register");
		return specs | cregister;
	}
}

u_long
quals(u_long specs)
{
	if (next_token(token_const))
		return specs | cconst;
	else if (next_token(token_volatile))
		return specs | cvolatile;
}

static type *
get_b_type(u_long specs)
{
	if (specs & cvoid)
		return get_basic_type(c_void);
	else if (specs & cchar)
		if (specs & cunsigned)
			return get_basic_type(c_unsigned_char);
		else
			return get_basic_type(c_char);
	else if (specs & cint)
		if (specs & cunsigned)
			if (specs & clonglong)
				return get_basic_type(c_unsigned_long_long_int);
			else if (specs & clong)
				return get_basic_type(c_unsigned_long_int);
			else if (specs & cshort)
				return get_basic_type(c_unsigned_short_int);
			else
				return get_basic_type(c_unsigned_int);
		else if (specs & clonglong)
			return get_basic_type(c_long_long_int);
		else if (specs & clong)
			return get_basic_type(c_long_int);
		else if (specs & cshort)
			return get_basic_type(c_short_int);
		else
			return get_basic_type(c_int);
	else if (specs & cdouble)
		if (specs & clong)
			return get_basic_type(c_long_double);
		else
			return get_basic_type(c_double);
	else if (specs & cfloat)
		return get_basic_type(cfloat);
}

static type *
c_type(type * next, u_long ct)
{
	type * t;
	
	t = cc_alloc(sizeof(type));
	t->kind = ctype;
	t->type = ct;
	t->next = next;
	
	return t;
}

type *
btype(u_long specs)
{
	type * t;
	
	t = get_b_type(specs);
	if (specs & cstatic)
		t = c_type(t, c_static);
	else if (specs & cextern)
		t = c_type(t, c_extern);
	else if (specs & c_register)
		t = c_type(t, c_register);
	if (specs & cvolatile)
		t = c_type(t, c_volatile);
	if (specs & cconst)
		t = c_type(t, c_const);
	
	return t;
}

type *
ptype(u_int kind, type * next)
{
	type * t;
	
	t = cc_alloc(sizeof(type));
	t->kind = kind;
	t->next = next;
	
	return t;
}

bool
void_checking(type * t)
{
	if (t->kind == ctype)
		if (t->type == c_void)
			return true;
	return false;
}

bool
is_void_btype(type * t)
{
	if (void_checking(t))
		if (kind == ')')
			return true;
	return false;
}

static int
conv_rank(type * t)
{
	switch (t->type) {
		case c_char:
		case c_unsigned_char:
			return 1;
		case c_short_int:
		case c_unsigned_short_int:
			return 2;
		case c_int:
		case c_unsigned_int:
			return 3;
		case c_long_int:
		case c_unsigned_long_int:
			return 4;
		case c_long_long_int:
		case c_unsigned_long_long_int:
			return 5;
		case c_float:
			return 6;
		case c_double:
			return 7;
		case c_long_double:
			return 8;
	}
}

static bool
value_preserving(type * a, type * b)
{
	switch (a->type) {
		case c_char:
			if (b->type == c_short_int ||
				b->type == c_int)
				return true;
		case c_unsigned_char:
			if (b->type == c_short_int ||
				b->type == c_unsigned_short_int ||
				b->type == c_int ||
				b->type == c_unsigned_int)
				return true;
		case c_unsigned_short_int:
			if (b->type == c_int ||
				b->type == c_unsigned_int)
				return true;
		default:
			return false;
	}
}

static bool
is_signed(type * t)
{
	switch (t->type) {
		case c_char:
		case c_short_int:
		case c_int:
		case c_long_int:
		case c_long_long_int:
			return true;
		default:
			return false;
	}
}

static bool
is_unsigned(type * t)
{
	switch (t->type) {
		case c_unsigned_char:
		case c_unsigned_short_int:
		case c_unsigned_int:
		case c_unsigned_long_int:
		case c_unsigned_long_long_int:
			return true;
		default:
			return false;
	}
}

static bool
is_narrowing(type * a, type * b)
{
	if (conv_rank(a) < conv_rank(b))
		return true;
	else
		return false;
}

type *
promote(type * a, type * b)
{
	if (is_narrowing(a, b))
		return b;
	else
		return a;
}

static type *
conv_unsigned(type * t)
{
	switch (t->type) {
		case c_char:
			return get_basic_type(c_unsigned_char);
		case c_short_int:
			return get_basic_type(c_unsigned_short_int);
		case c_int:
			return get_basic_type(c_unsigned_int);
		case c_long_int:
			return get_basic_type(c_unsigned_long_int);
		case c_long_long_int:
			return get_basic_type(c_unsigned_long_long_int);
	}
}

static type *
convert(type * a, type * b)
{
	if (is_narrowing(b, a))
		if (value_preserving(b, a))
			return a;
		else
			return conv_unsigned(a);
	else
		return b;
}

type *
convert_type(type * a, type * b)
{
	if (is_fp(a->type) || is_fp(b->type))
		return promote(a, b);
	else {
		a = promote(a, get_basic_type(c_int));
		b = promote(b, get_basic_type(c_int));

		if (is_signed(a) && is_signed(b))
			return promote(b, a);
		else if (is_unsigned(a) && is_unsigned(b))
			return promote(b, a);
		else if (is_signed(a) && is_unsigned(b))
			return convert(a, b);
		else if (is_signed(b) && is_unsigned(a))
			return convert(b, a);
		else
			return promote(a, b);
	}
}
