#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "board.h"
#include "main.c"
#define KEY 1023

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
      Attach semaphore to struct space board.
      Attach semaphore to struct chance deck.

      NOTE: Will need the client functions to actually access the semaphore
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


//union semun
//{
  // int              val;    /* Value for SETVAL */
   //struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   //unsigned short  *array;  /* Array for GETALL, SETALL */
   //struct seminfo  *__buf;  /* Buffer for IPC_INFO                               (Linux-specific) */
//} data;


int semcreate(int val)
{
  int semid;
  semid = semget(KEY, 1, IPC_EXCL | IPC_CREAT | 0600);
  if (semid==-1)
  {
    printf("Oh no this semaphore already exists :(\n");
    return 0;
  }
  else
  {
    printf("Status code: %d\n",semctl(semid, 0, SETVAL, val));
    printf("Tada, you have a semaphore now. It's at %d\n",semid);
  }
  return semid;
}



void newgame(int from_clients[4], int to_subservers[4]) {

  printf("[mainserver] okay.\n");

  //all around handshake: client->server->subserver->REPEAT
  int buff;
  for (int x = 0; x < 4; x++) {
    read(from_clients[x], &buff, sizeof(buff));
    printf("[mainserver] received: %d\n", buff);
    write(to_subservers[x], &buff, sizeof(buff));
  }

  // set up shared mem NOTE: should be size of (struct game)
  int board_id = shmget(getpid(), sizeof(struct game), IPC_CREAT | IPC_EXCL);
  // send board_id (game id) to all subservers (players)
  int i = 0;
  int gamepid = getpid();
  for (i = 0; i < 4; i++) {
    write(to_subservers[i], &gamepid, sizeof(int));
  }
  // fill out board info (names for spaces, etc.)

  Game.init(); 
  //where you want to render the image in the window
  SDL_Rect rect = {0, 0, Game.screen.w, Game.screen.h};
  //pixel info of one element (monopoly board)
  SDL_Texture* texture1 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.screen.loaded_surface);

  SDL_Event event;
  while(Game.running) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
  // user exits
      case SDL_QUIT: {
  Game.running = SDL_FALSE;
      } break;

  //you can add stuff like clicking, keyboard events, etc
      }
    }

    //render the image
    SDL_RenderClear(Game.screen.renderer);
    rect.x = 0, rect.y = 0;
    SDL_RenderCopy(Game.screen.renderer, texture1, NULL, &rect);
    SDL_RenderPresent(Game.screen.renderer);
  }

  //housekeeping like freeing memory
  SDL_DestroyTexture(texture1);
  Game.quit();

  // remove shared memory when game is over
  shmdt(&board_id);
  shmctl(board_id, IPC_RMID, NULL);
}
