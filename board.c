int error_check(int err) {
  //  printf("%d\n", err);
  if (err < 0) {
    printf("This was the error: %s\n", strerror(errno));
    exit(1);
  }
  return err;
}


//====================================================================
// DEFINING STRUCTS


// where everyone and their properties are on the board
struct spaces {

  char name[256];

  // 0 for don't draw or 1 for draw
  int chance;

  // 0 = can't buy, 1 = buy, 2 = possible to buy but sold
  int property;

  // add this value onto the amount of money a player has (this will only have a value if you end on income tax, railroad, community chest, electric company, waterworks, or luxury tax)
  int change_money;

  // of buying the property
  int cost;

  // player/subserver # = owner, -1 if no one owns it by default!!
  int owner;

  //how much it coses to buy one house
  int house_cost;

  // num of properties owned
  int houses_owned;

  // [0,0,0,0,0] if property = false, 0th num = base rent w/o properties
  int rent[5];

};


struct chance { // for chance deck
  char text [256];
  int money;
  int spaces;
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


//====================================================================
// ACCESSING THE SEM

#if defined (__linux__)
union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
			      (Linux-specific) */
} initVal;
#endif

union semun initVal;


//create two semaphores for both shms
int semcreate()
{
  int semid;
  semid = error_check(semget(SEMKEY, 2, IPC_EXCL | IPC_CREAT | 0600));
  initVal.val = 1;
  printf("Status code: %d\n",semctl(semid, 0, SETVAL, initVal));
  printf("Status code: %d\n",semctl(semid, 1, SETVAL, initVal));
  printf("Tada, you have a semaphore now. It's at %d\n", semid);
  return semid;
}


//SEMVAL = 0 (occupied) ; SEMVAL = 1 (unoccupied)
int gate( int semnum, int action ) {
  struct sembuf ctlVal;
  int sem_id = semget(SEMKEY, 0, 0);
  ctlVal.sem_num = semnum;
  if (action) {
    ctlVal.sem_op = -1; //enter = 1
  } else {
    ctlVal.sem_op = 1; //leave = 2
  }
  semop(sem_id, &ctlVal, 1);
  return ctlVal.sem_op;
}


//====================================================================
// ACCESSING THE SHM


//get shm val -- REMEMBER TO FREE AFTER
struct spaces * getshm_space(int space) {

  int mem_id = error_check(shmget(SPACE_MEMKEY, 0, 0));

  //attach it to a pointer; obtain info
  struct spaces * shm_spaces = (struct spaces*) shmat(mem_id, 0, SHM_RDONLY);
  struct spaces * currspace = (struct spaces*)malloc(sizeof(struct spaces));
  strcpy(currspace->name, shm_spaces[space].name);
  currspace->chance = shm_spaces[space].chance;
  currspace->property = shm_spaces[space].property;
  currspace->change_money = shm_spaces[space].change_money;
  currspace->rent[0] = shm_spaces[space].rent[0];
  currspace->rent[1] = shm_spaces[space].rent[1];
  currspace->rent[2] = shm_spaces[space].rent[2];
  currspace->rent[3] = shm_spaces[space].rent[3];
  currspace->rent[4] = shm_spaces[space].rent[4];
  currspace->cost = shm_spaces[space].cost;
  currspace->house_cost = shm_spaces[space].house_cost;
  currspace->owner = shm_spaces[space].owner;
  currspace->houses_owned = shm_spaces[space].houses_owned;

  //detach it
  shmdt(shm_spaces);
  return currspace;
}


//get chance card -- REMEMBER TO FREE AFTER
struct chance * getshm_chance() {
  int mem_id = shmget(CHANCE_MEMKEY, 0, 0);

  //attach it to a pointer; obtain info
  struct chance * shm_chance = (struct chance *) shmat(mem_id, 0, SHM_RDONLY);
  struct chance * chance_card = (struct chance *)malloc(sizeof(struct chance));
  int rand_card = rand() % 14;
  strcpy(chance_card->text, shm_chance[rand_card].text);
  chance_card->money = shm_chance[rand_card].money;
  chance_card->spaces = shm_chance[rand_card].spaces;

