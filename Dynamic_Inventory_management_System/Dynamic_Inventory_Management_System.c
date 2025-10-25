#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct productInformation
{
    unsigned short int productId;
    char productName[50];
    float productPrice;
    unsigned int productQuantity;

} product;

unsigned short productsCount = 0, sizeAllocated = 0;

unsigned int getIntInput(char *prompt)
{

    unsigned int value;
    char buffer[100];

    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%u", &value) == 1)
            {
                return value;
            }
            else
            {
                printf("invalid input please enter a valid integer \n");
            }
        }
        else
        {
            printf("error while reading Input\nTry again...");
        }
    }
}

float getFloatInput(char *prompt)
{

    float value;
    char buffer[100];

    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%f", &value) == 1)
            {
                return value;
            }
            else
            {
                printf("invalid input please enter a valid floating number \n");
            }
        }
        else
        {
            printf("error while reading Input\nTry again...");
        }
    }
}

unsigned short getId(product *products)
{
    unsigned short id;
    int duplicate;

    do
    {
        id = (unsigned short)getIntInput("Enter id of product: ");

        if (id < 1 || id > 10000)
        {
            printf("Please enter id in range 1-10000\n");
            continue;
        }

        duplicate = 0;
        for (int i = 0; i < productsCount; i++)
        {
            if (id == products[i].productId)
            {
                printf("ID %hu already exists. Enter new ID.\n", id);
                duplicate = 1;
                break;
            }
        }

    } while (id < 1 || id > 10000 || duplicate);

    return id;
}

void getName(char *productName)
{
    char tempProductName[50];

    while (1)
    {
        printf("Enter product name: ");

        if (fgets(tempProductName, sizeof(tempProductName), stdin) != NULL)
        {
            tempProductName[strcspn(tempProductName, "\n")] = '\0';

            if (strlen(tempProductName) > 0)
            {
                strncpy(productName, tempProductName, sizeof(tempProductName) - 1);
                productName[sizeof(tempProductName) - 1] = '\0';

                // printf("Product name taken successfully: %s\n", productName);
                return;
            }
            else
            {
                printf("Product name cannot be empty. Try again.\n");
            }
        }
        else
        {
            printf("Error reading input. Please try again.\n");

            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
    }
}

float getPrice()
{
    float price = getFloatInput("enter price of Product: ");

    while (price < 0 || price > 100000)
    {
        printf("enter price in range 0-100000\n");
        price = getFloatInput("enter price of Product: ");
    }

    return price;
}

unsigned int getQuantity()
{
    unsigned int quantity = getIntInput("Enter quantity of Product:");

    while (quantity < 0 || quantity > 1000000)
    {
        printf("enter quantity in range 0-1000000\n");
        quantity = getIntInput("Enter quantity of Product: ");
    }

    return quantity;
}

product *addNewProduct(product *products, unsigned short initialProducts)
{
    if (productsCount >= initialProducts)
    {
        product *tempProducts = (product *)realloc(products, (productsCount + 1) * sizeof(product));
        if (tempProducts == NULL)
        {
            printf("error while reallocating!\n");
            return products;
        }
        sizeAllocated++;
        products = tempProducts;
    }

    printf("enter details for product %hu:\n", productsCount + 1);
    products[productsCount].productId = getId(products);
    getName(products[productsCount].productName);
    products[productsCount].productPrice = getPrice();
    products[productsCount].productQuantity = getQuantity();

    productsCount++;

    printf("product create successful with id: %hu\n", products[productsCount - 1].productId);

    return products;
}

void printAllProducts(product* products) {
    for(int i=0; i<productsCount; i++) {
        printf("Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n",products[i].productId, products[i].productName, products[i].productPrice, products[i].productQuantity);
    }
    printf("\n");
}

void updateQuantity(product* products) {
    unsigned short id = (unsigned short)getIntInput("Enter Product ID to update quantity: "); // to be use getID()
    unsigned int quantity = getQuantity();

    for(int i=0; i<productsCount; i++) {
        if(products[i].productId == id) {
            products[i].productQuantity = quantity;
            printf("Quantity updated successfully!\n");
            return;
        }
    }

    printf("Quantity update unsuccessful, ID does not match\n");
}

void searchProductById(product *products) 
{
    unsigned short id = (unsigned short)getIntInput("Enter Product ID to search: ");
    
    for(int i=0; i<productsCount; i++) {
        if(id == products[i].productId) {
            printf("Product Found: Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n",products[i].productId, products[i].productName, products[i].productPrice, products[i].productQuantity);
            return;
        }
    }

    printf("Product Not Found for id: %hu\n",id);
}

int main()
{
    unsigned short initialProducts = 0;
    initialProducts = (unsigned short)getIntInput("enter initial number of products: ");
    
    while (initialProducts < 1 || initialProducts > 100)
    {
        printf("invalid input, please enter input between 1-100:\n");
        initialProducts = (unsigned short)getIntInput("enter initial number of products: ");
    }

    product *products = NULL;

    products = (product *)calloc(initialProducts, sizeof(product));

    if (products == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    sizeAllocated = initialProducts;

    while (1)
    {
        unsigned short choice;
        choice = (unsigned short)getIntInput("enter your choice:\n1.Add New Product\n2.View All Products\n3.Update Quantity of a Product\n4.Search for a Product by ID\n5.Search for Products by Name\n6.Search for Products by Price Range\n7.Delete a Product by ID\n8.Exit\n");

        switch (choice)
        {
        case 1:
            products = addNewProduct(products, initialProducts);
            break;

        case 2:
            printAllProducts(products);
            break;

        case 3:
            updateQuantity(products);
            break;

        case 4:
            searchProductById(products);
            break;

        case 5:

            break;

        case 6:

            break;

        case 7:

            break;

        case 8:
            exit(0);
            break;

        default:
            printf("enter correct choice\n");
            break;
        }
    }

    return 0;
}



// to be handled:
// max size of products array constraint    