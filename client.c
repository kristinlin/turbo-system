#include "pipe_networking.h"
#include "pipe_networking.c"
#include "board.c"

int main() {

  srand(getpid());

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
  //Notifying player of their character
  if (player_num == 0) {
    printf("YOU ARE THE HAT.\n");
  } else if (player_num == 1) {
    printf("YOU ARE THE CAR.\n");
  } else if (player_num == 2) {
    printf("YOU ARE AN IRON.\n");
  } else if (player_num == 3) {
    printf("YOU ARE THE WHEELBARROW.\n");
  }


  if (buff != buff_rec) {
    printf("Error. Not connected.\n");
  }

  while (1) {
    // get update struct,
    struct update *new_update = (struct update*)malloc(sizeof(struct update));
    read(from_subserver, new_update, sizeof(struct update));

    // update money
    money += new_update->gains[player_num];
    printf("\nAccount balance: [$%d]\n", money);

    // if it's your turn;
    if (new_update->curr_player == player_num) {

      printf("\n==================================================\n");
      printf("\nIT IS YOUR TURN. MAKE YOUR MOVE NOW.\n");
      printf("\n\nAccount balance: [$%d]\n\n", money);

      // // rand int 1 - 12
      int dice = (rand() % 12) + 1;
      printf("DICE ROLLED: %d\n", dice);
      // // sets turn's curr index correctly
      new_update->position[player_num] += dice;
      new_turn->curr_index = new_update->position[player_num];
      if (new_turn->curr_index > 39) {
	       money += 200;
	       new_turn->curr_index = new_turn->curr_index % 40;
      }

      // get struct space in shm and manage sem
      gate(SPACES, ENTER);
      struct spaces * curr_space = getshm_space(new_turn->curr_index);

      gate(SPACES, LEAVE);
      printf("YOU ARE ON SPACE: %s\n", curr_space->name);

      // curr_space = spaces struct
      // new_turn = turn struct
      // new_update = update struct
      //check to see card type
      // check to see if chance card
      if (curr_space->chance == 1) {
        // add money to bank if there is money
        if (curr_space->change_money > 0) {
          printf("YOU JUST EARNED $%d FROM THE COMMUNITY CHEST!\n", curr_space->change_money);
          money += curr_space->change_money;
        }
        // draw a chance card
        struct chance * curr_chance = getshm_chance();
        printf("CHANCE CARD: YOU JUST DREW \"%s\".\n", curr_chance->text);
        // update spot on board
        if (curr_chance->spaces > 0) {
          new_turn->curr_index = curr_chance->spaces;
        } else {
          new_turn->curr_index -= curr_chance->spaces;
        }
        if (new_turn->curr_index > 39 || new_turn->curr_index < 0) {
  	       money += 200;
  	       new_turn->curr_index = new_turn->curr_index % 40;
        }
        if (curr_chance->money != 0) {
          money += curr_chance->money;
          if (money < 0) {
            printf("YOU ARE NOW BANKRUPT. GOODBYE.\n");
            new_turn->dead = 1;
          }
        }
        // paying each player $50 in special case
        if (strcmp(curr_chance->text, "You have been elected Chairman of the Board - pay each player $50.") == 0) {
          // update money
          if (money + curr_chance->money > 0) {
            int all_players;
            for (all_players = 0; all_players < 4; all_players++) {
              if (all_players != player_num) {
                new_turn->dues[all_players] += 50;
              }
            }
          }
          else {
            printf("YOU ARE NOW BANKRUPT AND OUT OF THE GAME. GOODBYE.\n");
            new_turn->dead = 1;
          }
        }
        free(curr_chance);
      }
      // no chance card, land on space w/ someone else's property
      else if (curr_space->property == 2) {
        if (curr_space->owner != player_num) {
          int money_owed = (curr_space->rent)[curr_space->houses_owned];
          printf("YOU JUST LANDED ON PLAYER %d\'S PROPERTY. PAY %d.\n", curr_space->owner, money_owed);
          if (money - money_owed >= 0) {
            //pay up (subtract money from your bank acct)
            money -= money_owed;
            // add $$ to other person's bank acct
            new_turn->dues[curr_space->owner] = money_owed;
          }
          else {
            printf("YOU ARE NOW BANKRUPT AND OUT OF THE GAME. GOODBYE.\n");
            new_turn->dead = 1;
          }
        }
        else {
          printf("YOU JUST LANDED ON YOUR OWN PROPERTY. ENJOY YOUR STAY!\n");
        }
      }
      // no chance card, land on space w/ property you can buy
      else if (curr_space->property == 1) {
          if (money - curr_space->cost < 0) {
            printf("UNFORTUNATELY YOU DON'T HAVE ENOUGH MONEY TO PURCHASE THIS PROPERTY. TRY AGAIN LATER. \n");
          }
          else {
            printf("WOULD YOU LIKE TO BUY THIS PROPERTY? THE INITIAL COST IS %d. YOU WILL EARN $%d EVERYTIME SOMEONE LANDS ON THIS SPACE. YOU CURRENTLY HAVE $%d. TYPE YES AND ANYTHING OTHER THAN \'YES\' FOR NO.\n", curr_space->cost, curr_space->rent[0], money);
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strlen(buffer) - 1] = 0;
            if (strcmp(buffer, "YES") == 0 || strcmp(buffer, "yes") == 0) {
              //pay up (subtract money from your bank acct)
              money -= curr_space->cost;
              // set new owner
	            curr_space->property = 2;
              curr_space->owner = player_num;
              //buying houses
              printf("WOULD YOU LIKE TO BUY ANY HOUSES? YOU CAN BUY UP TO FOUR HOUSES, WITH EACH HOUSE COSTING $%d. USERS WILL PAY YOU AN ADDITIONAL RENT OF $%d for 1 HOUSE, $%d for 2 HOUSES, $%d for 3 HOUSES, $%d for 4 HOUSES, INCLUDING THE ORIGINAL RENT OF %d. TYPE IN THE NUMBER OF HOUSES YOU WOULD LIKE TO BUY. ZERO IS AN ACCEPTABLE ANSWER. CHOOSE WISELY BECAUSE YOU CAN'T GO BACK. (dun dun dun) \n", curr_space->house_cost, curr_space->rent[1], curr_space->rent[2], curr_space->rent[3], curr_space->rent[4], curr_space->rent[0]);
              fgets(buffer, BUFFER_SIZE, stdin);
              buffer[strlen(buffer) - 1] = 0;
              //pay up (subtract money from your bank acct)
              if (money - (curr_space->house_cost) * atoi(buffer) < 0) {
                printf("UNFORTUNATELY YOU DID NOT CHOOSELY WISELY AND WILL NOT BE ABLE TO BUY ANY HOUSES ON THIS PROPERTY FOR THIS TURN. \n");
              }
              else {
                money -= curr_space->house_cost * atoi(buffer);
                printf("YOU NOW OWN %s HOUSES ON THIS PROPERTY.\n", buffer);
		            curr_space->houses_owned = atoi(buffer);
              }
            }
          }
      }
      // land on some weird space, income tax
      else {
        // income tax or luxury tax
        if ((strcmp(curr_space->name, "Income Tax") == 0) || (strcmp(curr_space->name, "Super Tax") == 0)) {
          printf("PAY TAX OF %d.\n", curr_space->change_money);
          if (money + curr_space->change_money < 0) {
            printf("YOU ARE NOW BANKRUPT. GOODBYE.\n");
            new_turn->dead = 1;
          }
          else {
            money += curr_space->change_money;
          }
        }

        // electric company or water works
        if ((strcmp(curr_space->name, "Water Works") == 0) || (strcmp(curr_space->name, "Electric Company") == 0)) {

          // check to see if anyone owes
          if (curr_space->owner != -1) {
            if (curr_space->owner != player_num) {
              printf("THIS SPACE IS OWNED BY PLAYER %d. PAY RENT OF %d.\n", curr_space->owner, curr_space->rent[0]);
              if (money - curr_space->rent[0] < 0) {
                printf("YOU ARE NOW BANKRUPT. GOODBYE.\n");
                new_turn->dead = 1;
              }
              else {
                // change money in player bank acct
                money -= curr_space->rent[0];
                // add money to owner account
                new_turn->dues[curr_space->owner] = curr_space->rent[0];
              }
            }
          }

          // ask player if they want to buy
          else {
            if (money - curr_space->cost < 0) {
              printf("UNFORTUNATELY YOU DO NOT HAVE ENOUGH TO BUY THIS PROPERTY RIGHT NOW. TRY AGAIN LATER.\n");
            }
            else {
              printf("WOULD YOU LIKE TO BUY THIS PROPERTY? THE INITIAL COST IS %d. YOU WILL EARN $%d EVERYTIME SOMEONE LANDS ON THIS SPACE. TYPE YES AND ANYTHING OTHER THAN \'YES\' FOR NO.\n", curr_space->cost, curr_space->rent[0]);
              fgets(buffer, BUFFER_SIZE, stdin);
              buffer[strlen(buffer) - 1] = 0;
              if (strcmp(buffer, "YES") == 0 || strcmp(buffer, "yes") == 0) {
                //pay up (subtract money from your bank acct)
                money -= curr_space->cost;
                // set new owner
		            curr_space->property = 2;
                curr_space->owner = player_num;
              }
            }
          }
        }
        // go to jail
        if (strcmp(curr_space->name, "Go to Jail") == 0) {
          printf("PAY JAIL FINE OF $500.\n");
          if (money + curr_space->change_money < 0) {
            printf("YOU ARE NOW BANKRUPT. GOODBYE.");
            new_turn->dead = 1;
          }
          else {
            // change money in player bank acct
            money += curr_space->change_money;
          }
        }
      }

      gate(SPACES, ENTER);
      setshm_space(new_turn->curr_index, curr_space);
      gate(SPACES, LEAVE);

      printf("TYPE ANYTHING TO END TURN.\n");
      //No gui bc fgets :(
      fgets(buffer, BUFFER_SIZE, stdin);
      write(to_server, new_turn, sizeof(struct turn));

      // if bankrupt free then exit
      // free all
	    //      free(new_turn);
      free(new_update);
      //      free(curr_space);
    } else {
      printf("\nIt is currently player[%d]'s turn\n", new_update->curr_player);
    }

  }

}
