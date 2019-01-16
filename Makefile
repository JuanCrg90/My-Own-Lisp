all:
	gcc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing

debug:
	gcc -std=c99 -Wall -g parsing.c mpc.c -ledit -lm -o parsing

clear:
	rm parsing
