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
#include <pthread.h>
#include <ctype.h>
#include "server_file.h"
#include "products.h"
int checkUser(user c)
{
    int i = c.id - 1000;
    int fd = open("userDetails", O_RDONLY, 0744);
    lseek(fd, 0, SEEK_SET);
    int res;
    user t;

    int l1;
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = i * sizeof(user);
    lock.l_len = sizeof(user);
    lock.l_pid = getpid();
    l1 = fcntl(fd, F_SETLKW, &lock);
    lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
    read(fd, &t, sizeof(user));
    if (!strcmp(t.password, c.password) && t.id == c.id)
    {
        res = 1;
        if (t.perm == 1)
            res = 2;
    }
    else
        res = 0;

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);
    return res;
}


int addUser(char s[],int ap)
{
    int fd = open("userDetails", O_RDWR, 0744);
    lseek(fd, 0, SEEK_SET);
    int res;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    user us;
    int c = 0;
    user p;
    lseek(fd,0,SEEK_SET);
    while (read(fd, &p, sizeof(user)))
    {
        c++;
    }
    int i = c;
    lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
    us.id = 1000+i;
    strcpy(us.password , s);
    us.perm= ap;
    write(fd,&us,sizeof(user));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);
    return us.id;
}
void EstablishConn(int *nsd)
{
    int option;
    int flg = -1;
    while (1)
    {
        read(*nsd, &option, sizeof(option));
        if (option == 1)
        {
            user user1;
            read(*nsd, &user1, sizeof(user));
            int result = checkUser(user1);
            flg = result;
            write(*nsd, &result, sizeof(result));
            if(flg!=0)
                break;
        }
        else if(option == 2)
        {
            char s[10]="";
            read(*nsd,s,sizeof(s));
            int ap=0;
            read(*nsd,&ap,sizeof(int));
            int id = addUser(s,ap);
            write(*nsd,&id,sizeof(int));

        }
    }
    if (flg == 2) // admin
    {
        while (1)
        {
            read(*nsd, &option, sizeof(option));
            if (option == 1)
            {
                Product p;
                read(*nsd, &p, sizeof(p));
                int res = addProduct(p);
                write(*nsd, &res, sizeof(res));
                // break;
            }
            if (option == 2)
            {
                char s[1000] = "";
                displayProducts(s);
                write(*nsd, s, sizeof(s));
                // break;
            }
            if (option == 3) // delete
            {
                int pid;
                read(*nsd, &pid, sizeof(int));
                int res = deleteProduct(pid,nsd);
                write(*nsd, &res, sizeof(int));
            }
            if (option == 4)
            {
                Product p;
                read(*nsd, &p, sizeof(p));
                int res = updateProduct(p,nsd);
                write(*nsd, &res, sizeof(int));
            }
            if(option == 5)
            {
                AdminReceipt();
            }
        }
    }
    if (flg == 1) // user
    {
        while (1)
        {
            read(*nsd, &option, sizeof(option));
            if (option == 1) // Display
            {
                char s[1000] = "";
                displayProducts(s);
                write(*nsd, s, sizeof(s));
            }
            else if (option == 2)
            {
                int pid, uid, quantity;
                read(*nsd, &uid, sizeof(int));
                read(*nsd, &pid, sizeof(int));
                read(*nsd, &quantity, sizeof(int));
                int res = addProdToCart(uid, pid, quantity);
                write(*nsd, &res, sizeof(int));
            }
            else if (option == 3)
            {
                int uid;
                read(*nsd, &uid, sizeof(int));
                char s[1000] = "";
                displayCartProd(uid, s);
                write(*nsd, s, 1000);
            }
            else if (option == 4)
            {
                int uid, pid, quantity;
                read(*nsd, &uid, sizeof(int));
                read(*nsd, &pid, sizeof(int));
                read(*nsd, &quantity, sizeof(int));
                int res = editProdCart(uid, pid, quantity);
                write(*nsd, &res, sizeof(int));
            }
            else if (option == 5)
            {
                int uid;
                char s[1000] = "";
                read(*nsd, &uid, sizeof(int));
                int p = paymentCart(uid, s);
                write(*nsd, s, sizeof(s));
                write(*nsd, &p, sizeof(int));
                int res;
                read(*nsd, &res, sizeof(int));
                int stat = 0;
                if (res == 1)
                    stat = purchaseCart(uid);
                write(*nsd, &stat, sizeof(int));
            }
        }
    }
}

int main()
{
    int sd;
    struct sockaddr_in server, client;
    char buf[300];
    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    server.sin_family = AF_UNIX;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htonl(5001);

    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("BIND ERROR");
        exit(EXIT_FAILURE);
    }
    if (listen(sd, 5) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    int nsd;

    while (1)
    {
        int sz = sizeof(client);
        if ((nsd = accept(sd, (struct sockaddr *)(&client), &sz)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (!fork())
        {
            close(sd);
            EstablishConn(&nsd);
            exit(0);
        }
        else
        {
            close(nsd);
        }
    }
    close(sd);
}
