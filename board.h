#define MEMKEY 1123
#include <sys/types.h>
#include <sys/shm.h>

// structs

struct spaces { // where everyone and their properties are on the board
  char * name;
  int chance; // 0 for don't draw or 1 for draw
  int property; // 0 = can't buy, 1 = buy
  int property_sold; // 1 if user decides to buy it, 0 if no, 0 if this is not a property spaces at all
  int change_money;// add this value onto the amount of money a player has (this will only have a value if you end on income tax, railroad, community chest, electric company, waterworks, or luxury tax)
  int rent[5]; // [0,0,0,0,0] if property = false, 0th num = base rent w/o properties
  int cost; // of buying the property
  int owner; // player/subserver # = owner, -1 if no one owns it by default!!
  int house_cost;
  int houses_owned; // num of properties owned
};

/*
int property; //0 - can't buy; 1 - for sale, 2 - bought
int cost; // how much it costs to buy
int house_cost; //cost of one house
int total_rent; //starts off as base rent; whenever player buys more, add house_cost to this; limit house_cost * 4; so can't buy more than 3 houses
*/


struct chance { // for chance deck
  char * text;
  int money;
  int spaces;
};

struct game {
  struct spaces spaces[39];
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
  struct game updated = malloc(sizeof(struct game));
  struct game updated = *shm_val;
  //detach it
  shmdt(shm_val);
  return &updated;
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