  //detach it
  shmdt(shm_chance);
  return chance_card;
  }


//set a space in shm; make sure to allocate space before passing updated;
void setshm_space( int space, struct spaces * updated ) {
  int mem_id = shmget(SPACE_MEMKEY, 0, 0);

  //attach it to a pointer
  struct spaces * shm_spaces = (struct spaces *) shmat(mem_id, 0, 0);
  strcpy(shm_spaces[space].name, updated->name);
  shm_spaces[space].chance = updated->chance;
  shm_spaces[space].property = updated->property;
  shm_spaces[space].change_money = updated->change_money;
  shm_spaces[space].cost = updated->cost;
  shm_spaces[space].owner = updated->owner;
  shm_spaces[space].house_cost = updated->house_cost;
  shm_spaces[space].houses_owned = updated->houses_owned;
  shm_spaces[space].rent[0] = updated->rent[0];
  shm_spaces[space].rent[1] = updated->rent[1];
  shm_spaces[space].rent[2] = updated->rent[2];
  shm_spaces[space].rent[3] = updated->rent[3];
  shm_spaces[space].rent[4] = updated->rent[4];

  //detach it
  shmdt(shm_spaces);
  free(updated);
}



//====================================================================
// INIT THE SHM WITH THE INIT BOARD


//set the shm as the initial board; clear of everything for new game
void init_spaces() {

  //open shm mem
  int mem_id = shmget(SPACE_MEMKEY, 0, 0);
  struct spaces * starter = (struct spaces *) shmat(mem_id, 0, 0);

  strcpy(starter[0].name, "GO");
  //  printf("This is the first space. %s\n", starter[0].name);
  starter[0].chance = 0;
  starter[0].property = 0;
  starter[0].change_money = 0;
  starter[0].rent[0] = 0;
  starter[0].rent[1] = 0;
  starter[0].rent[2] = 0;
  starter[0].rent[3] = 0;
  starter[0].rent[4] = 0;
  starter[0].cost = 0;
  starter[0].house_cost = 0;
  starter[0].owner = -1;
  starter[0].houses_owned = 0;

  strcpy(starter[1].name,"Vine Street");
  printf("This is the first space. %s\n", starter[1].name);
  starter[1].chance = 0;
  starter[1].property = 1;
  starter[1].change_money = 0;
  starter[1].rent[0] = 2;
  starter[1].rent[1] = 10;
  starter[1].rent[2] = 30;
  starter[1].rent[3] = 90;
  starter[1].rent[4] = 160;
  starter[1].cost = 60;
  starter[1].house_cost = 50;
  starter[1].owner = -1;
  starter[1].houses_owned = 0;
  // spaces: Community Chest, basically a chance card
  strcpy(starter[2].name,"Community Chest");
  starter[2].chance = 1;
  starter[2].property = 0;
  starter[2].change_money = 200; // arbitrary number
  starter[2].rent[0] = 0;
  starter[2].rent[1] = 0;
  starter[2].rent[2] = 0;
  starter[2].rent[3] = 0;
  starter[2].rent[4] = 0;
  starter[2].cost = 0;
  starter[2].house_cost = 0;
  starter[2].owner = -1;
  starter[2].houses_owned = 0;
  // spaces: Mediterranean Avenue
  strcpy(starter[3].name,"Coventry Street");
  starter[3].chance = 0;
  starter[3].property = 1;
  starter[3].change_money = 0;
  starter[3].rent[0] = 4;
  starter[3].rent[1] = 20;
  starter[3].rent[2] = 60;
  starter[3].rent[3] = 180;
  starter[3].rent[4] = 320;
  starter[3].cost = 60;
  starter[3].house_cost = 50;
  starter[3].owner = -1;
  starter[3].houses_owned = 0;
  // Income tax, will simplify to -200
  strcpy(starter[4].name,"Income Tax");
  starter[4].chance = 0;
  starter[4].property = 0;
  starter[4].change_money = -200;
  starter[4].rent[0] = 0;
  starter[4].rent[1] = 0;
  starter[4].rent[2] = 0;
  starter[4].rent[3] = 0;
  starter[4].rent[4] = 0;
  starter[4].cost = 0;
  starter[4].house_cost = 0;
  starter[4].owner = -1;
  starter[4].houses_owned = 0;
  // spaces: Reading Railroad
  strcpy(starter[5].name,"Marylebone Station");
  starter[5].chance = 0;
  starter[5].property = 1;
  starter[5].change_money = 0;
  starter[5].rent[0] = 0;
  starter[5].rent[1] = 25;
  starter[5].rent[2] = 50;
  starter[5].rent[3] = 100;
  starter[5].rent[4] = 200;
  starter[5].cost = 200;
  starter[5].house_cost = 150;
  starter[5].owner = -1;
  starter[5].houses_owned = 0;
  // spaces: Oriental Avenue
  strcpy(starter[6].name,"Leicester Square");
  starter[6].chance = 0;
  starter[6].property = 1;
  starter[6].change_money = 0;
  starter[6].rent[0] = 6;
  starter[6].rent[1] = 30;
  starter[6].rent[2] = 90;
  starter[6].rent[3] = 270;
  starter[6].rent[4] = 400;
  starter[6].cost = 100;
  starter[6].house_cost = 50;
  starter[6].owner = -1;
  starter[6].houses_owned = 0;
  // spaces: Chance
  strcpy(starter[7].name,"Chance");
  starter[7].chance = 1;
  starter[7].property = 0;
  starter[7].change_money = 0;
  starter[7].rent[0] = 0;
  starter[7].rent[1] = 0;
  starter[7].rent[2] = 0;
  starter[7].rent[3] = 0;
  starter[7].rent[4] = 0;
  starter[7].cost = 0;
  starter[7].house_cost = 0;
  starter[7].owner = -1;
  starter[7].houses_owned = 0;
  // spaces: Vermont Avnue
  strcpy(starter[8].name,"Bow Street");
  starter[8].chance = 0;
  starter[8].property = 1;
  starter[8].change_money = 0;
  starter[8].rent[0] = 6;
  starter[8].rent[1] = 30;
  starter[8].rent[2] = 90;
  starter[8].rent[3] = 270;
  starter[8].rent[4] = 400;
  starter[8].cost = 100;
  starter[8].house_cost = 50;
  starter[8].owner = -1;
  starter[8].houses_owned = 0;
  // spaces: Conneticut Avnue
  strcpy(starter[9].name,"Whitechapel Road");
  starter[9].chance = 0;
  starter[9].property = 1;
  starter[9].change_money = 0;
  starter[9].rent[0] = 8;
  starter[9].rent[1] = 40;
  starter[9].rent[2] = 100;
  starter[9].rent[3] = 300;
  starter[9].rent[4] = 450;
  starter[9].cost = 120;
  starter[9].house_cost = 50;
  starter[9].owner = -1;
  starter[9].houses_owned = 0;
  // spaces: Just Visting Jail (literally nothing happens)
  strcpy(starter[10].name,"Just Visting Jail");
  starter[10].chance = 0;
  starter[10].property = 0;
  starter[10].change_money = 0;
  starter[10].rent[0] = 0;
  starter[10].rent[1] = 0;
  starter[10].rent[2] = 0;
  starter[10].rent[3] = 0;
  starter[10].rent[4] = 0;
  starter[10].cost = 0;
  starter[10].house_cost = 0;
  starter[10].owner = -1;
  starter[10].houses_owned = 0;
  // spaces: St. Charles Place
  strcpy(starter[11].name,"The Angel Islington");
  starter[11].chance = 0;
  starter[11].property = 1;
  starter[11].change_money = 0;
  starter[11].rent[0] = 10;
  starter[11].rent[1] = 50;
  starter[11].rent[2] = 150;
  starter[11].rent[3] = 450;
  starter[11].rent[4] = 625;
  starter[11].house_cost = 100;
  starter[11].cost = 140;
  starter[11].owner = -1;
  starter[11].houses_owned = 0;
  // spaces: Electric Company
  // this is a special case - no properties BUT rent will be collected if you are not the owner
  strcpy(starter[12].name,"Electric Company");
  starter[12].chance = 0;
  starter[12].property = 0;
  starter[12].change_money = 0;
  starter[12].rent[0] = 50;
  starter[12].rent[1] = 0;
  starter[12].rent[2] = 0;
  starter[12].rent[3] = 0;
  starter[12].rent[4] = 0;
  starter[12].cost = 150;
  starter[12].owner = -1;
  starter[12].house_cost = 0;
  starter[12].houses_owned = 0;
  // spaces: States Avenue
  strcpy(starter[13].name,"Trafalgar Square");
  starter[13].chance = 0;
  starter[13].property = 1;
  starter[13].change_money = 0;
  starter[13].rent[0] = 10;
  starter[13].rent[1] = 50;
  starter[13].rent[2] = 150;
  starter[13].rent[3] = 450;
  starter[13].rent[4] = 625;
  starter[13].cost = 140;
  starter[13].house_cost = 100;
  starter[13].owner = -1;
  starter[13].houses_owned = 0;
  // spaces: States Avenue
  strcpy(starter[14].name,"Northumrid Avenue");
  starter[14].chance = 0;
  starter[14].property = 1;
  starter[14].change_money = 0;
  starter[14].rent[0] = 12;
  starter[14].rent[1] = 60;
  starter[14].rent[2] = 180;
  starter[14].rent[3] = 500;
  starter[14].rent[4] = 700;
  starter[14].cost = 160;
  starter[14].house_cost = 100;
  starter[14].owner = -1;
  starter[14].houses_owned = 0;
  // spaces: Penn Railroad
  strcpy(starter[15].name,"Fenchurch St. Station");
  starter[15].chance = 0;
  starter[15].property = 1;
  starter[15].change_money = 0;
  starter[15].rent[0] = 0;
  starter[15].rent[1] = 25;
  starter[15].rent[2] = 50;
  starter[15].rent[3] = 100;
  starter[15].rent[4] = 2050;
  starter[15].cost = 200;
  starter[15].house_cost = 150;
  starter[15].owner = -1;
  starter[15].houses_owned = 0;

  strcpy(starter[16].name,"M'Bourough Street");
  starter[16].chance = 0;
  starter[16].property = 1;
  starter[16].change_money = 0;
  starter[16].rent[0] = 14;
  starter[16].rent[1] = 70;
  starter[16].rent[2] = 200;
  starter[16].rent[3] = 550;
  starter[16].rent[4] = 700;
  starter[16].cost = 180;
  starter[16].house_cost = 100;
  starter[16].owner = -1;
  starter[16].houses_owned = 0;
  // spaces: Com Chest
  strcpy(starter[17].name,"Community Chest");
  starter[17].chance = 1;
  starter[17].property = 0;
  starter[17].change_money = 200;
  starter[17].rent[0] = 50;
  starter[17].rent[1] = 0;
  starter[17].rent[2] = 0;
  starter[17].rent[3] = 0;
  starter[17].rent[4] = 0;
  starter[17].cost = 0;
  starter[17].house_cost = 0;
  starter[17].owner = -1;
  starter[17].houses_owned = 0;
  // spaces: Tennesee aVE
  strcpy(starter[18].name,"Fleet Street");
  starter[18].chance = 0;
  starter[18].property = 1;
  starter[18].change_money = 0;
  starter[18].rent[0] = 14;
  starter[18].rent[1] = 70;
  starter[18].rent[2] = 200;
  starter[18].rent[3] = 550;
  starter[18].rent[4] = 700;
  starter[18].cost = 180;
  starter[18].house_cost = 100;
  starter[18].owner = -1;
  starter[18].houses_owned = 0;
  // spaces: ny ave
  strcpy(starter[19].name,"Old Kent Road");
  starter[19].chance = 0;
  starter[19].property = 1;
  starter[19].change_money = 0;
  starter[19].rent[0] = 16;
  starter[19].rent[1] = 80;
  starter[19].rent[2] = 220;
  starter[19].rent[3] = 600;
  starter[19].rent[4] = 800;
  starter[19].cost = 200;
  starter[19].house_cost = 100;
  starter[19].owner = -1;
  starter[19].houses_owned = 0;
  // spaces: free parking literally what is the point of this spaces nothing happens
  strcpy(starter[20].name,"Free Parking");
  starter[20].chance = 0;
  starter[20].property = 0;
  starter[20].change_money = 0;
  starter[20].rent[0] = 50;
  starter[20].rent[1] = 0;
  starter[20].rent[2] = 0;
  starter[20].rent[3] = 0;
  starter[20].rent[4] = 0;
  starter[20].cost = 0;
  starter[20].house_cost = 0;
  starter[20].owner = -1;
  starter[20].houses_owned = 0;
  // spaces: kent
  strcpy(starter[21].name,"Whitehall");
  starter[21].chance = 0;
  starter[21].property = 1;
  starter[21].change_money = 0;
  starter[21].rent[0] = 18;
  starter[21].rent[1] = 90;
  starter[21].rent[2] = 250;
  starter[21].rent[3] = 700;
  starter[21].rent[4] = 875;
  starter[21].cost = 220;
  starter[21].house_cost = 150;
  starter[21].owner = -1;
  starter[21].houses_owned = 0;

  strcpy(starter[22].name,"Chance");
  starter[22].chance = 1;
  starter[22].property = 0;
  starter[22].change_money = 0;
  starter[22].rent[0] = 0;
  starter[22].rent[1] = 0;
  starter[22].rent[2] = 0;
  starter[22].rent[3] = 0;
  starter[22].rent[4] = 0;
  starter[22].cost = 0;
  starter[22].house_cost = 0;
  starter[22].owner = -1;
  starter[22].houses_owned = 0;

  strcpy(starter[23].name,"Pentonville Road");
  starter[23].chance = 0;
  starter[23].property = 1;
  starter[23].change_money = 0;
  starter[23].rent[0] = 18;
  starter[23].rent[1] = 90;
  starter[23].rent[2] = 250;
  starter[23].rent[3] = 700;
  starter[23].rent[4] = 875;
  starter[23].cost = 220;
  starter[23].house_cost = 150;
  starter[23].owner = -1;
  starter[23].houses_owned = 0;

  strcpy(starter[24].name,"Pall Mall");
  starter[24].chance = 0;
  starter[24].property = 1;
  starter[24].change_money = 0;
  starter[24].rent[0] = 20;
  starter[24].rent[1] = 100;
  starter[24].rent[2] = 300;
  starter[24].rent[3] = 750;
  starter[24].rent[4] = 925;
  starter[24].cost = 240;
  starter[24].house_cost = 150;
  starter[24].owner = -1;
  starter[24].houses_owned = 0;

  strcpy(starter[25].name,"Kings Cross Station");
  starter[25].chance = 0;
  starter[25].property = 1;
  starter[25].change_money = 0;
  starter[25].rent[0] = 0;
  starter[25].rent[1] = 25;
  starter[25].rent[2] = 50;
  starter[25].rent[3] = 100;
  starter[25].rent[4] = 200;
  starter[25].cost = 200;
  starter[25].house_cost = 150;
  starter[25].owner = -1;
  starter[25].houses_owned = 0;

  strcpy(starter[26].name,"Bond Street");
  starter[26].chance = 0;
  starter[26].property = 1;
  starter[26].change_money = 0;
  starter[26].rent[0] = 22;
  starter[26].rent[1] = 110;
  starter[26].rent[2] = 320;
  starter[26].rent[3] = 800;
  starter[26].rent[4] = 975;
  starter[26].cost = 260;
  starter[26].house_cost = 150;
  starter[26].owner = -1;
  starter[26].houses_owned = 0;

  strcpy(starter[27].name,"Strand");
  starter[27].chance = 0;
  starter[27].property = 1;
  starter[27].change_money = 0;
  starter[27].rent[0] = 22;
  starter[27].rent[1] = 110;
  starter[27].rent[2] = 320;
  starter[27].rent[3] = 800;
  starter[27].rent[4] = 975;
  starter[27].cost = 260;
  starter[27].house_cost = 150;
  starter[27].owner = -1;
  starter[27].houses_owned = 0;

  // this is a special case - no properties BUT rent will be collected if you are not the owner
  strcpy(starter[28].name,"Water Works");
  starter[28].chance = 0;
  starter[28].property = 0;
  starter[28].change_money = 0;
  starter[28].rent[0] = 50;
  starter[28].rent[1] = 0;
  starter[28].rent[2] = 0;
  starter[28].rent[3] = 0;
  starter[28].rent[4] = 0;
  starter[28].cost = 150;
  starter[28].house_cost = 0;
  starter[28].owner = -1;
  starter[28].houses_owned = 0;

  strcpy(starter[29].name,"Regent Street");
  starter[29].chance = 0;
  starter[29].property = 1;
  starter[29].change_money = 0;
  starter[29].rent[0] = 24;
  starter[29].rent[1] = 120;
  starter[29].rent[2] = 360;
  starter[29].rent[3] = 850;
  starter[29].rent[4] = 1025;
  starter[29].cost = 280;
  starter[29].house_cost = 150;
  starter[29].owner = -1;
  starter[29].houses_owned = 0;

  // idk what we should do for this - skip two turns + pay $200
  strcpy(starter[30].name,"Go to Jail");
  starter[30].chance = 0;
  starter[30].property = 0;
  starter[30].change_money = -200;
  starter[30].rent[0] = 0;
  starter[30].rent[1] = 0;
  starter[30].rent[2] = 0;
  starter[30].rent[3] = 0;
  starter[30].rent[4] = 0;
  starter[30].cost = 0;
  starter[30].house_cost = 0;
  starter[30].owner = -1;
  starter[30].houses_owned = 0;

  strcpy(starter[31].name,"Euston Road");
  starter[31].chance = 0;
  starter[31].property = 1;
  starter[31].change_money = 0;
  starter[31].rent[0] = 26;
  starter[31].rent[1] = 130;
  starter[31].rent[2] = 390;
  starter[31].rent[3] = 900;
  starter[31].rent[4] = 1100;
  starter[31].cost = 300;
  starter[31].house_cost = 200;
  starter[31].owner = -1;
  starter[31].houses_owned = 0;

  strcpy(starter[32].name,"Piccadilly");
  starter[32].chance = 0;
  starter[32].property = 1;
  starter[32].change_money = 0;
  starter[32].rent[0] = 26;
  starter[32].rent[1] = 130;
  starter[32].rent[2] = 390;
  starter[32].rent[3] = 900;
  starter[32].rent[4] = 1100;
  starter[32].cost = 300;
  starter[32].house_cost = 200;
  starter[32].owner = -1;
  starter[32].houses_owned = 0;

  strcpy(starter[33].name,"Community Chest");
  starter[33].chance = 1;
  starter[33].property = 0;
  starter[33].change_money = 200;
  starter[33].rent[0] = 0;
  starter[33].rent[1] = 0;
  starter[33].rent[2] = 0;
  starter[33].rent[3] = 0;
  starter[33].rent[4] = 0;
  starter[33].cost = 0;
  starter[33].house_cost = 0;
  starter[33].owner = -1;
  starter[33].houses_owned = 0;

  strcpy(starter[34].name,"Oxford Street");
  starter[34].chance = 0;
  starter[34].property = 1;
  starter[34].change_money = 0;
  starter[34].rent[0] = 28;
  starter[34].rent[1] = 150;
  starter[34].rent[2] = 240;
  starter[34].rent[3] = 1000;
  starter[34].rent[4] = 1200;
  starter[34].cost = 320;
  starter[34].house_cost = 200;
  starter[34].owner = -1;
  starter[34].houses_owned = 0;

  strcpy(starter[35].name,"Liverpool St. Station");
  starter[35].chance = 0;
  starter[35].property = 1;
  starter[35].change_money = 0;
  starter[35].rent[0] = 0;
  starter[35].rent[1] = 25;
  starter[35].rent[2] = 50;
  starter[35].rent[3] = 100;
  starter[35].rent[4] = 200;
  starter[35].cost = 200;
  starter[35].house_cost = 150;
  starter[35].owner = -1;
  starter[35].houses_owned = 0;

  strcpy(starter[36].name,"Chance");
  starter[36].chance = 1;
  starter[36].property = 0;
  starter[36].change_money = 0;
  starter[36].rent[0] = 0;
  starter[36].rent[1] = 0;
  starter[36].rent[2] = 0;
  starter[36].rent[3] = 0;
  starter[36].rent[4] = 0;
  starter[36].cost = 0;
  starter[36].house_cost = 0;
  starter[36].owner = -1;
  starter[36].houses_owned = 0;

  strcpy(starter[37].name,"Park Lane");
  starter[37].chance = 0;
  starter[37].property = 1;
  starter[37].change_money = 0;
  starter[37].rent[0] = 35;
  starter[37].rent[1] = 175;
  starter[37].rent[2] = 500;
  starter[37].rent[3] = 1100;
  starter[37].rent[4] = 1300;
  starter[37].cost = 350;
  starter[37].house_cost = 200;
  starter[37].owner = -1;
  starter[37].houses_owned = 0;

  strcpy(starter[38].name,"Super Tax");
  starter[38].chance = 0;
  starter[38].property = 0;
  starter[38].change_money = -100;
  starter[38].rent[0] = 0;
  starter[38].rent[1] = 0;
  starter[38].rent[2] = 0;
  starter[38].rent[3] = 0;
  starter[38].rent[4] = 0;
  starter[38].cost = 0;
  starter[38].house_cost = 0;
  starter[38].owner = -1;
  starter[38].houses_owned = 0;

  strcpy(starter[39].name, "Mayfair");
  starter[39].chance = 0;
  starter[39].property = 1;
  starter[39].change_money = 0;
  starter[39].rent[0] = 50;
  starter[39].rent[1] = 200;
  starter[39].rent[2] = 600;
  starter[39].rent[3] = 1400;
  starter[39].rent[4] = 1700;
  starter[39].cost = 400;
  starter[39].house_cost = 200;
  starter[39].owner = -1;
  starter[39].houses_owned = 0;

  shmdt(starter);

}


