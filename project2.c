// main file
// Project 2
/*

* Mohammad Odeh - Team leader
* Mahmoud Hamdan
* Yazeed Hamdan
* Mohammad Abu-Shmas

*/

# include "local.h"








char* trim(char *str) {
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n')) {
        str++;
    }
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n')) {
        len--;
    }
    str[len] = '\0';

    return str;
}

int randomInRange(int min_range, int max_range) {
    return (int) (min_range +  (rand() % (max_range - min_range)));
}



int main(int argc, char *argv[]){

    struct String arguments_filename ;
    struct String items_filename;

	if  ( argc != 3 ) {
        // Use the default file names
    	fprintf(stderr, "Usage: %s message\n", *argv);
        fprintf(stderr, "Using default file names: arguments.txt and items.txt\n");
        strcpy(arguments_filename.str, "arguments.txt");
        strcpy(items_filename.str, "items.txt");
    	
    }	
    else {
        // Use the file names provided by the user
        strcpy(arguments_filename.str, argv[1]);
        strcpy(items_filename.str, argv[2]);
    }
    	
    int p;
    int shelvingTeams;
    int productAmountThresh;
    int custArrivalRate;
    int simulationThresh;
    int minCustArrivalRate;
    int maxCustArrivalRate;


    srand((unsigned) getpid()); // seed for the random function with the ID of the current process


    FILE *file = fopen(arguments_filename.str, "r"); 
    if (file == NULL) {
        perror("Error opening the arguments file");
        return 1;
    }

    FILE *file2 = fopen(items_filename.str, "r"); 
    if (file == NULL) {
        perror("Error opening the items file");
        return 1;
    }
    
    char line[MAX_LINE_LENGTH];
    char varName[MAX_LINE_LENGTH];
    char valueStr[MAX_LINE_LENGTH];
    int value;

    
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Split the line into variable name and value
        sscanf(line, "%s %s", varName, valueStr);

        // Remove leading and trailing whitespaces from the variable name and value
        trim(varName);
        trim(valueStr);

        int min, max;
        // Assign values based on variable name
        if (strstr(valueStr, ",") != NULL) {
            // If the value is a range, convert it to two integers
            sscanf(valueStr, "%d,%d", &min, &max);
        } else {
            // If the value is a single number, convert it to an integer
            value = atoi(valueStr);
        }

        // Assign values based on variable name
        if (strcmp(varName, "p") == 0) {
            p = value;
        } else if (strcmp(varName, "custArrivalRate") == 0) {
            maxCustArrivalRate = max;
            minCustArrivalRate = min;
        } else if (strcmp(varName, "shelvingTeams") == 0) {
            shelvingTeams = value;
        } else if (strcmp(varName, "productAmountThresh") == 0) {
            productAmountThresh = value;
        } else if (strcmp(varName, "simulationThresh") == 0) {
          simulationThresh = value;
        } 
        
    }
    fclose(file); // closing the file
    
    
    
    
     struct Product products[100];
     char line2[MAX_LINE_LENGTH];
     int numProducts = 0;

    while (fgets(line2, sizeof(line2), file2) != NULL) {
        struct Product product;
        sscanf(line2, "%d,%s,%d,%d", &product.ID, product.Name, &product.onShelvesAmount, &product.StorageAmount);
        products[numProducts] = product;
        numProducts++;
    }

    fclose(file2);
    
     for (int i = 0; i < numProducts; i++) {
        printf("Product %d:\n", products[i].ID);
        printf("Name: %s\n", products[i].Name);
        printf("On Shelves Amount: %d\n", products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", products[i].StorageAmount);
        printf("\n");
    }
    





}
