/*
  God bless:
  Stack Overflow (https://stackoverflow.com/questions/21007329/what-is-a-sdl-renderer)
  Stephen Meier (http://stephenmeier.net/2014/08/10/sdl-2-0-tutorial-00-the-basic-sturcture/)
  Lazy Foo Productions (http://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_w 1000
#define SCREEN_H 1000

#define SCREEN_SCALE .6
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
    printf("SDL error -> %s\n", SDL_GetError());
    exit(1);
  }

  //create SDL window for game; centered
  unsigned int w = Game.screen.w;
  unsigned int h = Game.screen.h;
  const char* name = Game.screen.name;
  Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MINIMIZED;
  Game.screen.window = SDL_CreateWindow(name,
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					w, h, flags);
  //create renderer to load image in created window
  Game.screen.renderer = SDL_CreateRenderer(Game.screen.window, -1,
					    SDL_RENDERER_ACCELERATED |
					    SDL_RENDERER_PRESENTVSYNC);
  //game is now running
  Game.running = SDL_TRUE;
  //load image
  Game.screen.loaded_surface = SDL_LoadBMP("images/hello_world.bmp");

  //Some other stuff  img = 
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

//free after
int * convert(int space) {
  //malloc
  int * coors = calloc(2, sizeof(int));
  int margin = Game.screen.w * .125;
  int w_scaled = Game.screen.w * .275;
  int h_scaled = Game.screen.w * .292;
  if (space < 11) {
    coors[0] = 0;
  } else if (space > 19 && space < 31) {
    coors[0] = Game.screen.w - 100*SCREEN_SCALE;
  } else  if (space > 10 && space < 20) {
    coors[0] = (Game.screen.w - w_scaled)/9 * (space%10 - 1) + margin;
  } else {
    coors[0] = (Game.screen.w - w_scaled)/9 * (9 - space%10) + margin;
  }

  if (space > 9 && space < 21) {
    coors[1] = 0;
  } else if (space == 0 || space > 29) {
    coors[1] = Game.screen.w - 100*SCREEN_SCALE;
  } else if (space > 0 && space < 10) {
    coors[1] = (Game.screen.h - h_scaled)/9 * (9 - space%10) + margin;
  } else {
    coors[1] = (Game.screen.h - h_scaled)/9 * (space%10 - 1) + margin;
  }
  return coors;
}
