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
int simulationThresh;
int minCustArrivalRate;
int maxCustArrivalRate;
int isSimulationDone = 0;

pid_t customersPids[MAX_CUSTOMERS]; // array to store the customers pids
int customersPidsIndex = 0;
pid_t teamsPids[MAX_TEAMS]; // array to store the teams pids

struct Team teams[MAX_TEAMS];
struct AllProducts allProducts;
char *shmptr_product, *shmptr_team;
int semid_product;

char tempLine[MAX_LINE_LENGTH];
char varName[MAX_LINE_LENGTH];
char valueStr[MAX_LINE_LENGTH];
int value;

int main(int argc, char *argv[]){

    // signal handler for SIGINT to call a function before exiting
    if (signal(SIGINT, exitProgram) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    
    srand((unsigned) getpid()); // seed for the random function with the ID of the current process

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

    printf("getpid() = %d\n", getpid());
    	
    // Read the arguments file
    readArgumentsFile(arguments_filename.str);
    
    // Read the items file
    readProductsFile(products_filename.str, numProducts);

    // Read the teams file
    readTeamsFile(teams_filename.str, numShelvingTeams);

    
    for (int i = 0; i < numShelvingTeams; i++) {
        printf("Team %d:\n", teams[i].team_id);
        printf("Number of Employees: %d\n", teams[i].num_employees);
        printf("\n");
    }   

    // Create a shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), "project2.c");

    // Copy the all products struct to the shared memory segment
    memcpy(shmptr_product, (char *) &allProducts, sizeof(struct AllProducts));

    // print the shared memory segment
    printSharedMemory(shmptr_product, "project2.c");

    // craete shared memory segment for the pid of the teams
    shmptr_team = (char *) malloc(sizeof(pid_t) * numShelvingTeams);
    shmptr_team = createSharedMemory(SHKEY_TEAM, sizeof(pid_t) * numShelvingTeams, "project2.c");

    
    // Create 2 semaphores one for product on shelves and one for product in storage
    semid_product = createSemaphore(SEMKEY_PRODUCT, 2, "project2.c");

    /*
    fork and exec the shelf teams processes
    */
    for (int i = 0; i < numShelvingTeams; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /*
            child process
            */

            // get the number of employees and the team ID of the current team
            int numEmployees = teams[i].num_employees;
            int teamID = teams[i].team_id;

            // Create a string array to hold the arguments for the shelfTeam process
            char *args[4];
            args[0] = "./shelvingTeam";
            args[1] = (char *) malloc(sizeof(char) * 10);
            args[2] = (char *) malloc(sizeof(char) * 10);
            args[3] = NULL;

            sprintf(args[1], "%d", teamID);
            sprintf(args[2], "%d", numEmployees);

            // exec the shelfTeam process
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else {
            teamsPids[i] = pid;
        }
    }

    for (int i = 0; i < numShelvingTeams; i++) {
        printf("Team %d pid: %d\n", i, teamsPids[i]);
    }
    // Copy the teams pids to the shared memory segment
    memcpy(shmptr_team, (char *) teamsPids, sizeof(pid_t) * numShelvingTeams);


    

    /*
    fork and exec the customer processes
    */
    while (!isSimulationDone || customersPidsIndex < MAX_CUSTOMERS ) {
        // Sleep for a random amount of time between the min and max customer arrival rates
        int custArrivalRate = randomInRange(minCustArrivalRate, maxCustArrivalRate);
        printf("customer %d comes in %d seconds\n", customersPidsIndex, custArrivalRate);
        fflush(stdout);
        sleep(custArrivalRate);

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /*
            child process
            */

            // Create a string array to hold the arguments for the customer process
            char *args[5];


            args[0] = "./customer";
            args[1] = (char *) malloc(sizeof(char) * 10);
            args[2] = (char *) malloc(sizeof(char) * 10);
            args[3] = (char *) malloc(sizeof(char) * 10);
            args[4] = NULL;
            
            sprintf(args[1], "%d", customersPidsIndex);
            sprintf(args[2], "%d", productAmountThresh);
            sprintf(args[3], "%d", numShelvingTeams);

            // exec the customer process
            execvp(args[0], args);
            perror("execvp");
            exit(2);
        } else {
            customersPids[customersPidsIndex] = pid;
            customersPidsIndex++;
        }
        
    }

    if (customersPidsIndex == MAX_CUSTOMERS) {
        printf("Maximum number of customers reached\n");
    }


}


/*
handler for SIGINT signal
this function clean up IPC resources and kill all child processes before exiting
*/
void exitProgram(int signum) {
    
    printf("\nKilling all child processes...\n");
    fflush(stdout);

    // kill all the customer processes
    killChildProcesses(customersPids, customersPidsIndex);
    killChildProcesses(teamsPids, numShelvingTeams);
    printf("All child processes killed\n");

    printf("Cleaning up IPC resources...\n");
    fflush(stdout);

    // delete the shared memory segment for the all products struct
    deleteSharedMemory(SHKEY_PRODUCT, shmptr_product);

    // delete the semaphore for the all products struct
    deleteSemaphore(semid_product);

    printf("IPC resources cleaned up successfully\n");
    printf("Exiting...\n");
    fflush(stdout);
    exit(0);
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
        team.team_id = i;
        sscanf(tempLine, "%d", &team.num_employees);
        teams[i] = team;
        i++;
    }
    fclose(file); // closing the file

}
