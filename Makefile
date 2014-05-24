FLAGS = -Wall -Wextra -pedantic-errors -std=c99

all: rota

rota: main.c
	gcc $(FLAGS) -g main.c -o rota

clean:
	rm -f rota