  // spaces: GO
  starter.spaces[0].name = "GO";
  starter.spaces[0].chance = 0;
  starter.spaces[0].property = 0;
  starter.spaces[0].property_sold = 0;
  starter.spaces[0].change_money = 200; // add 200 everytime u land on go
  starter.spaces[0].rent[0] = 0;
  starter.spaces[0].rent[1] = 0;
  starter.spaces[0].rent[2] = 0;
  starter.spaces[0].rent[3] = 0;
  starter.spaces[0].rent[4] = 0;
  starter.spaces[0].cost = 0;
  starter.spaces[0].house_cost = 0;
  starter.spaces[0].owner = -1;
  starter.spaces[0].houses_owned = 0;
  // spaces: Mediterranean Avenue
  starter.spaces[1].name = "Mediterranean Avenue";
  starter.spaces[1].chance = 0;
  starter.spaces[1].property = 1;
  starter.spaces[1].property_sold = 0;
  starter.spaces[1].change_money = 0;
  starter.spaces[1].rent[0] = 2;
  starter.spaces[1].rent[1] = 10;
  starter.spaces[1].rent[2] = 30;
  starter.spaces[1].rent[3] = 90;
  starter.spaces[1].rent[4] = 160;
  starter.spaces[1].cost = 60;
  starter.spaces[1].house_cost = 50;
  starter.spaces[1].owner = -1;
  starter.spaces[1].houses_owned = 0;
  // spaces: Community Chest, basically a chance card
  starter.spaces[2].name = "Community Chest";
  starter.spaces[2].chance = 1;
  starter.spaces[2].property = 0;
  starter.spaces[2].property_sold = 0;
  starter.spaces[2].change_money = 200; // arbitrary number
  starter.spaces[2].rent[0] = 0;
  starter.spaces[2].rent[1] = 0;
  starter.spaces[2].rent[2] = 0;
  starter.spaces[2].rent[3] = 0;
  starter.spaces[2].rent[4] = 0;
  starter.spaces[2].cost = 0;
  starter.spaces[2].house_cost = 0;
  starter.spaces[2].owner = -1;
  starter.spaces[2].houses_owned = 0;
  // spaces: Mediterranean Avenue
  starter.spaces[3].name = "Baltic Avenue";
  starter.spaces[3].chance = 0;
  starter.spaces[3].property = 1;
  starter.spaces[3].property_sold = 0;
  starter.spaces[3].change_money = 0;
  starter.spaces[3].rent[0] = 4;
  starter.spaces[3].rent[1] = 20;
  starter.spaces[3].rent[2] = 60;
  starter.spaces[3].rent[3] = 180;
  starter.spaces[3].rent[4] = 320;
  starter.spaces[3].cost = 60;
  starter.spaces[3].house_cost = 50;
  starter.spaces[3].owner = -1;
  starter.spaces[3].houses_owned = 0;
  // Income tax, will simplify to -200
  starter.spaces[4].name = "Income tax";
  starter.spaces[4].chance = 0;
  starter.spaces[4].property = 0;
  starter.spaces[4].property_sold = 0;
  starter.spaces[4].change_money = -200;
  starter.spaces[4].rent[0] = 0;
  starter.spaces[4].rent[1] = 0;
  starter.spaces[4].rent[2] = 0;
  starter.spaces[4].rent[3] = 0;
  starter.spaces[4].rent[4] = 0;
  starter.spaces[4].cost = 0;
  starter.spaces[4].house_cost = 0;
  starter.spaces[4].owner = -1;
  starter.spaces[4].houses_owned = 0;
  // spaces: Reading Railroad
  starter.spaces[5].name = "Reading Railroad";
  starter.spaces[5].chance = 0;
  starter.spaces[5].property = 1;
  starter.spaces[5].property_sold = 0;
  starter.spaces[5].change_money = 0;
  starter.spaces[5].rent[0] = 0;
  starter.spaces[5].rent[1] = 25;
  starter.spaces[5].rent[2] = 50;
  starter.spaces[5].rent[3] = 100;
  starter.spaces[5].rent[4] = 200;
  starter.spaces[5].cost = 200;
  starter.spaces[5].house_cost = 150;
  starter.spaces[5].owner = -1;
  starter.spaces[5].houses_owned = 0;
  // spaces: Oriental Avenue
  starter.spaces[6].name = "Oriental Avenue";
  starter.spaces[6].chance = 0;
  starter.spaces[6].property = 1;
  starter.spaces[6].property_sold = 0;
  starter.spaces[6].change_money = 0;
  starter.spaces[6].rent[0] = 6;
  starter.spaces[6].rent[1] = 30;
  starter.spaces[6].rent[2] = 90;
  starter.spaces[6].rent[3] = 270;
  starter.spaces[6].rent[4] = 400;
  starter.spaces[6].cost = 100;
  starter.spaces[6].house_cost = 50;
  starter.spaces[6].owner = -1;
  starter.spaces[6].houses_owned = 0;
  // spaces: Chance
  starter.spaces[7].name = "Chance";
  starter.spaces[7].chance = 1;
  starter.spaces[7].property = 0;
  starter.spaces[7].property_sold = 0;
  starter.spaces[7].change_money = 0;
  starter.spaces[7].rent[0] = 0;
  starter.spaces[7].rent[1] = 0;
  starter.spaces[7].rent[2] = 0;
  starter.spaces[7].rent[3] = 0;
  starter.spaces[7].rent[4] = 0;
  starter.spaces[7].cost = 0;
  starter.spaces[7].house_cost = 0;
  starter.spaces[7].owner = -1;
  starter.spaces[7].houses_owned = 0;
  // spaces: Vermont Avnue
  starter.spaces[8].name = "Vermont Avenue";
  starter.spaces[8].chance = 0;
  starter.spaces[8].property = 1;
  starter.spaces[8].property_sold = 0;
  starter.spaces[8].change_money = 0;
  starter.spaces[8].rent[0] = 6;
  starter.spaces[8].rent[1] = 30;
  starter.spaces[8].rent[2] = 90;
  starter.spaces[8].rent[3] = 270;
  starter.spaces[8].rent[4] = 400;
  starter.spaces[8].cost = 100;
  starter.spaces[8].house_cost = 50;
  starter.spaces[8].owner = -1;
  starter.spaces[8].houses_owned = 0;
  // spaces: Conneticut Avnue
  starter.spaces[9].name = "Conneticut Avenue";
  starter.spaces[9].chance = 0;
  starter.spaces[9].property = 1;
  starter.spaces[9].property_sold = 0;
  starter.spaces[9].change_money = 0;
  starter.spaces[9].rent[0] = 8;
  starter.spaces[9].rent[1] = 40;
  starter.spaces[9].rent[2] = 100;
  starter.spaces[9].rent[3] = 300;
  starter.spaces[9].rent[4] = 450;
  starter.spaces[9].cost = 120;
  starter.spaces[9].house_cost = 50;
  starter.spaces[9].owner = -1;
  starter.spaces[9].houses_owned = 0;
  // spaces: Just Visting Jail (literally nothing happens)
  starter.spaces[10].name = "Just Visting Jail";
  starter.spaces[10].chance = 0;
  starter.spaces[10].property = 0;
  starter.spaces[10].property_sold = 0;
  starter.spaces[10].change_money = 0;
  starter.spaces[10].rent[0] = 0;
  starter.spaces[10].rent[1] = 0;
  starter.spaces[10].rent[2] = 0;
  starter.spaces[10].rent[3] = 0;
  starter.spaces[10].rent[4] = 0;
  starter.spaces[10].cost = 0;
  starter.spaces[10].house_cost = 0;
  starter.spaces[10].owner = -1;
  starter.spaces[10].houses_owned = 0;
  // spaces: St. Charles Place
  starter.spaces[11].name = "St. Charles Place";
  starter.spaces[11].chance = 0;
  starter.spaces[11].property = 1;
  starter.spaces[11].property_sold = 0;
  starter.spaces[11].change_money = 0;
  starter.spaces[11].rent[0] = 10;
  starter.spaces[11].rent[1] = 50;
  starter.spaces[11].rent[2] = 150;
  starter.spaces[11].rent[3] = 450;
  starter.spaces[11].rent[4] = 625;
  starter.spaces[11].house_cost = 100;
  starter.spaces[11].cost = 140;
  starter.spaces[11].owner = -1;
  starter.spaces[11].houses_owned = 0;
  // spaces: Electric Company
  // this is a special case - no properties BUT rent will be collected if you are not the owner
  starter.spaces[12].name = "Electric Company";
  starter.spaces[12].chance = 0;
  starter.spaces[12].property = 0;
  starter.spaces[12].property_sold = 0;
  starter.spaces[12].change_money = 0;
  starter.spaces[12].rent[0] = 50;
  starter.spaces[12].rent[1] = 0;
  starter.spaces[12].rent[2] = 0;
  starter.spaces[12].rent[3] = 0;
  starter.spaces[12].rent[4] = 0;
  starter.spaces[12].cost = 150;
  starter.spaces[12].owner = -1;
  starter.spaces[12].house_cost = 0;
  starter.spaces[12].houses_owned = 0;
  // spaces: States Avenue
  starter.spaces[13].name = "States Avenue";
  starter.spaces[13].chance = 0;
  starter.spaces[13].property = 1;
  starter.spaces[13].property_sold = 0;
  starter.spaces[13].change_money = 0;
  starter.spaces[13].rent[0] = 10;
  starter.spaces[13].rent[1] = 50;
  starter.spaces[13].rent[2] = 150;
  starter.spaces[13].rent[3] = 450;
  starter.spaces[13].rent[4] = 625;
  starter.spaces[13].cost = 140;
  starter.spaces[13].house_cost = 100;
  starter.spaces[13].owner = -1;
  starter.spaces[13].houses_owned = 0;
  // spaces: States Avenue
  starter.spaces[14].name = "Virginia Avenue";
  starter.spaces[14].chance = 0;
  starter.spaces[14].property = 1;
  starter.spaces[14].property_sold = 0;
  starter.spaces[14].change_money = 0;
  starter.spaces[14].rent[0] = 12;
  starter.spaces[14].rent[1] = 60;
  starter.spaces[14].rent[2] = 180;
  starter.spaces[14].rent[3] = 500;
  starter.spaces[14].rent[4] = 700;
  starter.spaces[14].cost = 160;
  starter.spaces[14].house_cost = 100;
  starter.spaces[14].owner = -1;
  starter.spaces[14].houses_owned = 0;
  // spaces: Penn Railroad
  starter.spaces[15].name = "Pennsylvania Railroad";
  starter.spaces[15].chance = 0;
  starter.spaces[15].property = 1;
  starter.spaces[15].property_sold = 0;
  starter.spaces[15].change_money = 0;
  starter.spaces[15].rent[0] = 0;
  starter.spaces[15].rent[1] = 25;
  starter.spaces[15].rent[2] = 50;
  starter.spaces[15].rent[3] = 100;
  starter.spaces[15].rent[4] = 2050;
  starter.spaces[15].cost = 200;
  starter.spaces[15].house_cost = 150;
  starter.spaces[15].owner = -1;
  starter.spaces[15].houses_owned = 0;

