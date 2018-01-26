#include "board.c"
#include "mainfunctions.c"


void newgame(int from_clients[4], int to_subservers[4]) {

  printf("[mainserver] okay.\n");

  //all around handshake: client->server->subserver->REPEAT
  int buff;
  for (int x = 0; x < 4; x++) {
    read(from_clients[x], &buff, sizeof(buff));
    printf("[mainserver] received: %d\n", buff);
    write(to_subservers[x], &buff, sizeof(buff));
    write(to_subservers[x], &x, sizeof(int));
  }

  // set up semaphore
  int semid = semcreate();
  
  // set up both shm for spaces and chance cards
  int spaces_id = error_check(shmget(SPACE_MEMKEY,
				    sizeof(struct spaces) *40,
				    IPC_CREAT | IPC_EXCL | 0744));
  int chances_id = error_check(shmget(CHANCE_MEMKEY,
				    sizeof(struct chance) *14,
				    IPC_CREAT | IPC_EXCL | 0744));

  //putting info in and seeding randomizer
  init_spaces();
  init_chance();
  srand(getpid());
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // GAME LOOP
  
  Game.init();

  //BOARD
  //where you want to render the image in the window
  SDL_Rect rect = {0, 0, Game.screen.w, Game.screen.h};
  //pixel info of one element (monopoly board)
  SDL_Texture* texture1 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.screen.loaded_surface);

  //PIECES
  SDL_Rect texr0;
  texr0.x = 75; texr0.y = 0;
  texr0.w = 100 * SCREEN_SCALE; texr0.h = 100 * SCREEN_SCALE;
  SDL_Texture* zero = IMG_LoadTexture(Game.screen.renderer, "images/right.bmp");
  SDL_Rect texr1;
  texr1.x = Game.screen.w; texr1.y = Game.screen.h;
  texr1.w = 100 * SCREEN_SCALE; texr1.h = 100 * SCREEN_SCALE;
  SDL_Texture* one = IMG_LoadTexture(Game.screen.renderer, "images/left.bmp");
  SDL_Rect texr2;
  texr2.x = Game.screen.w; texr2.y = Game.screen.h;
  texr2.w = 100 * SCREEN_SCALE; texr2.h = 100 * SCREEN_SCALE;
  SDL_Texture* two = IMG_LoadTexture(Game.screen.renderer, "images/up.bmp");
  SDL_Rect texr3;
  texr3.x = Game.screen.w; texr3.y = Game.screen.h;
  texr3.w = 100 * SCREEN_SCALE; texr3.h = 100 * SCREEN_SCALE;
  SDL_Texture* three = IMG_LoadTexture(Game.screen.renderer, "images/down.bmp");

  SDL_Event event;

  //struct update
  struct update * start_update = (struct update *)malloc(sizeof(struct update));
  start_update->curr_player = rand() % 4; // current player
  int i= 0;
  for (i = 0; i < 4; i++) {
    start_update->position[i] = 0;
    start_update->gains[i] = 1500;
  }

  // struct turn
  struct turn * start_turn = (struct turn *)malloc(sizeof(struct turn));

  // deaths
  int deaths = 0;
  int player_status[4] = {1, 1, 1, 1};
  
  while(Game.running && deaths < 4) {

    while(SDL_PollEvent(&event)) {
      switch(event.type) {
	// user exits
      case SDL_QUIT: {
	Game.running = SDL_FALSE;
      } break;
	//you can add stuff like clicking, keyboard events, etc
      }
    }

    //send everyones update struct (for loop) (if they're still alive)
    printf("This is what every player gets: %d\n", start_update->gains[0]);
    for (i = 0; i < 4; i++) {
      if (player_status[i]) {
	write(to_subservers[i], start_update, sizeof(struct update));
      }
    }

    //change the image
    /*
    int * coors;
    coors = convert(start_update->position[0]);
    texr0.x = coors[0]; texr3.x = coors[1];
    free(coors);
    coors = convert(start_update->position[1]);
    texr1.x = coors[0]; texr3.x = coors[1];
    free(coors);
    coors = convert(start_update->position[2]);
    texr2.x = coors[0]; texr3.x = coors[1];
    free(coors);
    coors = convert(start_update->position[3]);
    texr3.x = coors[0]; texr3.x = coors[1];
    free(coors);*/
    

    //render the image
    SDL_RenderClear(Game.screen.renderer);
    rect.x = 0, rect.y = 0;
    SDL_RenderCopy(Game.screen.renderer, texture1, NULL, &rect);
    SDL_RenderCopy(Game.screen.renderer, zero, NULL, &texr0);
    SDL_RenderCopy(Game.screen.renderer, one, NULL, &texr1);
    SDL_RenderCopy(Game.screen.renderer, two, NULL, &texr2);
    SDL_RenderCopy(Game.screen.renderer, three, NULL, &texr3);
    SDL_RenderPresent(Game.screen.renderer);

    //read from current player for turn struct if they're still alive
    if (player_status[start_update->curr_player]) {
      read(from_clients[start_update->curr_player],
	   start_turn, sizeof(struct turn));

      // changes player indices in struct update and dues
      start_update->position[start_update->curr_player]= start_turn->curr_index;
      start_update->gains[0] = start_turn->dues[0];
      start_update->gains[1] = start_turn->dues[1];
      start_update->gains[2] = start_turn->dues[2];
      start_update->gains[3] = start_turn->dues[3];
      start_update->gains[4] = start_turn->dues[4];

      // if dead, mark somehow
      if (start_turn->dead) {
	deaths++;
	player_status[start_update->curr_player] = 0;
      }
    }	 
    
    //next player
    start_update->curr_player++;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //CLEAN UP

  //housekeeping like freeing memory
  SDL_DestroyTexture(texture1);
  SDL_DestroyTexture(zero);
  SDL_DestroyTexture(one);
  SDL_DestroyTexture(two);
  SDL_DestroyTexture(three);

  Game.quit();

  //free all memory used
  free(start_update);
  free(start_turn);
  
  // remove shared memory when game is over
  shmctl(spaces_id, IPC_RMID, NULL);
  shmctl(chances_id, IPC_RMID, NULL);
  semctl(semid, IPC_RMID, 0, NULL);
  semctl(semid, IPC_RMID, 1, NULL);
}
