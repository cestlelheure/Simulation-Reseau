FLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-error=return-type -std=c99 -I.

all: main

adresse.o: adresse.c adresse.h
	gcc -c $(FLAGS) adresse.c

graphe.o: graphe.c graphe.h
	gcc -c $(FLAGS) graphe.c

station.o: station.c station.h
	gcc -c $(FLAGS) station.c

switch.o: switch.c switch.h
	gcc -c $(FLAGS) switch.c

trame.o: trame.c trame.h
	gcc -c $(FLAGS) trame.c

configuration.o: configuration.c configuration.h
	gcc -c $(FLAGS) configuration.c

main.o: main.c
	gcc -c $(FLAGS) main.c

main: main.o adresse.o trame.o graphe.o station.o switch.o configuration.o
	gcc $(FLAGS) -o main main.o adresse.o trame.o graphe.o station.o switch.o configuration.o


clean:
	rm -f *.o main
