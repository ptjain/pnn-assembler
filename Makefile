pnnas: pnn_assembler.c linkedlist.c linkedlist.h tobinary.h
	gcc -Wall -g pnn_assembler.c linkedlist.c tobinary.c -lm -o pnnas

clean:
	rm pnnas