//set up all the chance cards
void init_chance() {

  //open shm mem
  int mem_id = shmget(CHANCE_MEMKEY, 0, 0);
  struct chance * starter = (struct chance *) shmat(mem_id, 0, 0);

  //continue o
  //below are two chance cards
  strcpy(starter[0].text,"Advance to Go");
  starter[0].money = 0; // if it's on go it should automatically collect $200 i think
  starter[0].spaces = 0;
  strcpy(starter[1].text,"Advance to Pall Mall");
  starter[1].money = 0;
  starter[1].spaces = 24;
  strcpy(starter[2].text, "Advance to The Angel Islington");
  starter[2].money = 0;
  starter[2].spaces = 11;
  strcpy(starter[3].text,"Bank pays you dividend of $50");
  starter[3].money = 50;
  starter[3].spaces = 0;
  strcpy(starter[4].text, "Get out of Jail Free");
  starter[4].money = 0;
  starter[4].spaces = 0;
  strcpy(starter[5].text, "Go back 3 spaces.");
  starter[5].money = 0;
  starter[5].spaces = -3;
  strcpy(starter[6].text,"Go to Jail");
  starter[6].money = 0;
  starter[6].spaces = 0;
  strcpy(starter[7].text, "Pay poor tax of $15");
  starter[7].money = -15;
  starter[7].spaces = 0;
  strcpy(starter[8].text, "Take a trip to Marylebone Station");
  starter[8].money = 0;
  starter[8].spaces = 5;
  strcpy(starter[9].text,"Take a walk on Mayfair");
  starter[9].money = 0;
  starter[9].spaces = 39;
  strcpy(starter[10].text,"You have been elected Chairman of the Board - pay each player $50.");
  starter[10].money = -150;
  starter[10].spaces = 0;
  strcpy(starter[11].text,"Your building and loan matures. Collect $150.");
  starter[11].money = 150;
  starter[11].spaces = 0;
  strcpy(starter[12].text,"You have won a crossword competition.");
  starter[12].money = 0;
  starter[12].spaces = 0;
  strcpy(starter[13].text,"YOU JUST WON THE LOTTERY - AND $1000!");
  starter[13].money = 1000;
  starter[13].spaces = 0;

  shmdt(starter);
}