  starter.spaces[16].name = "St. James Place";
  starter.spaces[16].chance = 0;
  starter.spaces[16].property = 1;
  starter.spaces[16].property_sold = 0;
  starter.spaces[16].change_money = 0;
  starter.spaces[16].rent[0] = 14;
  starter.spaces[16].rent[1] = 70;
  starter.spaces[16].rent[2] = 200;
  starter.spaces[16].rent[3] = 550;
  starter.spaces[16].rent[4] = 700;
  starter.spaces[16].cost = 180;
  starter.spaces[16].house_cost = 100;
  starter.spaces[16].owner = -1;
  starter.spaces[16].houses_owned = 0;
  // spaces: Com Chest
  starter.spaces[17].name = "Community Chest";
  starter.spaces[17].chance = 1;
  starter.spaces[17].property = 0;
  starter.spaces[17].property_sold = 0;
  starter.spaces[17].change_money = 200;
  starter.spaces[17].rent[0] = 50;
  starter.spaces[17].rent[1] = 0;
  starter.spaces[17].rent[2] = 0;
  starter.spaces[17].rent[3] = 0;
  starter.spaces[17].rent[4] = 0;
  starter.spaces[17].cost = 0;
  starter.spaces[17].house_cost = 0;
  starter.spaces[17].owner = -1;
  starter.spaces[17].houses_owned = 0;
  // spaces: Tennesee aVE
  starter.spaces[18].name = "Tennessee Avenue";
  starter.spaces[18].chance = 0;
  starter.spaces[18].property = 1;
  starter.spaces[18].property_sold = 0;
  starter.spaces[18].change_money = 0;
  starter.spaces[18].rent[0] = 14;
  starter.spaces[18].rent[1] = 70;
  starter.spaces[18].rent[2] = 200;
  starter.spaces[18].rent[3] = 550;
  starter.spaces[18].rent[4] = 700;
  starter.spaces[18].cost = 180;
  starter.spaces[18].house_cost = 100;
  starter.spaces[18].owner = -1;
  starter.spaces[18].houses_owned = 0;
  // spaces: ny ave
  starter.spaces[19].name = "New York Avenue";
  starter.spaces[19].chance = 0;
  starter.spaces[19].property = 1;
  starter.spaces[19].property_sold = 0;
  starter.spaces[19].change_money = 0;
  starter.spaces[19].rent[0] = 16;
  starter.spaces[19].rent[1] = 80;
  starter.spaces[19].rent[2] = 220;
  starter.spaces[19].rent[3] = 600;
  starter.spaces[19].rent[4] = 800;
  starter.spaces[19].cost = 200;
  starter.spaces[19].house_cost = 100;
  starter.spaces[19].owner = -1;
  starter.spaces[19].houses_owned = 0;
  // spaces: free parking literally what is the point of this spaces nothing happens
  starter.spaces[20].name = "Free Parking";
  starter.spaces[20].chance = 0;
  starter.spaces[20].property = 0;
  starter.spaces[20].property_sold = 0;
  starter.spaces[20].change_money = 0;
  starter.spaces[20].rent[0] = 50;
  starter.spaces[20].rent[1] = 0;
  starter.spaces[20].rent[2] = 0;
  starter.spaces[20].rent[3] = 0;
  starter.spaces[20].rent[4] = 0;
  starter.spaces[20].cost = 0;
  starter.spaces[20].house_cost = 0;
  starter.spaces[20].owner = -1;
  starter.spaces[20].houses_owned = 0;
  // spaces: kent
  starter.spaces[21].name = "Kentucky Avenue";
  starter.spaces[21].chance = 0;
  starter.spaces[21].property = 1;
  starter.spaces[21].property_sold = 0;
  starter.spaces[21].change_money = 0;
  starter.spaces[21].rent[0] = 18;
  starter.spaces[21].rent[1] = 90;
  starter.spaces[21].rent[2] = 250;
  starter.spaces[21].rent[3] = 700;
  starter.spaces[21].rent[4] = 875;
  starter.spaces[21].cost = 220;
  starter.spaces[21].house_cost = 150;
  starter.spaces[21].owner = -1;
  starter.spaces[21].houses_owned = 0;

