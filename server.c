#include "pipe_networking.h"
#include "pipe_networking.c"
#include <signal.h>

/*
Server:
    There will be an open server on a network that accepts clients (players).

    1. (Kristin) Server will create shared memory:
      a. struct space board [40] (an array of structs representing spaces on the board- zero being GO and 39 being the space before GO)
      Struct space will have attributes:
        i. char * name
        ii. int chance (0 - don’t draw or 1 - draw)
        iii. Int properties [4] (each slot holding rent of num player’s property)
      b. struct chance deck [16]
      Struct chance will have attributes:
        i. char * text (text of card)
        ii. int money (< 0 meaning you owe, > 0 meaning you gain)
        iii. int space (0 meaning GO, 16 meaning jail, etc.)
    2. (Brian) Server will create a semaphore, so only 1 person can access each shm.
*/

//===================ATTRIBUTES===================


/*
  a. int to_subservers [4] (holding file descriptors)
  b. int from_clients [4] (holding file descriptors)
  c. int player_indices [4] (holding the index of the space of the board players are occupying in relation to shm’s board)
  d. int curr_turn (index of player_indices--basically whose turn?)
  e. int player_gains [4] (holding money each player earned--someone paid rent)
  f. A struct named start with attributes for receiving from client::
     i. int index
     ii. int gains
  g. struct turn with attributes for sending to subserver:
     i. int dead (0 for bankrupt, 1 for still alive)
     ii. int curr_index
     iii. int dues [5] (owing money to player 0, player 1…, player 3, and BANK)
*/

//===================METHODS===================

static void sighandler(int);
void subserver_player(int, int, int);
void newgame(int [4], int [4]);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove(PIPE_NAME);
    exit(0);
  }
}

void subserver_player(int player, int from_server, int to_client) {
  printf("[subserver %d] reporting.\n", player);
  while(1) {
    //don't move
  }
}

void newgame(int from_clients[4], int to_subservers[4]) {
  printf("[mainserver] okay.\n");
  while (1) {
    //don't move
  }
}

//=======================MAIN=======================

/*
  main server: from_client & to_subserver
  subserver: from_server & to_client
  client: from_subserver & to_server

 */

int main() {

  int to_subservers [4];
  int from_clients[4];
  int connect_players;
  int f;

  //when server is terminated with ctrl+c
  signal(SIGINT, sighandler);

  while(1) {
    
    //set up WKP to wait and connect 4 players
    for (connect_players = 0; connect_players < 4; connect_players++) {

      //server from_client
      from_clients[connect_players] = server_setup();

      //server to_subserver
      int subserver_pipe [2];
      pipe( subserver_pipe );
      
      //subserver for each player
      f = fork();
      if (!f) {

	//from_server = subserver_pipe[READ]
	close(subserver_pipe[WRITE]);
	
	//perform handshake for to_client
	int to_client = server_connect(from_clients[connect_players]);

	//subservers don't need from_clients
	int close_client;
	for (close_client = 0; close_client < 4; close_client++) {
	  close(from_clients[close_client]);
	  from_clients[close_client] = -1;
	}

	//subservers need to_clients and from_server
	subserver_player(connect_players, subserver_pipe[READ], to_client);
      }

      //server needs to_subserver
      to_subservers[connect_players] = subserver_pipe[WRITE];
      
      printf("ok, player [%d]?\n", connect_players);
    }

    f = fork();

    //if parent process (main server); close connections
    if (f) {
      for (connect_players = 0; connect_players < 4; connect_players++) {
	close(from_clients[connect_players]);
	close(to_subservers[connect_players]);
	from_clients[connect_players] = -1;
	to_subservers[connect_players] = -1;
      }
    }

    //children or subserver; new game; keep connection
    else {
      newgame( from_clients, to_subservers );
    }
  }
}






