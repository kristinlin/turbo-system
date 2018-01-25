#include "board.h"
#include "main.c"
#include "pipe_networking.h"
#define SEMKEY 1023


union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
			      (Linux-specific) */
} initVal;

//create a semaphore
int semcreate(int val)
{
  int semid;
  semid = semget(SEMKEY, 1, IPC_EXCL | IPC_CREAT | 0600);
  if (semid==-1)  {
    printf("Oh no this semaphore already exists :(\n");
    return 0;
  }  else  {
    initVal.val = 1;
    printf("Status code: %d\n",semctl(semid, 0, SETVAL, initVal));
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

  // set up shared mem note: should be size of (struct game) and innards
  int board_size = sizeof(struct game) + sizeof(struct spaces) * 40 + sizeof(struct chance) * 14;
  int board_id = error_check(shmget(MEMKEY,
				    board_size,
				    IPC_CREAT | IPC_EXCL | 0777));
  //putting info in

  init_struct();
  struct spaces * currspace = getshm_space(0);
  printf("This is space 0, and it's name is: %s\n", currspace->name);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Game.init();
  //where you want to render the image in the window
  SDL_Rect rect = {0, 0, Game.screen.w, Game.screen.h};
  //pixel info of one element (monopoly board)
  SDL_Texture* texture1 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.screen.loaded_surface);

  SDL_Event event;
  int player = rand() % 4; // current player
  //struct update
  // struct start

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

    //send everyone update struct (for loop)

    //send to just the current player;

    //read from current player for turn struct
    // changes player indices in struct update and dues
    // if dead, mark somehow

    //next player
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //housekeeping like freeing memory
  SDL_DestroyTexture(texture1);
  Game.quit();

  // remove shared memory when game is over
  shmctl(board_id, IPC_RMID, NULL);
}
