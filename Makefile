SRC = my_route_lookup.c my_route_lookup.h io.c io.h utils.c utils.h
CFLAGS = -Wall -O3
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
RT = assets/routing_table.txt 
T1 = assets/prueba1.txt
T2 = assets/prueba2.txt
T3 = assets/prueba3.txt

all: compile

compile: $(SRC)
	gcc $(CFLAGS) $(SRC) -o my_route_lookup -lm

test1: compile
	./my_route_lookup $(RT) $(T1)

test2: compile
	./my_route_lookup $(RT) $(T2)

test3: compile
	./my_route_lookup $(RT) $(T3)

valgrind1: compile
	valgrind $(VFLAGS) ./my_route_lookup $(RT) $(T1)

valgrind2: compile
	valgrind $(VFLAGS) ./my_route_lookup $(RT) $(T2)

valgrind3: compile
	valgrind $(VFLAGS) ./my_route_lookup $(RT) $(T3)

.PHONY: clean

clean:
	rm -f my_route_lookup

#RL Lab 2020 Switching UC3M
