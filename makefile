all: server client
	gcc -c pipe_networking.c
server: server.c
	gcc -I/usr/include/SDL/ -g server.c -o server -L/usr/lib -lSDL2
client: client.c
	gcc -g -o client client.c
sdl: main.c
	gcc -I/usr/include/SDL/ main.c -o sdltest -L/usr/lib -lSDL2 -lSDL2_image
runsdl: sdl
	./sdltest
board: board.h
	gcc -o board board.h
clean:
	rm *~; rm monopoly; rm client; rm server; rm board; rm *.o; rm sdltest; rm *.h.gch; rm -rf *.dSYM;
