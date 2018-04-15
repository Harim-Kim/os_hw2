all:
	gcc noparallel.c -lm -lrt -o noparallel
	gcc thread.c -lm -lrt -lpthread -o thread
noparallel: noparallel.c
	gcc noparallel.c -lm -lrt -o noparallel

thread: thread.c
	gcc thread.c -lm -lrt -lpthread -o thread

clean:
	rm thread
	rm noparallel
