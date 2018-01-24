#include <sys/shm.h>
#include "board.h"
#include "main.c"
#define SEMKEY 1023
#define MEMKEY 1123


union semun initVal;

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

  // set up shared mem NOTE: should be size of (struct game)
  int board_id = shmget(MEMKEY, sizeof(struct game), IPC_CREAT | IPC_EXCL);
  print(board_id)
    //  setshm(updated)


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  //housekeeping like freeing memory
  SDL_DestroyTexture(texture1);
  Game.quit();

  // remove shared memory when game is over
  shmdt(&board_id);
  shmctl(board_id, IPC_RMID, NULL);
}
