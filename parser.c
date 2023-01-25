#include <stdlib.h>
#include <stdbool.h>
#include "token.h"
#include "graph.h"
#include "ssaop.h"
#include "symbol.h"
#include "parser.h"

static node * parse(node * pred, table * o);

bool
next_token(token t)
{
	if (kind == t) {
		kind = lexan();
		return true;
	} else if (kind == eof)
		error("unexpected end of file");
	else
		return false;
}

void
expect(unsigned char c)
{
	if (!next_token(c))
		error("expected '%c'", c);
}

node *
compound(node * pred, table * o)
{
	while (!next_token('}'))
		if (declaration(kind))
			pred = decl(pred, o);
		else
			pred = parse(pred, o);
	return pred;
}

static node *
parse_asm(node * block, table * o)
{
	expect('(');
	if (kind != string_literal)
		error("expected string");
	if (next_token(':'))
		while (next_token(','));
	expect(')');
	expect(';');
}

static node *
parse_expr(node * block, table * o)
{
	expr(block, o);
	expect(';');
	return block;
}

static node *
parse_if(node * pred, table * o)
{
	node * block[6];
	
	block[4] = parse_branch(pred, o);
	block[0] = create_block(pred);
	block[1] = parse(block[0], o);
	if (next_token(token_else)) {
		block[5] = create_goto(block[1]);
		block[2] = create_block(block[1]);
		block[3] = parse(block[2], o);
		block[3] = create_block(block[3]);
		create_edge(block[4], block[2]);
		create_edge(block[5], block[3]);
		return block[3];
	}
	block[1] = create_block(block[1]);
	create_edge(block[4], block[1]);
	
	return block[1];
}

static node *
parse_while(node * pred, table * o)
{
	node * block[5];
	
	block[0] = create_block(pred);
	block[3] = parse_branch(block[0], o);
	block[1] = create_block(block[0]);
	block[2] = create_block(block[1]);
	block[1] = parse(block[1], o);
	block[4] = create_goto(block[1]);
	create_edge(block[3], block[2]);
	create_edge(block[4], block[0]);
	
	return block[1];
}

static node *
parse_do(node * pred, table * o)
{
	node * block[5];
	
	block[0] = create_block(pred);
	block[1] = create_block(block[0]);
	block[2] = create_block(block[1]);
	block[3] = parse(block[0], o);
	if (!next_token(token_while))
		error("expected while");
	block[4] = parse_if_then(block[1], o);
	expect(';');
	reuse_block(block[3], block[1]);
	create_edge(block[4], block[0]);
	
	return block[2];
}

static node *
parse_for(node * pred, table * o)
{
	node * block[6];
	
	expect('(');
	block[0] = create_block(pred);
	block[1] = create_block(block[0]);
	block[2] = create_block(block[1]);
	block[3] = create_block(block[2]);
	if (declaration(kind))
		decl(pred, o);
	else if (!next_token(';'))
		parse_expr(pred, o);
	if (!next_token(';')) {
		block[4] = create_if(block[0], o);
		expect(';');
	} else
		block[4] = NULL;
	if (!next_token(')')) {
		expr(block[2], o);
		expect(')');
	}
	block[1] = parse(block[1], o);
	block[5] = create_goto(block[2]);
	reuse_block(block[1], block[2]);
	create_edge(block[4], block[3]);
	create_edge(block[5], block[0]);
	
	return block[3];
}

static node *
parse_goto(node * pred, table * o)
{
	node * block[3];
	
	block[0] = create_block(pred);
	block[1] = create_goto(pred);
	if (kind == token_ident) {
		block[2] = create_node(ident);
		block[2]->data = dup();
	} else if (next_token('*'))
		block[2] = expr(pred, o);
	else
		error("expected id or '*'");
	create_edge(block[1], block[2]);
	expect(';');
	
	return block[0];
}

static node *
parse_return(node * pred, table * o)
{
	node * block[3];
	
	block[1] = parse_expr(pred, o);
	block[0] = create_block(pred);
	block[2] = create_return(pred, o);
	create_edge(block[2], block[1]);
	
	return block[0];
}

static node *
parse_stmt(node * pred, table * o)
{
	if (next_token(token_if))
		return parse_if(pred, o);
	else if (next_token(token_while))
		return parse_while(pred, o);
	else if (next_token(token_do))
		return parse_do(pred, o);
	else if (next_token(token_for))
		return parse_for(pred, o);
	else if (next_token(token_goto))
		return parse_goto(pred, o);
	else if (next_token(token_return))
		return parse_return(pred, o);
	else if (next_token('{')) {
		o = create_table(o);
		return compound(pred, o);
	} else
		return parse_expr(pred, o);
}

static node *
parse(node * pred, table * o)
{
	if (next_token(token_asm))
		return parse_asm(pred, o);
	else if (next_token(';'))
		return pred;
	else if (o->scope != 0)
		return parse_stmt(pred, o);
	else
		return decl(pred, o);
}

graph *
program(const char * file)
{
	graph * g;
	table * o;
	
	g = create_graph(file);
	o = create_table(NULL);
	filename = file;
	line_number = 1;
	get_next_char();
	kind = lexan();
	while (!next_token(eof))
		g->exit = parse(g->exit, o);
	print_graph(g);
	
	return g;
}
