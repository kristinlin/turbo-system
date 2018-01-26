#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "mainfunctions.c"

SDL_Rect placePlayerRect(int space)
{
	int* position = convert(space);
	SDL_Rect returnRect;
	returnRect.x = position[0];
	returnRect.y = position[1];
	returnRect.w = 50;
	returnRect.h = 50;
	return returnRect;
}

int placePlayer(SDL_Renderer renderer, char* spriteName, int space)
{
	SDL_Rect = placePlayerRect()
	SDL_RenderCopy(renderer, )
}

/*
SDL_Rect texr; texr.x = 40; texr.y = 670; texr.w = 50; texr.h = 50;


SDL_Texture* img = IMG_LoadTexture(Game.screen.renderer, "images/right.bmp");


SDL_RenderCopy(Game.screen.renderer, img, NULL, &texr);

*/

int main(int argc, char* argv[])
{
	placePlayer("Hello",1);
}
