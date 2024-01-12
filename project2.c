/*

* Mohammed Owda - Team leader
* Mahmoud Hamdan
* Yazeed Hamdan
* Mohammad Abu-Shmas

*/

# include "local.h"
# include "ipcs.h"
# include "gui.h"

int numProducts;
int numShelvingTeams;
int productAmountThresh;
int simulationThresh;
int minCustArrivalRate;
int maxCustArrivalRate;
int managerTimeToFillCart;
int employeeFillTime;
int custBuyTimeMin;
int custBuyTimeMax;
int isSimulationDone = 0;

pid_t customersPids[MAX_CUSTOMERS]; // array to store the customers pids
int customersPidsIndex = 0;
pid_t teamsPids[MAX_TEAMS]; // array to store the teams pids

struct Team teams[MAX_TEAMS];
struct AllProducts allProducts;
char *shmptr_product, *shmptr_team;
int semid_product, msgqid_team, msgqid_gui;

char tempLine[MAX_LINE_LENGTH];
char varName[MAX_LINE_LENGTH];
char valueStr[MAX_LINE_LENGTH];
int value;

int main(int argc, char *argv[]){
    
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
    	
    // Read the arguments file
    readArgumentsFile(arguments_filename.str);
    
    // Read the items file
    readProductsFile(products_filename.str, numProducts);

    // Read the teams file
    readTeamsFile(teams_filename.str, numShelvingTeams);

    // initialize the signal handlers (SIGINT, SIGUSR1, SIGALRM)
    initializeSignalHandlers();
    
    // set the alarm to the simulation threshold
    alarm(simulationThresh*60);
    
    // initialize IPCs resources (shared memory, semaphores, message queues)
    initializeIPCResources();

    /*
    fork and exec the gui process
    */
    pid_t pid_gui = fork();
    if (pid_gui == -1) {
        perror("fork");
        exit(1);
    } else if (pid_gui == 0) {
        /*
        child process
        */
        // exec the gui process

        char *args[3];
        args[0] = "./gui";
        args[1] = (char *) malloc(sizeof(char) * 10);
        args[2] = NULL;
        execvp(args[0], args);
        sprintf(args[1], "%d", productAmountThresh);

        perror("execvp -- gui");

        exit(1);
    } else {
        // parent process
        printf("GUI process created\n");
        fflush(stdout);
    }
    
    /*
    fork and exec the shelf teams processes
    */
    for (int i = 0; i < numShelvingTeams; i++) {
        pid_t pid_team = fork();
        if (pid_team == -1) {
            perror("fork");
            exit(1);
        } else if (pid_team == 0) {
            /*
            child process
            */

            // get the number of employees and the team ID of the current team
            int numEmployees = teams[i].num_employees;
            int teamID = teams[i].team_id;

            // Create a string array to hold the arguments for the shelfTeam process
            char *args[7];
            args[0] = "./shelvingTeam";
            args[1] = (char *) malloc(sizeof(char) * 10);
            args[2] = (char *) malloc(sizeof(char) * 10);
            args[3] = (char *) malloc(sizeof(char) * 10);
            args[4] = (char *) malloc(sizeof(char) * 10);
            args[5] = (char *) malloc(sizeof(char) * 10);
            args[6] = NULL;

            sprintf(args[1], "%d", teamID);
            sprintf(args[2], "%d", numEmployees);
            sprintf(args[3], "%d", productAmountThresh);
            sprintf(args[4], "%d", managerTimeToFillCart);
            sprintf(args[5], "%d", employeeFillTime);

            // exec the shelfTeam process
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else {
            teamsPids[i] = pid_team;
        }
    }


    /*
    fork and exec the customer processes
    */
    while ( !isSimulationDone && customersPidsIndex < MAX_CUSTOMERS) {
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

            int buyTime = randomInRange(custBuyTimeMin, custBuyTimeMax);

            // Create a string array to hold the arguments for the customer process
            char *args[6];

            args[0] = "./customer";
            args[1] = (char *) malloc(sizeof(char) * 10);
            args[2] = (char *) malloc(sizeof(char) * 10);
            args[3] = (char *) malloc(sizeof(char) * 10);
            args[4] = (char *) malloc(sizeof(char) * 10);
            args[5] = NULL;
            
            sprintf(args[1], "%d", customersPidsIndex);
            sprintf(args[2], "%d", productAmountThresh);
            sprintf(args[3], "%d", numShelvingTeams);
            sprintf(args[4], "%d", buyTime);

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
        fflush(stdout);
        exitProgram(0);
    }


}

/*
function to initialize IPCs resources (shared memory, semaphores, message queues)
*/
void initializeIPCResources() {
    // Create a shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), "project2.c");

    // Copy the all products struct to the shared memory segment
    memcpy(shmptr_product, (char *) &allProducts, sizeof(struct AllProducts));

    // print the shared memory segment
    printSharedMemory(shmptr_product, "project2.c");

    // create the msg queue for the teams
    msgqid_team = createMessageQueue(MSGQKEY_TEAM, "project2.c");

    // create the msg queue for the gui
    msgqid_gui = createMessageQueue(MSGQKEY_GUI, "project2.c");
    
    // Create 2 semaphores one for product on shelves and one for product in storage
    semid_product = createSemaphore(SEMKEY_PRODUCT, 2, "project2.c");

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

    // delete the message queue for the teams
    deleteMessageQueue(msgqid_team);

    // delete the message queue for the gui
    deleteMessageQueue(msgqid_gui);

    printf("IPC resources cleaned up successfully\n");
    printf("Exiting...\n");
    fflush(stdout);
    exit(0);
}

// function to initialize the signal handlers (SIGINT, SIGUSR1, SIGALRM)
void initializeSignalHandlers() {
    // signal handler for SIGINT to call a function before exiting
    if (signal(SIGINT, exitProgram) == SIG_ERR) {
        perror("signal -- SIGINT -- project2.c");
        exit(1);
    }

    // signal handler for SIGUSR1 to indicate that that storage is out of stock
    if (signal(SIGUSR1, catchSIGUSR1) == SIG_ERR) {
        perror("signal -- SIGUSR1 -- project2.c");
        exit(1);
    }

    // signal handler for alarm to indicate that the simulation time is over
    if (signal(SIGALRM, catchSIGALRM) == SIG_ERR) {
        perror("signal -- SIGALRM -- project2.c");
        exit(1);
    }
}

// function to catch SIGUSR1 signal to indicate that that storage is out of stock
void catchSIGUSR1(int signum) {
    printf("SIGUSR1 signal received\n");
    printf("*********************************\n");
    printf("**** Storage is out of stock ****\n");
    printf("*********************************\n");
    fflush(stdout);
    // set the flag to indicate that the simulation is done
    isSimulationDone = 1;
    exitProgram(signum);
}

// function to catch SIGALRM signal to indicate that the simulation time is over
void catchSIGALRM(int signum) {
    printf("SIGALRM signal received\n");
    printf("*********************************\n");
    printf("**** Simulation time is over ****\n");
    printf("*********************************\n");
    fflush(stdout);
    // set the flag to indicate that the simulation is done
    isSimulationDone = 1;
    exitProgram(signum);
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
        } else if (strcmp(varName, "managerTimeToFillCart") == 0) {
            managerTimeToFillCart = value;
        } else if (strcmp(varName, "employeeFillTime") == 0) {
            employeeFillTime = value;
        } else if (strcmp(varName, "custBuyTime") == 0) {
            custBuyTimeMin = min;
            custBuyTimeMax = max;
        } else {
            printf("Invalid variable name: %s\n", varName);
            fflush(stdout);
        }

//         managerTimeToFillCart   1
// employeeFillTime    1
// custBuyTime         1,2
        
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
    while (fgets(tempLine, sizeof(tempLine), file) != NULL && i < numProducts && i < MAX_PRODUCTS) {
        struct Product product;
        product.ID = i + 1;
        sscanf(tempLine, "%[^,], %d, %d", product.Name.str, &product.onShelvesAmount, &product.storageAmount);
        product.shelfCapacity = product.onShelvesAmount; // set the shelf capacity to the amount of the product on shelves
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

    while (fgets(tempLine, sizeof(tempLine), file) != NULL && i < numShelvingTeams && i < MAX_TEAMS) {
        struct Team team;
        team.team_id = i;
        sscanf(tempLine, "%d", &team.num_employees);
        teams[i] = team;
        i++;
    }
    fclose(file); // closing the file

}
