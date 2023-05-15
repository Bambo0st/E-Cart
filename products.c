#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "products.h"
#include <fcntl.h>
#include <unistd.h>
int numOfProducts()
{
    int fd = open("products", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");
    lseek(fd, 0, SEEK_SET);
    int c = 0;
    Product p;
    while (read(fd, &p, sizeof(Product)))
    {
        if (strcmp(p.name, ""))
            c++;
    }
    close(fd);
    return c;
}

int addProduct(Product p)
{
    int count = 0;
    int fd = open("products", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");
    Product p1;
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    count = numOfProducts();
    count++;
    p.id = count;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &p1, sizeof(Product)))
    {
        if (!(strcmp(p.name, p1.name)) && p1.quantity > 0)
        {
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            close(fd);
            return 1;
        }
    }
    write(fd, &p, sizeof(Product));
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return 0;
}

void displayProducts(char s[])
{
    int fd = open("products", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd, 0, SEEK_SET);

    int i = 0;
    Product p1;
    strcat(s,"ProductID\tProductName\tQuantity\tPrice\n");

    while (read(fd, &p1, sizeof(Product)))
    {
        if (p1.quantity >0)
        {
            char s2[500];
            sprintf(s2, "%-10d\t%-15s\t%-8d\t%-6d\n", p1.id, p1.name, p1.quantity, p1.price);
            strcat(s,s2);
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

int deleteProduct(int id, int *nsd)
{
    int fd = open("products", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");
    Product p1;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = (id-1)*sizeof(Product);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(Product);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    int inLock=1;
    write(*nsd,&inLock,sizeof(int));
    lseek(fd, (id - 1) * sizeof(Product), SEEK_SET);
    read(fd, &p1, sizeof(Product));
    if (id == p1.id)
    {
        p1.quantity = -1;
        lseek(fd, -1 * sizeof(Product), SEEK_CUR);
        write(fd, &p1, sizeof(Product));
        int lockFlag = 0;
        read(*nsd, &lockFlag, sizeof(int));
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        return 0;
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return 1;
}

// // Function to update the quantity or price of a product in the inventory
int updateProduct(Product p , int *nsd)
{
    int flag = 0;
    Product p1;
    int fd = open("products", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (p.id-1)*sizeof(Product);
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    int inLock=1;
    write(*nsd,&inLock,sizeof(int));
    int lockFlag=1;
    read(*nsd, &lockFlag, sizeof(int));
    lseek(fd, (p.id - 1) * sizeof(Product), SEEK_SET);

    read(fd, &p1, sizeof(Product));
    if (p.id == p1.id && p1.quantity > 0)
    {
        strcpy(p.name, p1.name);
        lseek(fd, -1 * sizeof(Product), SEEK_CUR);
        write(fd, &p, sizeof(Product));
        flag = 1;
        close(fd);
        return 0;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 1;
}


int addProdToCart(int uid, int pid, int quantity)
{
    int fd = open("customer", O_RDWR | O_CREAT, 0744);
    if (fd == -1)
        perror(" ");
    Cart cart;

    lseek(fd, (uid - 1001) * sizeof(Cart), SEEK_SET);
    read(fd, &cart, sizeof(Cart));
    int i = 0;
    if (quantity <= 0)
    {
        return 3;
    }
    int fd1 = open("products", O_RDWR | O_CREAT, 0744);
    if (fd1 == -1)
        perror(" ");
    

    for (i = 0; i < cart.num; i++)
    {
        if (pid == cart.arr[i].pid)
        {
            if (cart.arr[i].quantity < 0)
                cart.arr[i].quantity = quantity;
            else
                cart.arr[i].quantity += quantity;

            //////
            int count = 0;
            Product p1;
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = (pid - 1) * sizeof(Product);
            lock.l_len = sizeof(Product);
            lock.l_pid = getpid();
            
            fcntl(fd1, F_SETLKW, &lock);
            lseek(fd1, (pid - 1) * sizeof(Product), SEEK_SET);

            int checkPID = 0;
            read(fd1, &p1, sizeof(Product));

            lock.l_type = F_UNLCK;
            fcntl(fd1, F_SETLK, &lock);

            if (pid == p1.id && p1.quantity > 0)
            {
                checkPID = 1;
            }
            if (pid == p1.id && p1.quantity < cart.arr[i].quantity)
            {
                return 1;
            }
            if (pid == p1.id && p1.quantity <= 0)
            {
                return 3;
            }

            ////
            if (checkPID)
            {
                lseek(fd, -1 * sizeof(Cart), SEEK_CUR);
                write(fd, &cart, sizeof(Cart));
                close(fd);
                if (cart.arr[i].quantity > 0)
                    return 2;
                else
                    return 0;
            }
            return 3;
        }
    }

    Product p1;

    lseek(fd1, 0, SEEK_SET);
    int chk = 0;
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (pid - 1) * sizeof(Product);
    lock.l_len = sizeof(Product);
    lock.l_pid = getpid();
    fcntl(fd1, F_SETLKW, &lock);
    lseek(fd1, (pid - 1) * sizeof(Product), SEEK_SET);
    read(fd1, &p1, sizeof(Product));
    lock.l_type = F_UNLCK;
    fcntl(fd1, F_SETLK, &lock);
    if (pid == p1.id && p1.quantity > 0)
    {
        chk = 1;
        close(fd1);
    }

    close(fd1);
    if (chk)
    {
        Item it;
        it.pid = pid;
        it.quantity = quantity;
        strcpy(it.name , p1.name);
        cart.arr[cart.num] = it;
        cart.num++;
        lseek(fd, -1 * sizeof(Cart), SEEK_CUR);

        write(fd, &cart, sizeof(Cart));
        close(fd);
        return 0;
    }
    else
        return 3;
}

void displayCartProd(int uid, char s[])
{
    int fd1 = open("customer", O_RDONLY, 0744);
    if (fd1 == -1)
        perror(" ");
    if (lseek(fd1, (uid - 1001) * sizeof(Cart), SEEK_SET) == -1)
        perror(" ");
    Cart cart;

    strcat(s,"ProductID\tProductName\tQuantity\tPrice\n");

    char s2[100];

    if (read(fd1, &cart, sizeof(Cart)) == -1)
        perror(" ");

    Item it;
    for (int i = 0; i < cart.num; i++)
    {
        char ProdName[50];
        int cost;
        it = cart.arr[i];
        //////
        if (it.quantity > 0)
        {
            Product p1;
            int fd = open("products", O_RDONLY);
            if (fd == -1)
                perror(" ");
                struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = (it.pid - 1) * sizeof(Product);
            lock.l_len = sizeof(Product);
            lock.l_pid = getpid();
            fcntl(fd, F_SETLKW, &lock);
            lseek(fd, (it.pid - 1)*sizeof(Product), SEEK_SET);
            read(fd, &p1, sizeof(Product));

            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            
            if (p1.id == it.pid)
            {
                strcpy(ProdName, p1.name);
                cost = p1.price;
            }
            close(fd);
            /////
            sprintf(s2, "%-10d\t%-15s\t%-8d\t%-6d\t\n", p1.id, p1.name, cart.arr[i].quantity, p1.price);
            strcat(s,s2);
        }
    }
    close(fd1);
}

int editProdCart(int uid, int pid, int quantity)
{
    int fd = open("customer", O_RDWR);
    if (fd == -1)
        perror(" ");
    if (lseek(fd, (uid - 1001) * sizeof(Cart), SEEK_SET) == -1)
        perror(" ");

    Cart cart;
    /////   Checking quantity
    Product p1;
    int fd1 = open("products", O_RDWR | O_CREAT, 0744);
    if (fd1 == -1)
        perror(" ");
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (pid - 1) * sizeof(Product);
    lock.l_len = sizeof(Product);
    lock.l_pid = getpid();
    fcntl(fd1, F_SETLKW, &lock);
    lseek(fd1, (pid - 1) * sizeof(Product), SEEK_SET);

    read(fd1, &p1, sizeof(Product));

    lock.l_type = F_UNLCK;
    fcntl(fd1, F_SETLK, &lock);

    if (pid == p1.id && p1.quantity < quantity)
    {
        close(fd1);
        return 1;
    }
    if (pid == p1.id && quantity < -1)
    {
        close(fd1);
        return 3;
    }

    close(fd1);
    /////
    if (read(fd, &cart, sizeof(Cart)) == -1)
        perror(" ");
    int i = 0;
    for (i = 0; i < cart.num; i++)
    {
        if (pid == cart.arr[i].pid)
        {
            cart.arr[i].quantity = quantity;
            lseek(fd, -1 * sizeof(Cart), SEEK_CUR);
            write(fd, &cart, sizeof(Cart));
            return 2;
        }
    }
    close(fd);
    return 0;
}

int paymentCart(int uid, char s[])
{
    int fd1 = open("customer", O_RDWR | O_CREAT, 0744);
    if (fd1 == -1)
        perror(" ");
    if (lseek(fd1, (uid - 1001) * sizeof(Cart), SEEK_SET) == -1)
        perror(" ");
    Cart cart;

    strcat(s,"ProductID\tProductName\tQuantity\tPrice\tTotal Cost\n");
    char s2[100];

    if (read(fd1, &cart, sizeof(Cart)) == -1)
        perror(" ");

    Item it;
    int totalCost = 0;
    for (int i = 0; i < cart.num; i++)
    {
        char ProdName[50];
        int cost;
        it = cart.arr[i];
        //////
        if (it.quantity > 0)
        {
            Product p1;
            int fd = open("products", O_RDWR | O_CREAT, 0744);
            if (fd == -1)
                perror(" ");

            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = (it.pid - 1) * sizeof(Product);
            lock.l_len = sizeof(Product);
            lock.l_pid = getpid();
            fcntl(fd, F_SETLKW, &lock);
            lseek(fd, (it.pid - 1) * sizeof(Product), SEEK_SET);
            read(fd, &p1, sizeof(Product));

            if (p1.id == it.pid && it.quantity > 0 && p1.quantity > 0)
            {
                strcpy(ProdName, p1.name);
                strcpy(cart.arr[i].name,p1.name);
                cost = p1.price;
            }
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);

            close(fd);

            sprintf(s2, "%-10d\t%-15s\t%-8d\t%-6d\t%d\n", p1.id, p1.name, cart.arr[i].quantity, p1.price,cart.arr[i].quantity*p1.price);
            strcat(s,s2);
            totalCost += cost * it.quantity;
        }
    }
    close(fd1);
    return totalCost;
}

int purchaseCart(int uid)
{
    int fd1 = open("customer", O_RDWR | O_CREAT, 0744);
    if (fd1 == -1)
        perror(" ");
    int fd3 = open("receipt", O_WRONLY | O_CREAT | O_TRUNC ,0744);
    write(fd3, "ProductID\tProductName\tQuantity\tPrice\tTotal Cost\n", 48);
    if (lseek(fd1, (uid - 1001) * sizeof(Cart), SEEK_SET) == -1)
        perror(" ");
    Cart cart;
    if (read(fd1, &cart, sizeof(Cart)) == -1)
        perror(" ");

    Item it;
    int totalCost = 0;
    char s[100]="";
    char s2[100]="";
    for (int i = 0; i < cart.num; i++)
    {
        char ProdName[50];
        int cost;
        it = cart.arr[i];
        //////
        Product p1;
        int fd = open("products", O_RDWR);
        if (fd == -1)
            perror(" ");
        struct flock lock;
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = (it.pid - 1) * sizeof(Product);
        lock.l_len = sizeof(Product);
        lock.l_pid = getpid();

        lseek(fd, (it.pid - 1) * sizeof(Product), SEEK_SET);
        read(fd, &p1, sizeof(Product));

        if (p1.id == it.pid && it.quantity>0)
        {
            if (it.quantity > p1.quantity)
                return 1;
            sprintf(s, "%-10d\t%-15s\t%-8d\t%-6d\t%d\n", p1.id, p1.name, cart.arr[i].quantity, p1.price,cart.arr[i].quantity*p1.price);
            write(fd3, s, strlen(s));
            p1.quantity -= it.quantity;
            lseek(fd, -1 * sizeof(Product), SEEK_CUR);
            write(fd, &p1, sizeof(Product));
            cart.arr[i].quantity = -1;
        }
        lock.l_type = F_UNLCK;
        fcntl(fd1, F_SETLK, &lock);
        close(fd);
    }
    lseek(fd1, -1 * sizeof(Cart), SEEK_CUR);
    write(fd1, &cart, sizeof(Cart));
    return 0;
    close(fd1);
}

void AdminReceipt()
{
    int fd = open("products", O_RDONLY);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);

    int fd2 = open("adminReceipt.txt", O_WRONLY | O_CREAT | O_TRUNC, 0744);

    write(fd2, "*************** ADMIN RECEIPT ***************:\n", 47);
    write(fd2, "ProductID\tProductName\tQuantity\tPrice\n", 37);

    lseek(fd, 0, SEEK_SET);
    struct product p1;
    while (read(fd, &p1, sizeof(struct product)))
    {
        if (p1.quantity>0)
        {
            char temp[100];
            sprintf(temp, "%-15d%-15s\t%d\t%d\n",p1.id, p1.name, p1.quantity, p1.price);
            write(fd2, temp, strlen(temp));
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
}

