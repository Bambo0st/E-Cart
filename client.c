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
#include "client_file.h"
#include "products.h" 

int uid;
void ContactServer(int sd)
{
    int flg = -1;
    while (1)
    {
        printf("Choose one of the following\n");
        printf("1. Login \n");
        printf("2. Register\n");
        int choice;
        scanf("%d", &choice);
        if (choice == 1)
        {
            write(sd, &choice, sizeof(int));
            printf("Enter the userid\n");
            scanf("%d", &uid);
            flg = LoginUser(sd, uid);
            if (flg != 0)
                break;
        }
        else if(choice==2)
        {
            write(sd,&choice,sizeof(int));
            AddUser(sd);
            // exit(0);
        }
    }
    while (1)
    {
        if (flg == 1) // user
        {
            printf("Choose one of the following\n");
            printf("1. Display all the products\n");
            printf("2. Add a product to cart\n");
            printf("3. Display the Cart\n");
            printf("4. Edit the cart \n");
            printf("5. Proceed with Payment\n");
            printf("6. Exit \n");
            int choice;
            scanf("%d", &choice);
            if (choice == 1)
            {
                write(sd, &choice, sizeof(int));
                displayProd(sd);
                // break;
            }
            if (choice == 2)
            {
                write(sd, &choice, sizeof(int));
                addToCart(sd, uid);
            }
            if (choice == 3)
            {
                write(sd, &choice, sizeof(int));
                displayCart(sd, uid);
            }
            if (choice == 4)
            {
                write(sd, &choice, sizeof(int));
                editCart(sd, uid);
            }
            if (choice == 5)
            {
                write(sd, &choice, sizeof(int));
                payment(sd, uid);

            }
            if(choice==6)
            {
                exit(0);
            }
        }
        else if (flg == 2) // admin
        {
            printf("Choose one of the following\n");
            printf("1. Add a product \n");
            printf("2. Display the products\n");
            printf("3. Delete a product\n");
            printf("4. Update a product\n");
            printf("5. Exit\n");
            int choice = 0;
            scanf("%d", &choice);

            if (choice == 1)
            {
                write(sd, &choice, sizeof(int));
                addProd(sd);
            }
            if (choice == 2)
            {
                write(sd, &choice, sizeof(int));
                displayProd(sd);
                // break;
            }
            if (choice == 3)
            {
                write(sd, &choice, sizeof(int));
                deleteProd(sd);
            }
            if (choice == 4)
            {
                write(sd, &choice, sizeof(int));
                updateProd(sd);
                // break;
            }
            if(choice == 5)
            {
                write(sd,&choice , sizeof(int));
                exit(0);
            }
        }
    }
}
void AddUser(int sd)
{
    printf("Enter the password\n");
    char s[10]="";
    scanf("%s",s);
    write(sd,s,sizeof(s));
    printf("Please choose access permissions\n");
    printf("Enter 1 for admin, 0 for user\n");
    int ap = 0;
    scanf("%d",&ap);
    write(sd,&ap,sizeof(int));
    int id=0;
    read(sd,&id,sizeof(int));
    printf("User added succesfully\n");
    printf("Your user id is %d\n",id);
}
int LoginUser(int sd, int uid)
{
    int userid;
    user user1;
    user1.id = uid;
    printf("Enter the password\n");
    scanf("%s", user1.password);
    write(sd, &user1, sizeof(user));

    int res;
    read(sd, &res, sizeof(res)); // taking return value from server

    if (!res)
    {
        printf("Invalid Login\n");
        ContactServer(sd);
    }
    else if (res == 1)
    {
        printf("Successfully logged in\n");
    }
    else
        printf("You are logged in as ADMIN\n");
    return res;
}

void addProd(int sd)
{
    Product p;
    printf("Enter the product name: \n");
    scanf("%s", p.name);
    printf("Enter the product price: \n");
    scanf("%d", &p.price);
    printf("Enter the product quantity: \n");
    scanf("%d", &p.quantity);

    write(sd, &p, sizeof(Product));
    int res;
    read(sd, &res, sizeof(res));

    if (res == 1)
    {
        printf("The product was already there , so product cannot be added \n");
    }
    else
        printf("The product was added succesfully\n");
}

void displayProd(int sd)
{
    char s[1000];
    read(sd, &s, sizeof(s));
    printf("%s\n", s);
}

