typedef struct product
{
    int id;
    char name[50];
    int price;
    int quantity;
} Product;

typedef struct item
{
    int pid;
    char name[50];
    int cost;
    int quantity;
} Item;

typedef struct cart
{
    int uid;
    Item arr[50];
    int num;
} Cart;

int numOfProducts();

int addProduct(Product);

int deleteProduct(int, int *nsd);

int updateProduct(Product , int*nsd);
void displayProducts(char s[]);
int addProdToCart(int uid, int pid, int quantity);
void displayCart();
void displayCartProd(int, char s[]);
int editProdCart(int, int, int);
int paymentCart(int, char s[]);
int purchaseCart(int uid);
void AdminReceipt();