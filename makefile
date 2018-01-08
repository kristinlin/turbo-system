all: main.c
	gcc -I/usr/include/SDL/ main.c -o monopoly -L/usr/lib -lSDL2
run: all
	./monopoly
clean:
	rm *~

