all prog

prog: funcoes_uteis.o main.o
	gcc -o prog funcoes_uteis.o main.o

funcoes_uteis.o: funcoes_uteis.c
	gcc -c funcoes_uteis.c

main.o: main.c
	gcc -c main.c