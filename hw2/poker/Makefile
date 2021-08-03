all : run1000000

poker.o: poker.c
	cc -c -std=c11 -pedantic poker.c

poker: poker.o
	cc -o poker poker.o

run1000000: poker.c poker
	./poker 1000000

clean:
	rm -f poker *~ *.o

