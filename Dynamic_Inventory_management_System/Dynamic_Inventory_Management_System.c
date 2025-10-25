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

unsigned short productsCount = 0;

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
        id = (unsigned short)getIntInput("Product ID: ");

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

void getName(char *productName, char *prompt)
{
    char tempProductName[50];

    while (1)
    {
        printf("%s", prompt);

        if (fgets(tempProductName, sizeof(tempProductName), stdin) != NULL)
        {
            tempProductName[strcspn(tempProductName, "\n")] = '\0';

            if (strlen(tempProductName) > 0)
            {
                strncpy(productName, tempProductName, sizeof(tempProductName) - 1);
                productName[sizeof(tempProductName) - 1] = '\0';

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
    float price = getFloatInput("Product Price: ");

    while (price < 0 || price > 100000)
    {
        printf("enter price in range 0-100000\n");
        price = getFloatInput("enter price of Product: ");
    }

    return price;
}

unsigned int getQuantity(char *prompt)
{
    unsigned int quantity = getIntInput(prompt);

    while (quantity > 1000000)
    {
        printf("enter quantity in range 0-1000000\n");
        quantity = getIntInput(prompt);
    }

    return quantity;
}

product *addNewProduct(product *products, unsigned short *initialProducts)
{
    if (products == NULL)
    {
        products = (product *)malloc(sizeof(product));
        if (products == NULL)
        {
            printf("Memory allocation failed!\n");
            return NULL;
        }
    }
    else if (productsCount >= *initialProducts)
    {
        product *tempProducts = (product *)realloc(products, (productsCount + 1) * sizeof(product));
        if (tempProducts == NULL)
        {
            printf("error while reallocating!\n");
            return products;
        }
        products = tempProducts;
    }

    printf("Enter details for product %hu:\n", productsCount + 1);
    products[productsCount].productId = getId(products);
    getName(products[productsCount].productName, "Product Name: ");
    products[productsCount].productPrice = getPrice();
    products[productsCount].productQuantity = getQuantity("Product Quantity: ");

    productsCount++;

    printf("product create successful with id: %hu\n", products[productsCount - 1].productId);

    printf("\n");

    return products;
}

void printAllProducts(product *products)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return;
    }

    printf("========= PRODUCT LIST =========\n");

    for (int i = 0; i < productsCount; i++)
    {
        printf("Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n", products[i].productId, products[i].productName, products[i].productPrice, products[i].productQuantity);
    }
    printf("\n");
}

void updateQuantity(product *products)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return;
    }

    unsigned short id = (unsigned short)getIntInput("Enter Product ID to update quantity: ");
    unsigned int quantity = getQuantity("Enter new Quantity: ");
    
    for (int i = 0; i < productsCount; i++)
    {
        if (products[i].productId == id)
        {
            products[i].productQuantity = quantity;
            printf("Quantity updated successfully!\n\n");
            return;
        }
    }

    printf("Quantity update unsuccessful, ID does not match\n\n");
}

void searchProductById(product *products)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return;
    }

    unsigned short id = (unsigned short)getIntInput("Enter Product ID to search: ");

    for (int i = 0; i < productsCount; i++)
    {
        if (id == products[i].productId)
        {
            printf("Product Found: Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n\n", products[i].productId, products[i].productName, products[i].productPrice, products[i].productQuantity);
            return;
        }
    }

    printf("Product Not Found for id: %hu\n\n", id);
}

void searchProductByName(product *products)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return;
    }

    char productNameToSearch[50];
    getName(productNameToSearch, "Enter name to search (partial allowed): ");

    strlwr(productNameToSearch);
    printf("Products Found:\n");

    int found = 0;
    for (int i = 0; i < productsCount; i++)
    {
        char productName[50];
        strcpy(productName, products[i].productName);
        strlwr(productName);

        if (strstr(productName, productNameToSearch) != NULL)
        {
            printf("Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n",
                   products[i].productId,
                   products[i].productName,
                   products[i].productPrice,
                   products[i].productQuantity);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No products found with that name.\n");
    }
}

void searchProductByPriceRange(product *products)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return;
    }

    float minimumPrice = getFloatInput("Enter minimum price: ");
    float maximumPrice = getFloatInput("Enter maximum price: ");

    printf("\nProducts in price range: \n");
    for (int i = 0; i < productsCount; i++)
    {
        if (products[i].productPrice >= minimumPrice && products[i].productPrice <= maximumPrice)
        {
            printf("Product ID: %hu | Name: %s | Price: %.2f | Quantity: %u\n\n", products[i].productId, products[i].productName, products[i].productPrice, products[i].productQuantity);
        }
    }
}

product *deleteProductById(product *products, unsigned short *initialProducts)
{
    if (products == NULL || productsCount == 0)
    {
        printf("No products available.\n\n");
        return NULL;
    }

    unsigned short idToDelete = getIntInput("Enter Product ID to delete: ");

    for (int i = 0; i < productsCount; i++)
    {
        if (idToDelete == products[i].productId)
        {
            for (int j = i; j < productsCount - 1; j++)
            {
                products[j] = products[j + 1];
            }

            productsCount--;

            if (productsCount == 0)
            {
                free(products);
                products = NULL;
                productsCount = 0;
                *initialProducts = 0;
                printf("Last product deleted. Array is now empty.\n\n");
                return products;
            }

            product *tempProducts = (product *)realloc(products, (productsCount) * sizeof(product));

            if (tempProducts == NULL)
            {
                productsCount++;
                printf("products Array reallocation falied!\n");
                return products;
            }

            products = tempProducts;
            printf("Product deleted successfully!\n\n");
            return products;
        }
    }

    printf("Error while deleting product\n\n");
    return products;
}

int main()
{
    unsigned short initialProducts = 0;
    initialProducts = (unsigned short)getIntInput("Enter initial number of products: ");

    while (initialProducts < 1 || initialProducts > 100)
    {
        printf("invalid input, please enter input between 1-100\n\n");
        initialProducts = (unsigned short)getIntInput("enter initial number of products: ");
        printf("\n");
    }
    printf("\n");

    product *products = NULL;

    products = (product *)calloc(initialProducts, sizeof(product));

    if (products == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < initialProducts; i++)
    {
        addNewProduct(products, &initialProducts);
    }

    while (1)
    {
        unsigned short choice;
        choice = (unsigned short)getIntInput("========= INVENTORY MENU =========\n1.Add New Product\n2.View All Products\n3.Update Quantity of a Product\n4.Search for a Product by ID\n5.Search for Products by Name\n6.Search for Products by Price Range\n7.Delete a Product by ID\n8.Exit\nEnter your choice: ");
        printf("\n");

        switch (choice)
        {
        case 1:
            products = addNewProduct(products, &initialProducts);
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
            searchProductByName(products);
            break;

        case 6:
            searchProductByPriceRange(products);
            break;

        case 7:
            products = deleteProductById(products, &initialProducts);
            break;

        case 8:
            free(products);
            printf("Memory released successfully. Exiting program...");
            exit(0);
            break;

        default:
            printf("enter correct choice\n");
            break;
        }
    }

    return 0;
}