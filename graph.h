#ifndef _graph_h_
#define _graph_h_

struct edge;

typedef struct node
{
	unsigned id;
	struct edge * pred;
	struct edge * succ;
	unsigned i;
	void * data;
	struct node * prev;
	struct node * next;
} node;

typedef struct edge {
	node * u;
	node * v;
	struct edge * pred_next;
	struct edge * succ_next;
} edge;

typedef struct graph {
	node * entry;
	node * exit;
} graph;

#endif
