#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "graph.h"
#include "ssaop.h"
#include "types.h"
#include "ctype.h"
#include "symbol.h"
#include "expr.h"

#define not_lvalue(a)				a != deref && a != subscript && a != pointer && a != acess && a != csymbol
#define lvalue_expected_in_op(a)	a == store || a == add || a == sub || a == div || a == mod || a == left_shift ||	\
									a == right_shift || a == bitand || a == xor || a == bitor

static node * unary(node * block, table * o, bool in_sz);
static node * assign(node * block, table * o);
node * expr(node * block, table * o);

static void
lvalue_expected(u_long op)
{
	if (lvalue_expected_in_op(op))
		error("lvalue expected");
}

type *
typename(void)
{
	type * t;

	t = btype(declspec(false));
	if (is_void_btype(t))
		return NULL;
	else
		t = abstract(t);

	return t;
}

static node *
cast(node * block, table * o,
				  bool in_sz)
{
	type * t;
	node * rhs;
	
	t = typename();
	expect(')');
	if (in_sz == true)
		return create_size(t);
	else {
		rhs = unary(block, o, false);
		rhs->data = t;
	}
	
	return rhs;
}

static node *
open(node * block, table * o,
				  bool in_sz)
{
	node * rhs;
	
	if (declaration(kind))
		return cast(block, o, in_sz);
	else
		rhs = expr(block, o);
	expect(')');
	
	return rhs;
}

static node *
primary(node * block, table * o,
					 bool in_sz)
{
	node * lhs;
	
	if (kind == token_ident)
		lhs = table_lookup(o);
	else if (next_token('('))
		return open(block, o, in_sz);
	else if (primary_expr(kind))
		lhs = create_value();
	else
		error("expr expected");
	kind = lexan();
	
	return lhs;
}

static node *
brack(node * block, table * o)
{
	node * rhs;
	
	rhs = expr(block, o);
	expect(']');
	
	return rhs;
}

static node *
call(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	if (next_token(')'))
		return NULL;
	lhs = create_node(cargs);
	do {
		rhs = assign(block, o);
		create_edge(lhs, rhs);
	} while (next_token(','));
	expect(')');
	
	return lhs;
}

static node *
reference(node * lhs)
{
	node * rhs;
	
	if (kind != token_ident)
		error("expected id");
	rhs = get_reference(lhs);
	
	return rhs;
}

static node *
infix(node * block, table * o, node * lhs,
								u_long op)
{
	node * rhs;
	
	if (op == add || op == sub)
		rhs = bin(lhs, op, one);
	else if (op == subscript)
		rhs = brack(block, o);
	else if (op == ccall)
		rhs = call(block, o);
	else if (op == pointer || op == acess)
		rhs = reference(lhs);
	
	return rhs;
}

static node *
postfix(node * block, table * o,
					 bool in_sz)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = primary(block, o, in_sz);
	while (post_expr(kind)) {
		if (next_token(token_inc))
			op = add;
		else if (next_token(token_dec))
			op = sub;
		else if (next_token('('))
			op = ccall;
		else if (next_token('['))
			op = subscript;
		else if (next_token(token_pointer))
			op = pointer;
		else if (next_token('.'))
			op = acess;
		if (not_lvalue(lhs->id))
			lvalue_expected(op);
		rhs = infix(block, o, lhs, op);
		if (op == add || op == sub)
			op = store;
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
unary(node * block, table * o,
				   bool in_sz)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	if (next_token(token_inc))
		op = add;
	else if (next_token(token_dec))
		op = sub;
	else if (next_token('+'))
		return unary(block, o, in_sz);
	else if (next_token('-'))
		op = neg;
	else if (next_token('!'))
		op = not;
	else if (next_token('~'))
		op = bitnot;
	else if (next_token('*'))
		op = deref;
	else if (next_token('&'))
		op = addressof;
	else if (next_token(token_sizeof)) {
		op = csizeof;
		in_sz = true;
	} else
		return postfix(block, o, in_sz);
	lhs = unary(block, o, in_sz);
	if (not_lvalue(lhs->id))
		lvalue_expected(op);
	if (op == add || op == sub)
		lhs = ind(lhs, op, one);
	else
		lhs = un(op, lhs);
	create_edge(block, lhs);
	
	return lhs;
}

