#include "pipe_networking.h"
#include "pipe_networking.c"

int money;
typedef struct space{
  char * name;
  int chance;
  int properties[4];  
};

typedef struct chance{
  char * text;
  int money;
  int space;
};

typedef struct start{
  int index;
  int gains;
};

typedef struct turn{
  int dead;
  int curr_index;
  int dues [4];
};

int main() {

  int to_server;
  int from_subserver;
  char buffer[BUFFER_SIZE];
  struct chance Space;

  //from_subserver = client_handshake( &to_server );

  /*  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(to_server, buffer, sizeof(buffer));
    read(from_server, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
  }
  */
  printf("%d bytes large\n",sizeof() );
}
