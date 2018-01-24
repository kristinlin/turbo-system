// structs

struct space { // where everyone and their properties are on the board
  char * name;
  int chance; // 0 for don't draw or 1 for draw
  int p1; //one house
  int p2; //two houses
  int p3; //three houses
  int properties[4];
};

struct chance { // for chance deck
  char * text;
  int money;
  int space;
};

struct game {
  struct space spaces[3];
  struct chance chance_cards[2];
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


//get shm val
struct game * getshm() {
  int mem_id = shmget(MEMKEY, 0, 0);
  //attach it to a pointer; obtain info
  struct game * shm_val = (struct game *) shmat(mem_id, 0, SHM_RDONLY);
  struct game updated = *shm_val;
  //detach it
  shmdt(shm_val);
  return updated;
}

//set new shm val
void setshm( struct game updated ) {
  int mem_id = shmget(MEMKEY, 0, 0);
  //attach it to a pointer
  struct game * shm_val = (struct game *) shmat(mem_id, 0, 0);
  *shm_val = updated;
  //detach it
  shmdt(shm_val);
}

struct game * init_structs() {
  struct game starter;

  //PLEASE REPLACE BELOW WITH CORRECT VALUES

  //this is one space
  starter.spaces[0].name = "GO";
  starter.spaces[0].chance = 1;
  starter.spaces[0].p1 = 500;
  starter.spaces[0].p2 = 1200;
  starter.spaces[0].p3 = 1900;
  starter.spaces[0].properties[0] = 0;
  starter.spaces[0].properties[1] = 0;
  starter.spaces[0].properties[2] = 0;
  starter.spaces[0].properties[3] = 0;
  //this is the second space
  starter.spaces[1].name = "Boardwalk1";
  starter.spaces[1].chance = 0;
  starter.spaces[0].p1 = 700;
  starter.spaces[0].p2 = 2200;
  starter.spaces[0].p3 = 7900;
  starter.spaces[1].properties[0] = 0;
  starter.spaces[1].properties[1] = 0;
  starter.spaces[1].properties[2] = 0;
  starter.spaces[1].properties[3] = 0;
  //continue on
  //below are two chance cards
  starter.chance_cards[0].text = "Go to boardwalk!";
  starter.chance_cards[0].money = 20;
  starter.chance_cards[0].space = 0;
  starter.chance_cards[1].text = "Go to boardwalk again!";
  starter.chance_cards[1].money = 20;
  starter.chance_cards[1].space = 0;
  return &starter;
}



