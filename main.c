/*
  God bless:
  Stack Overflow (https://stackoverflow.com/questions/21007329/what-is-a-sdl-renderer)
  Stephen Meier (http://stephenmeier.net/2014/08/10/sdl-2-0-tutorial-00-the-basic-sturcture/)
  Lazy Foo Productions (http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php)
 */

#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#define SCREEN_w 1212
#define SCREEN_H 1238
#define SCREEN_SCALE 1
#define SCREEN_NAME "Monopoly"

//========================ATTRIBUTES======================================

void game_init(void);
void game_quit(void);
 
struct {
  //has the user quit?
  SDL_bool running;

  struct {
    unsigned int w;
    unsigned int h;
    const char * name;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* loaded_surface;
  } screen;
  
  // define "methods"
  void (*init)(void);
  void (*quit)(void);
} Game = {
  SDL_FALSE,
  {
    SCREEN_SCALE*SCREEN_w,
    SCREEN_SCALE*SCREEN_H,
    SCREEN_NAME,
    NULL,
    NULL
  },
  game_init,
  game_quit
};


//===========================METHODS===================================

//------------------------------------------------------
// INIT GAME
//create window and renderer & indicate image desired

void game_init(void) {
  if(SDL_Init(SDL_INIT_EVERYTHING)!=0) {
    printf("SDL error -> %sn", SDL_GetError());
    exit(1);
  }

  //create SDL window for game; centered
  unsigned int w = Game.screen.w;
  unsigned int h = Game.screen.h;
  const char* name = Game.screen.name;
  Game.screen.window = SDL_CreateWindow(name,
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					w, h, 0
					);
  //create renderer to load image in created window
  Game.screen.renderer = SDL_CreateRenderer(Game.screen.window, -1,
					    SDL_RENDERER_ACCELERATED |
					    SDL_RENDERER_PRESENTVSYNC);
  //game is now running
  Game.running = SDL_TRUE;
  //load image
  Game.screen.loaded_surface = SDL_LoadBMP("board.bmp");
}

//------------------------------------------------------
// QUIT GAME 
// free memory used

void game_quit(void) {
  SDL_FreeSurface(Game.screen.loaded_surface);
  Game.screen.loaded_surface = NULL;
  
  SDL_DestroyRenderer(Game.screen.renderer);
  Game.screen.renderer = NULL;

  SDL_DestroyWindow(Game.screen.window);
  Game.screen.window = NULL;

  SDL_Quit();
}


//==========================MAIN====================================
int main(int argc, char* argv[]) {

  Game.init();

  //where you want to render the image in the window
  SDL_Rect rect = {0, 0, Game.screen.w, Game.screen.h};
  //pixel info of one element (monopoly board)
  SDL_Texture* texture1 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.screen.loaded_surface);

  //sort of like events in Javascript
  SDL_Event event;
  while(Game.running) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
	// user exits
      case SDL_QUIT: {
	Game.running = SDL_FALSE;
      } break;

	/*you can add stuff like clicking, keyboard events, etc*/
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
  return 0;
}

