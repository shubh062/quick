all:
	gcc -c main.c
	gcc -c copy.c
	gcc -v -o quick main.o copy.o


clean:
	rm -rf *.o quick