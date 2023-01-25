#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "ssaop.h"
#include "symbol.h"
#include "ssa.h"

void print(node * o);

node * create_block(node * prev);
node * ind(node * lhs, u_long op, node * rhs);

graph *
create_graph(const char * file)
{
	graph * g;
	
	source = fopen(file, "r");
	if (source == NULL)
		error("%s: no such file", file);
	g = cc_alloc(sizeof(graph));
	g->entry = create_block(NULL);
	g->exit = create_block(g->entry);
	
	return g;
}

node *
create_node(unsigned id)
{
	node * succ;
	
	succ = cc_alloc(sizeof(node));
	succ->id = id;
	succ->i = id == cblock ? block_index++
			: is_op(id) ? var_index++ : 0;
	
	return succ;
}

void
create_edge(node * u, node * v)
{
	edge * e;
	
	if (u == NULL || v == NULL)
		return;
	e = cc_alloc(sizeof(edge));
	e->u = u;
	e->v = v;
	e->pred_next = v->pred;
	e->succ_next = u->succ;
	u->succ = v->pred = e;
}

node *
create_value(void)
{
	node * succ;
	
	succ = create_node(value);
	
	return succ;
}

node *
create_size(type * t)
{
	node * succ;
	
	succ = create_node(csizeof);
	succ->data = t;
	
	return succ;
}

void
o_edge(node * block, node * succ, node * rhs)
{
	if (block->succ->v == rhs)
		block->succ->v = succ;
	else
		create_edge(block, succ);
}

node *
create_store(node * block, node * lhs, u_long op, node * rhs)
{
	node * succ;
	
	if (op != store)
		succ = ind(lhs, op, rhs);
	else {
		succ = create_node(op);
		create_edge(succ, lhs);
		create_edge(succ, rhs);
	}
	if (block->succ != NULL)
		o_edge(block, succ, rhs);
	else
		create_edge(block, succ);
	
	return succ;
}

node *
create_block(node * prev)
{
	node * block;
	
	block = create_node(cblock);
	if (prev != NULL)
		prev->next = block;
	
	return block;
}

node *
create_if(node * block, table * o)
{
	node * succ[3];
	
	succ[0] = create_node(if_then);
	succ[1] = create_node(not);
	succ[2] = expr(block, o);
	create_edge(block, succ[0]);
	create_edge(succ[0], succ[1]);
	create_edge(succ[1], succ[2]);
	
	return succ[0];
}

node *
create_goto(node * block)
{
	node * succ;
	
	succ = create_node(cgoto);
	create_edge(block, succ);
	
	return succ;
}

node *
create_return(node * block, table * o)
{
	node * succ[2];
	
	succ[0] = create_node(creturn);
	succ[1] = expr(block, o);
	create_edge(block, succ[0]);
	create_edge(succ[0], succ[1]);
	
	return succ[0];
}

node *
un(u_long op, node * lhs)
{
	node * succ;
	
	succ = create_node(op);
	create_edge(succ, lhs);
	
	return succ;
}

node *
bin(node * lhs, u_long op, node * rhs)
{
	node * succ;
	
	succ = create_node(op);
	create_edge(succ, lhs);
	create_edge(succ, rhs);
	
	return succ;
}

node *
ter(node * lhs, node * cs, node * rhs)
{
	node * succ;
	
	succ = create_node(ccond);
	create_edge(succ, lhs);
	create_edge(succ, cs);
	create_edge(succ, rhs);
	
	return succ;
}

node *
ind(node * lhs, u_long op, node * rhs)
{
	node * succ[2];
	
	succ[0] = create_node(store);
	succ[1] = create_node(op);
	create_edge(succ[0], lhs);
	create_edge(succ[0], succ[1]);
	create_edge(succ[1], lhs);
	create_edge(succ[1], rhs);
	
	return succ[0];
}

node *
parse_branch(node * block, table * o)
{
	node * succ;
	
	expect('(');
	succ = create_if(block, o);
	expect(')');
	
	return succ;
}

