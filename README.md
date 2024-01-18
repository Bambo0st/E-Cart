
# E-commerce Platform
Designed and implemented an e-commerce system enabling user product purchases, shopping cart management,
and admin product additions with user authentication using Linux programming in C. 

This uses client-server architecture where the customers are clients and the store is the server. The program uses socket programming concept for communication between client and server.

The program uses file-locking mechanism for concurrency control.

# User Menu
1. Login
2. Register

# User Usecases:
1.  Display all the products: shows all the products in inventory with its P ID, P name, P Cost, and P quantity
2.  Add a product to the cart
3.  Display the Cart
4.  Edit the cart: This gives the option to change the product quantity. If the quantity is updated to -1, then the item is removed from the cart
5.  Proceed with Payment: This allows the user to purchase the items in the cart. The user is shown all the products in the cart with the total cost and asked to pay. If they enter an amount greater than the total cost, the transaction is successful, and the quantity decreases in the product's data
6.  Exit: This quits the client program

# Admin Usecases:
1. Add a product
2. Display all the products
3. Delete a product
4. Update a product : This allows to change the price and quantity of the product , if quantity entered is -1 then
product is removed from the products data
5. Exit 












## Instructions To Run The Code

1. Open a terminal and write the following command
```bash
  gcc data.c
  ./a.out
```
2. Open 2 terminals side by side, one for server and other for client.
Terminal 1:
```bash
  gcc -o server server.c products.c
  ./server
```
Terminal 2:
```bash
  gcc -o client client.c
  ./client
```

Proceed to choose the options based on your choice, more detailed description about the options can be found in the
[Report](OS_Report.pdf).