  starter.spaces[22].name = "Chance";
  starter.spaces[22].chance = 1;
  starter.spaces[22].property = 0;
  starter.spaces[22].property_sold = 0;
  starter.spaces[22].change_money = 0;
  starter.spaces[22].rent[0] = 0;
  starter.spaces[22].rent[1] = 0;
  starter.spaces[22].rent[2] = 0;
  starter.spaces[22].rent[3] = 0;
  starter.spaces[22].rent[4] = 0;
  starter.spaces[22].cost = 0;
  starter.spaces[22].house_cost = 0;
  starter.spaces[22].owner = -1;
  starter.spaces[22].houses_owned = 0;

  starter.spaces[23].name = "Indiana Avenue";
  starter.spaces[23].chance = 0;
  starter.spaces[23].property = 1;
  starter.spaces[23].property_sold = 0;
  starter.spaces[23].change_money = 0;
  starter.spaces[23].rent[0] = 18;
  starter.spaces[23].rent[1] = 90;
  starter.spaces[23].rent[2] = 250;
  starter.spaces[23].rent[3] = 700;
  starter.spaces[23].rent[4] = 875;
  starter.spaces[23].cost = 220;
  starter.spaces[23].house_cost = 150;
  starter.spaces[23].owner = -1;
  starter.spaces[23].houses_owned = 0;

