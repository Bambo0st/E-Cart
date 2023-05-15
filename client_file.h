#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>

typedef struct user
{
    int id;
    char password[10];
    int perm;
} user;

int LoginUser(int sd, int uid);
void ContactServer(int sd);
void addProd(int sd);
void displayProd(int sd);
void updateProd(int sd);
void deleteProd(int sd);
void AddUser(int sd);
void addToCart(int sd, int uid);
void editCart(int sd, int uid);
void payment(int sd, int uid);