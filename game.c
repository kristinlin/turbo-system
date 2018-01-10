#include "board.h"
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


void newgame(int from_clients[4], int to_subservers[4]) {

  printf("[mainserver] okay.\n");

  //all around handshake: client->server->subserver->REPEAT
  int buff;
  for (int x = 0; x < 4; x++) {
    read(from_clients[x], &buff, sizeof(buff));
    printf("[mainserver] received: %d\n", buff);
    write(to_subservers[x], &buff, sizeof(buff));
  }

  while(1) {
    //don't move
  }
}