  starter.spaces[24].name = "Illinois Avenue";
  starter.spaces[24].chance = 0;
  starter.spaces[24].property = 1;
  starter.spaces[24].property_sold = 0;
  starter.spaces[24].change_money = 0;
  starter.spaces[24].rent[0] = 20;
  starter.spaces[24].rent[1] = 100;
  starter.spaces[24].rent[2] = 300;
  starter.spaces[24].rent[3] = 750;
  starter.spaces[24].rent[4] = 925;
  starter.spaces[24].cost = 240;
  starter.spaces[24].house_cost = 150;
  starter.spaces[24].owner = -1;
  starter.spaces[24].houses_owned = 0;

  starter.spaces[25].name = "B. & O. Railroad";
  starter.spaces[25].chance = 0;
  starter.spaces[25].property = 1;
  starter.spaces[25].property_sold = 0;
  starter.spaces[25].change_money = 0;
  starter.spaces[25].rent[0] = 0;
  starter.spaces[25].rent[1] = 25;
  starter.spaces[25].rent[2] = 50;
  starter.spaces[25].rent[3] = 100;
  starter.spaces[25].rent[4] = 200;
  starter.spaces[25].cost = 200;
  starter.spaces[25].house_cost = 150;
  starter.spaces[25].owner = -1;
  starter.spaces[25].houses_owned = 0;

  starter.spaces[26].name = "Atlantic Avenue";
  starter.spaces[26].chance = 0;
  starter.spaces[26].property = 1;
  starter.spaces[26].property_sold = 0;
  starter.spaces[26].change_money = 0;
  starter.spaces[26].rent[0] = 22;
  starter.spaces[26].rent[1] = 110;
  starter.spaces[26].rent[2] = 320;
  starter.spaces[26].rent[3] = 800;
  starter.spaces[26].rent[4] = 975;
  starter.spaces[26].cost = 260;
  starter.spaces[26].house_cost = 150;
  starter.spaces[26].owner = -1;
  starter.spaces[26].houses_owned = 0;

  starter.spaces[27].name = "Ventnor Avenue";
  starter.spaces[27].chance = 0;
  starter.spaces[27].property = 1;
  starter.spaces[27].property_sold = 0;
  starter.spaces[27].change_money = 0;
  starter.spaces[27].rent[0] = 22;
  starter.spaces[27].rent[1] = 110;
  starter.spaces[27].rent[2] = 320;
  starter.spaces[27].rent[3] = 800;
  starter.spaces[27].rent[4] = 975;
  starter.spaces[27].cost = 260;
  starter.spaces[27].house_cost = 150;
  starter.spaces[27].owner = -1;
  starter.spaces[27].houses_owned = 0;

