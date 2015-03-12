warmup2: warmup2.o generalFunctions.o my402list.o
	gcc -o warmup2 -g -Wall warmup2.o generalFunctions.o my402list.o -lpthread -lm -D_POSIX_PTHREAD_SEMANTICS 

warmup2.o: warmup2.c my402list.h generalFunctions.h
	gcc -g -c -D_POSIX_PTHREAD_SEMANTICS -Wall warmup2.c 

generalFunctions.o: generalFunctions.c generalFunctions.h
	gcc -g -c -Wall generalFunctions.c

my402list.o: my402list.c my402list.h cs402.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o warmup2
