#include "pipe_networking.h"
#include "pipe_networking.c"
#include "game.c"

//===================METHODS===================

static void sighandler(int);
void subserver_player(int, int, int);

static void sighandler(int signo) {
  int mem_id = shmget(SPACE_MEMKEY, 0, 0);
  shmctl(mem_id, IPC_RMID, NULL);
  mem_id = shmget(CHANCE_MEMKEY, 0, 0);
  shmctl(mem_id, IPC_RMID, NULL);


  if (signo == SIGINT) {
    printf("THis was the error: %s\n", strerror(errno));
    remove(PIPE_NAME);
    exit(0);
  }
}

void subserver_player(int player, int from_server, int to_client) {

  printf("[subserver %d] reporting.\n", player);

  //all around handshake: client->server->subserver->REPEAT
  int buff;
  read(from_server, &buff, sizeof(buff));
  printf("[subserver %d] received: %d\n", player, buff);
  write(to_client, &buff, sizeof(buff));
  read(from_server, &buff, sizeof(buff));
  write(to_client, &buff, sizeof(buff));

  struct update updateInfo;

  //pass info from server to client
  while(1) {
    read(from_server, &updateInfo, sizeof(updateInfo));
    write(to_client, &updateInfo, sizeof(updateInfo));
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
      close(subserver_pipe[READ]);
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
