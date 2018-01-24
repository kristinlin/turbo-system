#include "pipe_networking.h"
#include "pipe_networking.c"
#include "board.h"
#define SEMKEY 1023
#define MEMKEY 1123

int money;
/** struct space{
  char * name;
  int chance;
  int properties[4];
};

struct chance{
  char * text;
  int money;
  int space;
};

struct start{
  int index;
  int gains;
};

struct turn{
  int dead;
  int curr_index;
  int dues [4];
}; **/

//ignore this, carry on
/*
int semview() {
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

  int to_server;
  int from_subserver;
  char buffer[BUFFER_SIZE];
  struct chance Space;

  from_subserver = client_handshake( &to_server );

  //full circle handshake. client->server->subserver->REPEAT
  int buff = getpid();
  int buff_rec;
  printf("Waiting for more players");
  printf("[player] writing: %d\n", buff);
  write(to_server, &buff, sizeof(buff));
  read(from_subserver, &buff_rec, sizeof(buff_rec));
  printf("[player] received: %d\n", buff_rec);

  if (buff != buff_rec) {
    printf("Error. Not connected.\n");
  }

  struct game new_turn = *getshm();

  //printf("%d bytes large\n",sizeof() );
}
