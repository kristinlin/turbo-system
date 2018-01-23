#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#ifndef NETWORKING_H
#define NETWORKING_H

#define ACK "HOLA"
#define PIPE_NAME "monopoly"
#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000
#define READ 0
#define WRITE 1
#define KEY 1111

int server_handshake(int *to_client);
int client_handshake(int *to_server);
int server_setup();
int server_connect(int from_client);

#endif
