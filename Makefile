all:
	gcc noparallel.c -lm -lrt -o noparallel
	gcc thread.c -lm -lrt -lpthread -o thread
	gcc process.c -lm -lrt -o process

clean:
	rm thread
	rm noparallel
	rm process
