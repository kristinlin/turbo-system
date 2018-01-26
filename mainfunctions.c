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


/*
GAME SPACES
0: GO
1: Vine Street
2: Community Chest
3: Coventry Street
4: INCOME TAX
5: Marleybone Station
6: Leicester Square
7: Chance
8: Bow Street
9: Whitechapel Road
10: Just Visiting Jail
11: The Angel Islington
12: The Electric Company
13: Trafalgar Square
14: Northumrl'd Avenue
15: Fenchurch Station
16: M'Borough Street
17: Community Chest (2)
18: Fleet Street
19: Old Kent Road
20: Free Parking
21: Whitehall
22: Chance
23: Pentonville Road
24: Pall Mall
25: Kings Cross Station
26: Bond Street
27: Strand
28: Water Works
29: Regent Street
30: Go to Jail
31: Eliston Road
32: Piccadilly
33: Community Chest (3)
34: Oxford Street
35: Liverpool St Station
36: Chance
37: Park Lane
38: Super Tax
39: Mayfair
*/

//Convert space number to coordinates on board for placing the texture
//free mem after

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