  // this is a special case - no properties BUT rent will be collected if you are not the owner
  starter.spaces[28].name = "Water Works";
  starter.spaces[28].chance = 0;
  starter.spaces[28].property = 0;
  starter.spaces[28].property_sold = 0;
  starter.spaces[28].change_money = 0;
  starter.spaces[28].rent[0] = 50;
  starter.spaces[28].rent[1] = 0;
  starter.spaces[28].rent[2] = 0;
  starter.spaces[28].rent[3] = 0;
  starter.spaces[28].rent[4] = 0;
  starter.spaces[28].cost = 150;
  starter.spaces[28].house_cost = 0;
  starter.spaces[28].owner = -1;
  starter.spaces[28].houses_owned = 0;

  starter.spaces[29].name = "Marvin Gardens";
  starter.spaces[29].chance = 0;
  starter.spaces[29].property = 1;
  starter.spaces[29].property_sold = 0;
  starter.spaces[29].change_money = 0;
  starter.spaces[29].rent[0] = 24;
  starter.spaces[29].rent[1] = 120;
  starter.spaces[29].rent[2] = 360;
  starter.spaces[29].rent[3] = 850;
  starter.spaces[29].rent[4] = 1025;
  starter.spaces[29].cost = 280;
  starter.spaces[29].house_cost = 150;
  starter.spaces[29].owner = -1;
  starter.spaces[29].houses_owned = 0;

  // idk what we should do for this - skip two turns + pay $200
  starter.spaces[30].name = "Go to Jail";
  starter.spaces[30].chance = 0;
  starter.spaces[30].property = 0;
  starter.spaces[30].property_sold = 0;
  starter.spaces[30].change_money = -200;
  starter.spaces[30].rent[0] = 0;
  starter.spaces[30].rent[1] = 0;
  starter.spaces[30].rent[2] = 0;
  starter.spaces[30].rent[3] = 0;
  starter.spaces[30].rent[4] = 0;
  starter.spaces[30].cost = 0;
  starter.spaces[30].house_cost = 0;
  starter.spaces[30].owner = -1;
  starter.spaces[30].houses_owned = 0;

  starter.spaces[31].name = "Pacific Avenue";
  starter.spaces[31].chance = 0;
  starter.spaces[31].property = 1;
  starter.spaces[31].property_sold = 0;
  starter.spaces[31].change_money = 0;
  starter.spaces[31].rent[0] = 26;
  starter.spaces[31].rent[1] = 130;
  starter.spaces[31].rent[2] = 390;
  starter.spaces[31].rent[3] = 900;
  starter.spaces[31].rent[4] = 1100;
  starter.spaces[31].cost = 300;
  starter.spaces[31].house_cost = 200;
  starter.spaces[31].owner = -1;
  starter.spaces[31].houses_owned = 0;

  starter.spaces[32].name = "North Carolina Avenue";
  starter.spaces[32].chance = 0;
  starter.spaces[32].property = 1;
  starter.spaces[32].property_sold = 0;
  starter.spaces[32].change_money = 0;
  starter.spaces[32].rent[0] = 26;
  starter.spaces[32].rent[1] = 130;
  starter.spaces[32].rent[2] = 390;
  starter.spaces[32].rent[3] = 900;
  starter.spaces[32].rent[4] = 1100;
  starter.spaces[32].cost = 300;
  starter.spaces[32].house_cost = 200;
  starter.spaces[32].owner = -1;
  starter.spaces[32].houses_owned = 0;

  starter.spaces[33].name = "Community Chest";
  starter.spaces[33].chance = 1;
  starter.spaces[33].property = 0;
  starter.spaces[33].property_sold = 0;
  starter.spaces[33].change_money = 200;
  starter.spaces[33].rent[0] = 0;
  starter.spaces[33].rent[1] = 0;
  starter.spaces[33].rent[2] = 0;
  starter.spaces[33].rent[3] = 0;
  starter.spaces[33].rent[4] = 0;
  starter.spaces[33].cost = 0;
  starter.spaces[33].house_cost = 0;
  starter.spaces[33].owner = -1;
  starter.spaces[33].houses_owned = 0;