static node *
prod(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = unary(block, o, false);
	while (prod_expr(kind)) {
		if (next_token('*'))
			op = mul;
		else if (next_token('/'))
			op = div;
		else if (next_token('%'))
			op = mod;
		rhs = unary(block, o, false);
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
sum(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = prod(block, o);
	while (sum_expr(kind)) {
		if (next_token('+'))
			op = add;
		else if (next_token('-'))
			op = sub;
		rhs = prod(block, o);
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
shift(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = sum(block, o);
	while (shift_expr(kind)) {
		if (next_token(token_left_shift))
			op = left_shift;
		else if (next_token(token_right_shift))
			op = right_shift;
		rhs = sum(block, o);
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
rel(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = shift(block, o);
	while (rel_expr(kind)) {
		if (next_token('<'))
			op = less;
		else if (next_token(token_less_equal))
			op = less_equal;
		else if (next_token('>'))
			op = greater;
		else if (next_token(token_greater_equal))
			op = greater_equal;
		rhs = shift(block, o);
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
rel_eq(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = rel(block, o);
	while (rel_eq_expr(kind)) {
		if (next_token(token_equal))
			op = equal;
		else if (next_token(token_not_equal))
			op = not_equal;
		rhs = rel(block, o);
		lhs = bin(lhs, op, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
bit_and(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	lhs = rel_eq(block, o);
	while (next_token('&')) {
		rhs = rel_eq(block, o);
		lhs = bin(lhs, bitand, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
bit_xor(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	lhs = bit_and(block, o);
	while (next_token('^')) {
		rhs = bit_and(block, o);
		lhs = bin(lhs, xor, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
bit_or(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	lhs = bit_xor(block, o);
	while (next_token('|')) {
		rhs = bit_xor(block, o);
		lhs = bin(lhs, bitor, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
log_and(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	lhs = bit_or(block, o);
	while (next_token(token_and)) {
		rhs = bit_or(block, o);
		lhs = bin(lhs, and, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
log_or(node * block, table * o)
{
	node * lhs;
	node * rhs;
	
	lhs = log_and(block, o);
	while (next_token(token_or)) {
		rhs = log_and(block, o);
		lhs = bin(lhs, or, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
cond(node * block, table * o)
{
	node * lhs;
	node * cs;
	node * rhs;
	
	lhs = log_or(block, o);
	if (next_token('?')) {
		cs = expr(block, o);
		expect(':');
		rhs = cond(block, o);
		lhs = ter(lhs, cs, rhs);
		create_edge(block, lhs);
	}
	
	return lhs;
}

static node *
assign(node * block, table * o)
{
	node * lhs;
	node * rhs;
	u_long op;
	
	lhs = cond(block, o);
	if (assign_expr(kind)) {
		if (next_token('='))
			op = store;
		else if (next_token(token_add_assign))
			op = add;
		else if (next_token(token_sub_assign))
			op = sub;
		else if (next_token(token_mul_assign))
			op = mul;
		else if (next_token(token_div_assign))
			op = div;
		else if (next_token(token_mod_assign))
			op = mod;
		else if (next_token(token_left_shift_assign))
			op = left_shift;
		else if (next_token(token_right_shift_assign))
			op = right_shift;
		else if (next_token(token_bit_and_assign))
			op = bitand;
		else if (next_token(token_xor_assign))
			op = xor;
		else if (next_token(token_bit_or_assign))
			op = bitor;
		if (not_lvalue(lhs->id))
			lvalue_expected(op);
		rhs = assign(block, o);
		lhs = create_store(block, lhs, op, rhs);
	}
	
	return lhs;
}

node *
expr(node * block, table * o)
{
	node * lhs;
	
	do lhs = assign(block, o);
	while (next_token(','));
	
	return lhs;
}
