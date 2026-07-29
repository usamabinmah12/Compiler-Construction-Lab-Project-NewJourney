CC = gcc
CFLAGS = -Wall -g

all: interpreter

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

interpreter: parser.tab.c lex.yy.c ast.c ast.h
	$(CC) $(CFLAGS) -o interpreter parser.tab.c lex.yy.c ast.c

run: interpreter
	./interpreter example.txt

clean:
	rm -f interpreter parser.tab.c parser.tab.h lex.yy.c *.o
