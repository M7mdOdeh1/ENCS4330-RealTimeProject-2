/*

* Mohammed Owda - Team leader
* Mahmoud Hamdan
* Yazeed Hamdan
* Mohammad Abu-Shmas

*/

# include "local.h"

int numProducts;
int numShelvingTeams;
int productAmountThresh;
int custArrivalRate;
int simulationThresh;
int minCustArrivalRate;
int maxCustArrivalRate;
struct Product products[100];
struct Team teams[MAX_TEAMS];
struct AllProducts allProducts;
char *shmptr_product;



char tempLine[MAX_LINE_LENGTH];
char varName[MAX_LINE_LENGTH];
char valueStr[MAX_LINE_LENGTH];
int value;

int main(int argc, char *argv[]){

    struct String arguments_filename ;
    struct String products_filename;
    struct String teams_filename;

	if  ( argc != 4 ) {
        // Use the default file names
    	fprintf(stderr, "Usage: %s message\n", *argv);
        fprintf(stderr, "Using default file names: arguments.txt and products.txt and teams.txt\n");
        strcpy(arguments_filename.str, "arguments.txt");
        strcpy(products_filename.str, "products.txt");
        strcpy(teams_filename.str, "teams.txt");
    	
    }	
    else {
        // Use the file names provided by the user
        strcpy(arguments_filename.str, argv[1]);
        strcpy(products_filename.str, argv[2]);
        strcpy(teams_filename.str, argv[3]);
    }
    	
    // Read the arguments file
    readArgumentsFile(arguments_filename.str);
    
    // Read the items file
    readProductsFile(products_filename.str, numProducts);

    // Read the teams file
    readTeamsFile(teams_filename.str, numShelvingTeams);

    
    // Print the values read from the files
    printf("numProducts: %d\n", numProducts);
    printf("custArrivalRate: %d-%d\n", minCustArrivalRate, maxCustArrivalRate);
    printf("numShelvingTeams: %d\n", numShelvingTeams);
    printf("productAmountThresh: %d\n", productAmountThresh);
    printf("simulationThresh: %d\n", simulationThresh);
    printf("\n");

    
    for (int i = 0; i < allProducts.numProducts; i++) {
        printf("Product %d:\n", allProducts.products[i].ID);
        printf("Name: %s\n", allProducts.products[i].Name.str);
        printf("On Shelves Amount: %d\n", allProducts.products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", allProducts.products[i].StorageAmount);
        printf("\n");
    }

    for (int i = 0; i < numShelvingTeams; i++) {
        printf("Team %d:\n", teams[i].team_id);
        printf("Number of Employees: %d\n", teams[i].num_employees);
        printf("\n");
    }

    createSharedMemory(SHKEY_PRODUCT, shmptr_product);

    // Copy the all products struct to the shared memory segment
    memcpy(shmptr_product, (char *) &allProducts, sizeof(struct AllProducts));

    //print the shared memory segment
    printf("Shared memory segment:\n");
    for (int i = 0; i < numProducts; i++) {
        printf("Product %d:\n", ((struct AllProducts *) shmptr_product)->products[i].ID);
        printf("Name: %s\n", ((struct AllProducts *) shmptr_product)->products[i].Name.str);
        printf("On Shelves Amount: %d\n", ((struct AllProducts *) shmptr_product)->products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", ((struct AllProducts *) shmptr_product)->products[i].StorageAmount);
        printf("\n");
    }
    
    
    


}

// function to create shared memory segment and attach it to the parent process
void createSharedMemory(int key, char *shmptr) {
    // Create a shared memory segment for the all products struct
    int shmid = shmget( key, sizeof(allProducts), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmid -- parent -- creation");
        exit(1);
    }
   
    // Attach the shared memory segment to the parent process
    if ( (shmptr_product = (char *) shmat(shmid, 0, 0)) == (char *) -1 ) {
        perror("shmat -- parent -- attach");
        exit(2);
    }

    
    


    
}


// function to read the arguments file
void readArgumentsFile(char *arguments_filename) {

    // Open the arguments file
    FILE *file = fopen(arguments_filename, "r"); 
    if (file == NULL) {
        perror("Error opening the arguments file");
        exit(1);
    }

    while (fgets(tempLine, sizeof(tempLine), file) != NULL) {
        // Split the line into variable name and value
        sscanf(tempLine, "%s %s", varName, valueStr);

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
        if (strcmp(varName, "numProducts") == 0) {
            numProducts = value;
        } else if (strcmp(varName, "custArrivalRate") == 0) {
            maxCustArrivalRate = max;
            minCustArrivalRate = min;
        } else if (strcmp(varName, "numShelvingTeams") == 0) {
            numShelvingTeams = value;
        } else if (strcmp(varName, "productAmountThresh") == 0) {
            productAmountThresh = value;
        } else if (strcmp(varName, "simulationThresh") == 0) {
          simulationThresh = value;
        } 
        
    }
    fclose(file); // closing the file
}

// function to read the products file and return the number of products
void readProductsFile(char *items_filename, int numProducts) {
    // Open the products file
    FILE *file = fopen(items_filename, "r"); 
    if (file == NULL) {
        perror("Error opening the products file");
        exit(1);
    }

    int i = 0;
    while (fgets(tempLine, sizeof(tempLine), file) != NULL && i < numProducts) {
        struct Product product;
        product.ID = i + 1;
        sscanf(tempLine, "%[^,], %d, %d", product.Name.str, &product.onShelvesAmount, &product.StorageAmount);
        allProducts.products[i] = product;
        i++;
    }
    allProducts.numProducts = i;

    fclose(file); // closing the file
    
}

// function to read the teams file and return the number of teams
void readTeamsFile(char *teams_filename, int numShelvingTeams) {

    // Open the teams file
    FILE *file = fopen(teams_filename, "r"); 
    if (file == NULL) {
        perror("Error opening the teams file");
        exit(1);
    }

    int i = 0;

    while (fgets(tempLine, sizeof(tempLine), file) != NULL && i < numShelvingTeams) {
        struct Team team;
        team.team_id = i + 1;
        sscanf(tempLine, "%d", &team.num_employees);
        teams[i] = team;
        i++;
    }
    fclose(file); // closing the file

}




