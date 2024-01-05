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


char tempLine[MAX_LINE_LENGTH];
char varName[MAX_LINE_LENGTH];
char valueStr[MAX_LINE_LENGTH];
int value;

int main(int argc, char *argv[]){

    struct String arguments_filename ;
    struct String items_filename;
    struct String teams_filename;

	if  ( argc != 4 ) {
        // Use the default file names
    	fprintf(stderr, "Usage: %s message\n", *argv);
        fprintf(stderr, "Using default file names: arguments.txt and products.txt and teams.txt\n");
        strcpy(arguments_filename.str, "arguments.txt");
        strcpy(items_filename.str, "products.txt");
        strcpy(teams_filename.str, "teams.txt");
    	
    }	
    else {
        // Use the file names provided by the user
        strcpy(arguments_filename.str, argv[1]);
        strcpy(items_filename.str, argv[2]);
        strcpy(teams_filename.str, argv[3]);
    }
    	
    // Open the arguments file
    FILE *file1 = fopen(arguments_filename.str, "r"); 
    if (file1 == NULL) {
        perror("Error opening the arguments file");
        return 1;
    }

    // Open the items file
    FILE *file2 = fopen(items_filename.str, "r"); 
    if (file2 == NULL) {
        perror("Error opening the products file");
        return 1;
    }

    // Open the teams file
    FILE *file3 = fopen(teams_filename.str, "r"); 
    if (file3 == NULL) {
        perror("Error opening the teams file");
        return 1;
    }
    
    // Read the arguments file
    readArgumentsFile(file1);
    
    // Read the items file
    readItemsFile(file2, numProducts);

    // Read the teams file
    readTeamsFile(file3, numShelvingTeams);

    
    // Print the values read from the files
    printf("numProducts: %d\n", numProducts);
    printf("custArrivalRate: %d-%d\n", minCustArrivalRate, maxCustArrivalRate);
    printf("numShelvingTeams: %d\n", numShelvingTeams);
    printf("productAmountThresh: %d\n", productAmountThresh);
    printf("simulationThresh: %d\n", simulationThresh);
    printf("\n");

    
    for (int i = 0; i < numProducts; i++) {
        printf("Product %d:\n", products[i].ID);
        printf("Name: %s\n", products[i].Name.str);
        printf("On Shelves Amount: %d\n", products[i].onShelvesAmount);
        printf("Storage Amount: %d\n", products[i].StorageAmount);
        printf("\n");
    }

    for (int i = 0; i < numShelvingTeams; i++) {
        printf("Team %d:\n", teams[i].team_id);
        printf("Number of Employees: %d\n", teams[i].num_employees);
        printf("\n");
    }

    // Create a shared memory segment for the shelf products
    int shmid_shelf = shmget( SHKEY_SHELF, sizeof(memory), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmid -- parent -- creation");
        exit(1);
    }

    // Attach the shared memory segment to the parent process
    if ( (shmptr = (char *) shmat(shmid, 0, 0)) == (char *) -1 ) {
        perror("shmptr -- parent -- attach");
        exit(2);
    }

    memory.numItems = itemCount;
    // Copy the array of items to the struct memory
    memcpy(memory.items, items, sizeof(items));

    // copy the memory struct to the shared memory segment
    memcpy(shmptr, (char *) &memory, sizeof(memory));
    
    
    
    


}


// function to read the arguments file
void readArgumentsFile(FILE *file) {
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

// function to read the items file and return the number of products
void readItemsFile(FILE *file, int numProducts) {
    int i = 0;
    while (fgets(tempLine, sizeof(tempLine), file) != NULL && i < numProducts) {
        struct Product product;
        product.ID = i + 1;
        sscanf(tempLine, "%[^,], %d, %d", product.Name.str, &product.onShelvesAmount, &product.StorageAmount);
        products[i] = product;
        i++;
    }
    fclose(file); // closing the file
    
}

// function to read the teams file and return the number of teams
void readTeamsFile(FILE *file, int numShelvingTeams) {
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




