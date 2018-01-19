// structs

struct space { // where everyone and their properties are on the board
  char * name;
  int chance; // 0 for don't draw or 1 for draw
  int properties[4];
};

struct chance { // for chance deck
  char * text;
  int money;
  int space;
};

struct game {
  struct space spaces[40];
  struct chance chance_cards[16];
};

struct update { // idk
  int curr_player; //0 to 3
  int position[4]; // where everyone is rn
  int gains[4]; //how much money each person made
};

struct turn {
  int dead; // bankrupt or nah
  int curr_index; // where you are right window
  int dues[4]; //how much money you owe to each player
};
