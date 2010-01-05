LEX = flex -I
YACC = bison -y -d

CC = gcc -g -Wall -ansi -pedantic -D_POSIX_SOURCE

objects = tree.o simplify.o bignum.o bigrat.o polynomial.o euclidean.o \
		coef_array.o prs.o squarefree.o

integrator:	$(objects) y.tab.o lex.yy.o
		$(CC) -o integrator $(objects) y.tab.o lex.yy.o -ly -lfl -lm

lex.yy.o:	lex.yy.c y.tab.h

y.tab.o:	

y.tab.c y.tab.h:	int.y tree.h simplify.h bignum.h bigrat.h polynomial.h \
				euclidean.h
			$(YACC) int.y

lex.yy.c:	int.l tree.h
		$(LEX) int.l

tree.o:		tree.h y.tab.h

simplify.o:	simplify.h tree.h y.tab.h

bignum.o:	bignum.h

bigrat.o:	bigrat.h bignum.h

polynomial.o:	polynomial.h bigrat.h

euclidean.o:	euclidean.h polynomial.h

coef_array.o:	coef_array.h polynomial.h

prs.o:		prs.h coef_array.h polynomial.h

squarefree.o:	squarefree.h

.PHONY: clean
clean:	
	rm integrator $(objects) y.tab.h y.tab.c lex.yy.c y.tab.o lex.yy.o