  starter.spaces[34].name = "Pennsylvania Avenue";
  starter.spaces[34].chance = 0;
  starter.spaces[34].property = 1;
  starter.spaces[34].property_sold = 0;
  starter.spaces[34].change_money = 0;
  starter.spaces[34].rent[0] = 28;
  starter.spaces[34].rent[1] = 150;
  starter.spaces[34].rent[2] = 240;
  starter.spaces[34].rent[3] = 1000;
  starter.spaces[34].rent[4] = 1200;
  starter.spaces[34].cost = 320;
  starter.spaces[34].house_cost = 200;
  starter.spaces[34].owner = -1;
  starter.spaces[34].houses_owned = 0;

  starter.spaces[35].name = "Short Line";
  starter.spaces[35].chance = 0;
  starter.spaces[35].property = 1;
  starter.spaces[35].property_sold = 0;
  starter.spaces[35].change_money = 0;
  starter.spaces[35].rent[0] = 0;
  starter.spaces[35].rent[1] = 25;
  starter.spaces[35].rent[2] = 50;
  starter.spaces[35].rent[3] = 100;
  starter.spaces[35].rent[4] = 200;
  starter.spaces[35].cost = 200;
  starter.spaces[35].house_cost = 150;
  starter.spaces[35].owner = -1;
  starter.spaces[35].houses_owned = 0;

  starter.spaces[36].name = "Chance";
  starter.spaces[36].chance = 1;
  starter.spaces[36].property = 0;
  starter.spaces[36].property_sold = 0;
  starter.spaces[36].change_money = 0;
  starter.spaces[36].rent[0] = 0;
  starter.spaces[36].rent[1] = 0;
  starter.spaces[36].rent[2] = 0;
  starter.spaces[36].rent[3] = 0;
  starter.spaces[36].rent[4] = 0;
  starter.spaces[36].cost = 0;
  starter.spaces[36].house_cost = 0;
  starter.spaces[36].owner = -1;
  starter.spaces[36].houses_owned = 0;

  starter.spaces[37].name = "Park Place";
  starter.spaces[37].chance = 0;
  starter.spaces[37].property = 1;
  starter.spaces[37].property_sold = 0;
  starter.spaces[37].change_money = 0;
  starter.spaces[37].rent[0] = 35;
  starter.spaces[37].rent[1] = 175;
  starter.spaces[37].rent[2] = 500;
  starter.spaces[37].rent[3] = 1100;
  starter.spaces[37].rent[4] = 1300;
  starter.spaces[37].cost = 350;
  starter.spaces[37].house_cost = 200;
  starter.spaces[37].owner = -1;
  starter.spaces[37].houses_owned = 0;

  starter.spaces[38].name = "Luxury Tax";
  starter.spaces[38].chance = 0;
  starter.spaces[38].property = 0;
  starter.spaces[38].property_sold = 0;
  starter.spaces[38].change_money = -100;
  starter.spaces[38].rent[0] = 0;
  starter.spaces[38].rent[1] = 0;
  starter.spaces[38].rent[2] = 0;
  starter.spaces[38].rent[3] = 0;
  starter.spaces[38].rent[4] = 0;
  starter.spaces[38].cost = 0;
  starter.spaces[38].house_cost = 0;
  starter.spaces[38].owner = -1;
  starter.spaces[38].houses_owned = 0;

  starter.spaces[39].name = "Boardwalk";
  starter.spaces[39].chance = 0;
  starter.spaces[39].property = 1;
  starter.spaces[39].property_sold = 0;
  starter.spaces[39].change_money = 0;
  starter.spaces[39].rent[0] = 50;
  starter.spaces[39].rent[1] = 200;
  starter.spaces[39].rent[2] = 600;
  starter.spaces[39].rent[3] = 1400;
  starter.spaces[39].rent[4] = 1700;
  starter.spaces[39].cost = 400;
  starter.spaces[39].house_cost = 200;
  starter.spaces[39].owner = -1;
  starter.spaces[39].houses_owned = 0;

  //continue o
  //below are two chance cards
  starter.chance_cards[0].text = "Go to boardwalk!";
  starter.chance_cards[0].money = 20;
  starter.chance_cards[0].spaces = 0;
  starter.chance_cards[1].text = "Go to boardwalk again!";
  starter.chance_cards[1].money = 20;
  starter.chance_cards[1].spaces = 0;
  return &starter;
}
