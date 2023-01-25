#ifndef _symbol_h_
#define _symbol_h_

typedef struct list {
	node * head;
} list;

typedef struct type {
	u_int kind;
	union {
		list * args;
		node * lims;
		u_long qual;
		u_long type;
	};
	struct type * next;
} type;

typedef struct symbol {
	char * name;
	type * type;
	u_long addr;
} symbol;

typedef struct table {
	list ** hash;
	u_long scope;
	struct table * prev;
} table;

#endif