void updateProd(int sd)
{
    printf("Enter the P_ID of the product which you want to update\n");
    Product p;
    scanf("%d", &p.id);
    printf("Enter the updated price  of the product\n ");
    scanf("%d", &p.price);
    printf("Enter the updated quantity of the product\n");
    scanf("%d", &p.quantity);
    write(sd, &p, sizeof(p));
    int lockCheck;
    read(sd,&lockCheck,sizeof(int));
    printf("Enter 1  to unlock\n");
    int lockFlag = 0;
    scanf("%d", &lockFlag);
    write(sd, &lockFlag, sizeof(int));
    int res;
    read(sd, &res, sizeof(int));
    if (res)
        printf("The P_ID entered doesn't exist\n");
    else
        printf("The product was updated successfully\n");
}

void deleteProd(int sd)
{
    printf("Enter the P_ID of the product which you want to delete\n");
    Product p;
    scanf("%d", &p.id);
    write(sd, &p.id, sizeof(int));
    int lockCheck;
    read(sd,&lockCheck,sizeof(int));
    printf("Enter 1  to unlock\n");
    int lockFlag = 0;
    scanf("%d", &lockFlag);
    write(sd, &lockFlag, sizeof(int));
    int res;
    read(sd, &res, sizeof(int));
    if (!res)
        printf("The item was deleted succesfully\n");
    else
        printf("The P_ID entered doesn't exist");
}

void addToCart(int sd, int uid)
{
    printf("Enter the pid of the product which you want to add\n");
    int pid, quantity;
    scanf("%d", &pid);
    printf("Enter the quantity you want to add\n");
    scanf("%d", &quantity);
    write(sd, &uid, sizeof(int));
    write(sd, &pid, sizeof(int));
    write(sd, &quantity, sizeof(int));
    int res;
    read(sd, &res, sizeof(int));
    if (res == 3)
        printf("Incorrect PID or quantity entered\n");
    else if (res == 1)
        printf("Error ! The quantity entered is more than available in store\n");
    else
        printf("The product was added succesfully to the cart\n");
}

void displayCart(int sd, int uid)
{
    write(sd, &uid, sizeof(int));
    char s[1000] = "";
    read(sd, &s, sizeof(s));
    printf("%s", s);
}

void editCart(int sd, int uid)
{
    printf("Enter the product ID you want to edit\n");
    int pid;
    scanf("%d", &pid);
    printf("Enter the updated quantity\n");
    int quantity;
    scanf("%d", &quantity);
    write(sd, &uid, sizeof(int));
    write(sd, &pid, sizeof(int));
    write(sd, &quantity, sizeof(int));
    int res;
    read(sd, &res, sizeof(int));
    if (res == 0)
        printf("The entered pid is not in cart\n");
    if (res == 1)
        printf("The entered quantity exceeds the quantity present in the store\n");
    if (res == 2)
        printf("The product was successfully edited in the cart\n");
    if (res == 3)
        printf("Incorrect value for quantity entered\n");
}

void payment(int sd, int uid)
{
    write(sd, &uid, sizeof(int));
    char s[1000] = "";
    read(sd, &s, sizeof(s));
    int totalCost = 0;
    read(sd, &totalCost, sizeof(int));
    printf("%s\n", s);
    printf("The total cost is %d\n\n", totalCost);
    printf("Enter the amount\n");
    int amount;
    scanf("%d", &amount);
    int res = 0;
    if (amount == totalCost)
        res = 1;
    write(sd, &res, sizeof(int));
    int stat = 0;
    read(sd, &stat, sizeof(int));
    if (stat == 0 && res == 1)
        printf("The payment was successful \n");
    else if (res == 0)
        printf("The payment was not successfull\n");
    else if (stat == 1)
        printf("The payment was not successful because items in cart are out of stock\n");
}

int main()
{

    int sd;
    struct sockaddr_in server;

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    // server.sin_addr.s_addr = inet_addr("192.168.0.1");
    server.sin_addr.s_addr = INADDR_ANY;

    server.sin_family = AF_UNIX;
    server.sin_port = htonl(5001);

    if (connect(sd, (struct sockaddr *)(&server), sizeof(server)) == -1)
    {
        perror("Failed to connect\nExiting");
        exit(EXIT_FAILURE);
    }

    char req[300];
    char res[300];
    ContactServer(sd);
    close(sd);
}
