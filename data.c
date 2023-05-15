#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "products.h"
typedef struct user
{
    int id;
    char password[10];
    int perm;
} user;

int main()
{
    int fd = open("userDetails", O_RDWR | O_CREAT, 0744);
    user admin;
    admin.id = 1000;
    strcpy(admin.password, "1");
    admin.perm = 1;
    write(fd, &admin, sizeof(user));

    user user1;
    user1.id = 1001;
    strcpy(user1.password, "123");
    user1.perm = 0;
    write(fd, &user1, sizeof(user));

    user1.id = 1002;
    strcpy(user1.password, "123");
    user1.perm = 0;
    write(fd, &user1, sizeof(user));
    close(fd);

    int fd1 = open("customer", O_RDWR | O_CREAT, 0744);
    Cart c;
    lseek(fd1, 0, SEEK_SET);
    c.num = 0;
    for (int i = 0; i < 100; i++)
    {
        c.uid = 1001 + i;
        Item it;
        it.pid = -1;
        it.cost = -1;
        strcpy(it.name, "");
        it.quantity = -1;
        c.arr[0] = it;

        write(fd, &c, sizeof(Cart));
    }
    close(fd1);
}