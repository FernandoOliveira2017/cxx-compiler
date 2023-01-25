cc = gcc
flags = -lm

cc: cc.c lexer.c parser.c decl.c expr.c type.c ssa.c hash.c list.c memory.c error.c helper.c
	$(cc) $^ -o $@ $(flags)

.PHONY: rm cc
