// structs

#include <stdio.h>
#include <string.h>

struct Space { // where everyone and their properties are on the board
  char * name;
  int chance; // 0 for don't draw or 1 for draw
  int properties[4];
};

struct Chance { // for chance deck
  char * text;
  int money;
  int space;
};

struct Update { // idk
  int position[4] // where everyone is rn
};

struct Turn {
  int dead; // bankrupt or nah
  int curr_index; // where you are right window
  int dues[4]; //how much money you owe to each player
};
