all: server client
	gcc -c pipe_networking.c
server: server.c
	gcc -o server server.c
client: client.c
	gcc -o client client.c
sdl: main.c
	gcc -I/usr/include/SDL/ main.c -o sdltest -L/usr/lib -lSDL2
runsdl: sdl
	./sdltest
board: board.h
	gcc -o board board.h
clean:
	rm *~; rm *#; rm monopoly; rm *.o;