node *
parse_if_then(node * block, table * o)
{
	node * succ[2];
	
	expect('(');
	succ[0] = create_node(if_then);
	succ[1] = expr(block, o);
	create_edge(block, succ[0]);
	create_edge(succ[0], succ[1]);
	expect(')');
	
	return succ[0];
}

void
reuse_block(node * u, node * v)
{
	if (u->succ != NULL)
		return;
	v->prev = u->prev;
	//v->prev->next = v;
	/* copy and merge edges from u to v */
	v->pred = u->pred;
}

static char *
get_op(u_long id)
{
	switch (id) {
		case pointer:
			return "->";
		case acess:
			return ".";
		case neg:
			return "-";
		case not:
			return "!";
		case bitnot:
			return "~";
		case deref:
			return "*";
		case addressof:
			return "&";
		case mul:
			return " * ";
		case div:
			return " / ";
		case mod:
			return " % ";
		case add:
			return " + ";
		case sub:
			return " - ";
		case left_shift:
			return " << ";
		case right_shift:
			return " >> ";
		case less:
			return " < ";
		case less_equal:
			return " <= ";
		case greater:
			return " > ";
		case greater_equal:
			return " >= ";
		case equal:
			return " == ";
		case not_equal:
			return " != ";
		case bitand:
			return " & ";
		case xor:
			return " ^ ";
		case bitor:
			return " | ";
		case and:
			return " && ";
		case or:
			return " || ";
	}
}

void
print_args(edge * e)
{
	if (e->succ_next != NULL)
		print_args(e->succ_next);
	print(e->v);
	printf(", ");
}

void
print_sym(symbol * s)
{
	printf("%s", s->name);
}

void
print(node * o)
{
	if (o->id == csymbol)
		print_sym(o->data);
	else if (is_op(o->id))
		printf("t%d", o->i);
	else if (o->id == value)
		printf("%lf", (* (double *) (o->data)));
	else if (o->id == cargs)
		print_args(o->succ);
	else
		printf("?");
}

void
print_ssa(node * o)
{
	if (is_un_op(o->id)) {
		printf("%s", get_op(o->id));
		print(o->succ->v);
	} else if (is_bin_op(o->id)) {
		print(o->succ->succ_next->v);
		printf("%s", get_op(o->id));
		print(o->succ->v);
	} else if (o->id == ccall) {
		print(o->succ->succ_next->v);
		printf("(");
		print(o->succ->v);
		printf(")");
	} else if (o->id == subscript) {
		print(o->succ->succ_next->v);
		printf("[");
		print(o->succ->v);
		printf("]");
	} else if (o->id == ccond) {
		print(o->succ->succ_next->succ_next->v);
		printf(" ? ");
		print(o->succ->succ_next->v);
		printf(" : ");
		print(o->succ->v);
	} else
		print(o);
}

void
print_statement(node * o)
{
	if (o->id == if_then) {
		printf("\tif (");
		print_ssa(o->succ->succ_next->v);
		printf(") then");
		printf("\n\t\tgoto L%d;\n",
			   o->succ->v->i);
	} else if (o->id == cgoto)
		printf("\tgoto L%d;\n",
			   o->succ->v->i);
	else if (o->id == creturn) {
		printf("\treturn ");
		print_ssa(o->succ->v);
		printf("\n");
	} else if (o->id == store) {
		printf("\t");
		print_ssa(o->succ->succ_next->v);
		printf(" := ");
		print_ssa(o->succ->v);
		printf(";\n");
	} else if (is_op(o->id)) {
		printf("\tt%d := ", o->i);
		print_ssa(o);
		printf(";\n");
	}
}

void
print_flow(edge * e)
{
	if (e != NULL) {
		print_flow(e->succ_next);
		print_statement(e->v);
	}
}

void
print_block(node * o)
{
	if (o != NULL) {
		printf("L%d:\n", o->i);
		print_flow(o->succ);
		print_block(o->next);
	}
}

void
print_graph(graph * g)
{
	print_block(g->entry);
}
