all: warmup1 listtest

warmup1: sort.o my402list.o
	gcc -o warmup1 -g sort.o my402list.o

listtest: listtest.o my402list.o
	gcc -o listtest -g listtest.o my402list.o

sort.o: sort.c my402list.h
	gcc -g -c -Wall sort.c

listtest.o: listtest.c my402list.h
	gcc -g -c -Wall listtest.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o listtest warmup1
