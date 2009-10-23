LEX = flex -I
YACC = bison -y -d

CC = gcc -g -Wall -ansi -pedantic

objects = tree.o simplify.o bignum.o

integrator:	$(objects) y.tab.o lex.yy.o
		$(CC) -o integrator $(objects) y.tab.o lex.yy.o -ly -ll -lm

lex.yy.o:	lex.yy.c y.tab.h

y.tab.o:	

y.tab.c y.tab.h:	int.y tree.h simplify.h bignum.h
			$(YACC) int.y

lex.yy.c:	int.l tree.h
		$(LEX) int.l

tree.o:		tree.h y.tab.h

simplify.o:	simplify.h tree.h y.tab.h

bignum.o:	bignum.h

.PHONY: clean
clean:	
	rm integrator $(objects) y.tab.h y.tab.c lex.yy.c y.tab.o lex.yy.o