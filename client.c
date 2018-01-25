#include "pipe_networking.h"
#include "pipe_networking.c"
#include "board.h"
#define SEMKEY 1023
#define MEMKEY 1123

/*int semview() {
  int semval = semget(KEY, 1, 0600);
  int semcut = semctl(semval, 0, GETVAL);
  printf("%d\n",semcut);
  if (semcut<0)
  {
    printf("You need to create a semaphore first\n");
    return 0;
  }
  else
  {
    printf("The semaphore value is: %d\n",semcut);
    return semcut;
  }
  //return semval.data;
  //return data.val+0;
}
*/


int main() {

  int player_num;
  int money = 0;
  int to_server;
  int from_subserver;
  char buffer[BUFFER_SIZE];
  struct turn * new_turn;
  new_turn = (struct turn *)malloc(sizeof(struct turn));

  from_subserver = client_handshake( &to_server );

  //full circle handshake. client->server->subserver->REPEAT
  int buff = getpid();
  int buff_rec;
  printf("Waiting for more players");
  printf("[player] writing: %d\n", buff);
  write(to_server, &buff, sizeof(buff));
  read(from_subserver, &buff_rec, sizeof(buff_rec));
  printf("[player] received: %d\n", buff_rec);

  //get official player number
  read(from_subserver, &player_num, sizeof(int));
  printf("====================================================\n");
  printf("WELCOME TO MONOPOLY.\n");
  printf("You are officially player [%d]\n", player_num);
	

  if (buff != buff_rec) {
    printf("Error. Not connected.\n");
  }

  while (1) {
  // get update struct,
  struct update *new_update = (struct update*)malloc(sizeof(struct update));
  read(from_subserver, new_update, sizeof(struct update));

  // update money
  money += new_update->gains[player_num];
  printf("This is your account balance [%d]\n", money);

  // if it's your turn;
  if (new_update->curr_player == player_num) {

    printf("\n\nIT IS YOUR TURN. MAKE YOUR MOVE NOW.\n");
    
    // // rand int 1 - 12

    int dice = rand() % 12 + 1;
    // // sets turn's curr index correctly
    new_turn->curr_index = new_update->position[player_num] + dice;
    if (new_turn->curr_index > 39) {
      money += 200;
      new_turn->curr_index = new_turn->curr_index % 40;
    }   
    // // get struct space
    printf("YOU ARE ON SPACE: %s\n", getshm_space(dice)->name);
    // // check if chance card
    // // // look at contents
    // // check if rent
    // // // update client money; etc. 
    // // ask user if they want property (if available)
    // // // change money
    // // // change struct space
    // // write turn back
    // // if dead, exit now

    
  } else {
    printf("\nIt is currently player[%d]'s turn\n", new_update->curr_player);
  }
  

  
  }

}
