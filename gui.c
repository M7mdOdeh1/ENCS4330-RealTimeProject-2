#include "gui.h"
#include "local.h"
#include "ipcs.h"


Customer customers[MAX_CUSTOMERS];
ProductObject product[MAX_PRODUCTS];
TeamObject teams[MAX_TEAMS];


int msgqid_gui; // Message queue ID
PositionUpdateMessage *msg;
char *shmptr_product;
struct AllProducts *ptrAllProducts;
int productAmountThresh;

// Main function
int main(int argc, char **argv) {
    productAmountThresh = atoi(argv[1]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1150, 650);
    glutCreateWindow("Supermarket Simulation");

    // Initialize positions
    initPositions();

    // OpenGL initialization
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1150.0, 0.0, 650.0); // Updated to match the new window size


    glutDisplayFunc(display);

    glutIdleFunc(display);

    glutMainLoop();

    return 0;
}

// Function to initialize positions for customers and employees
void initPositions() {

    // access the msg queue for the gui
    msgqid_gui = createMessageQueue(MSGQKEY_GUI, "gui.c");

    // access the shared memory segment for the all products struct
    shmptr_product = (char *) malloc(sizeof(struct AllProducts));
    shmptr_product = createSharedMemory(SHKEY_PRODUCT, sizeof(struct AllProducts), "gui.c");
    ptrAllProducts = (struct AllProducts *) shmptr_product;

    msg = (PositionUpdateMessage *) malloc(sizeof(PositionUpdateMessage));

    // Initialize customer positions
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        customers[i].x = -10.0f;
        customers[i].y = -10.0f;
    }

    // Initialize employee positions
    float startX = -10.0f; // Starting X position for the first team member
    float startY = -10.0f; // Starting Y position for the first team
    float gapX = 0.0f;    // Horizontal gap between team members
    float gapY = 0.0f;   // Vertical gap between teams

    for (int i = 0; i < MAX_TEAMS; i++) {
        for (int member = 0; member < MAX_EMPLOYEES; member++) {
            //int index = i * MAX_EMPLOYEES + member;
            teams[i].employees[member].x = startX + member * gapX;
            teams[i].employees[member].y = startY + i * gapY;
           
            if (member == MAX_EMPLOYEES - 1) {
                teams[i].employees[member].r = 0.9f;
                teams[i].employees[member].g = 0.8f;
                teams[i].employees[member].b = 0.1f;
                
            } else {
                teams[i].employees[member].r = 0.15f;
                teams[i].employees[member].g = 0.15f;
                teams[i].employees[member].b = 1.0f;            
            }
      
        }
        teams[i].rollingCartAmount = 0;
        // draw text with the rolling cart amount
        char amount[255];
        sprintf(amount, "%d", teams[i].rollingCartAmount);
        drawText(amount, teams[i].employees[MAX_EMPLOYEES - 1].x - 10.0f, teams[i].employees[MAX_EMPLOYEES - 1].y - 5.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        product[i].x = -10.0f;
        product[i].y = -10.0f;
    }
}
void drawArrowWithText() {
    // Draw the arrow body
    glColor3f(0.0f, 0.9f, 0.0f); // Green color
    glBegin(GL_POLYGON);
    glVertex2f(20.0f, 30.0f);
    glVertex2f(200.0f, 30.0f); // Extended length to fit the text
    glVertex2f(200.0f, 70.0f);
    glVertex2f(20.0f, 70.0f);
    glEnd();
    
    // Draw the arrowhead
    glColor3f(0.0f, 0.9f, 0.0f); // Green color
    glBegin(GL_TRIANGLES);
    glVertex2f(200.0f, 20.0f);
    glVertex2f(200.0f, 80.0f);
    glVertex2f(270.0f, 50.0f); // Position adjusted for the extended arrow body
    glEnd();

    // Draw the text "Customers Go to Buy"
    drawText("Customers Go to Buy", 20.0f, 45.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
}

void drawStorageBlock() {
    drawRectangle(920.0f, 80.0f, 150.0f, 540.0f, 0.2f, 0.2f, 0.3f); 

    drawText("Storage", 960.0f, 535.0f, GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    // Draw shelves as  squares at the sides of the Products block
    for (int i = 1; i <= MAX_PRODUCTS/2; i++) {
        drawRectangle(900.0f, 36.0f + i * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f); //  shelves
        // find product id
        int productIndex = findProductIndex(i);
        if (productIndex == -1) {
            continue;
        }
        char name[255];
        sprintf(name, "%s", ptrAllProducts->products[productIndex].Name.str);   
        // draw product name in bold
        drawText(name, 930.0f, 50.0f + i * 44.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
        // draw product amount
        char amount[255];
        sprintf(amount, "%d", ptrAllProducts->products[productIndex].storageAmount);
        // draw product amount in black
        drawText(amount, 900.0f, 45.0f + i * 44.0f, GLUT_BITMAP_HELVETICA_12, 0.0f, 0.0f, 0.0f);
    }

    for (int i = MAX_PRODUCTS/2 +1 ; i <= MAX_PRODUCTS; i++) {
        drawRectangle(1070.0f, 36.0f + (i - MAX_PRODUCTS/2) * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f); //  shelves
        // find product id
        int productIndex = findProductIndex(i);
        if (productIndex == -1) {
            continue;
        }
        char name[255];
        sprintf(name, "%s", ptrAllProducts->products[productIndex].Name.str);
        // draw product name in bold
        drawText(name, 990.0f, 50.0f + (i - MAX_PRODUCTS/2) * 44.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
        // draw product amount
        char amount[255];
        sprintf(amount, "%d", ptrAllProducts->products[productIndex].storageAmount);
        drawText(amount, 1070.0f, 45.0f + (i - MAX_PRODUCTS/2) * 44.0f, GLUT_BITMAP_HELVETICA_12, 0.0f, 0.0f, 0.0f);
        
    }

    
}

void drawSupermarketLayout(){
    drawCheckeredFloor(); // Draw the checkered floor first

    // Draw item block in the middle  brown color
    drawRectangle(300.0f, 80.0f, 200.0f, 540.0f, 0.2f, 0.2f, 0.3f); 

    // Draw storage block on the right
    drawStorageBlock();

    // Draw Palestinian flag above the Products block with the same width as the item block
    drawPalestinianFlag(300.0f, 570.0f, 200.0f, 70.0f); // Flag width same as Products block
    //drawPalestinianFlag(900.0f, 570.0f, 150.0f, 60.0f); // Flag width same as Products block
    drawSouthAfricanFlag(920.0f, 570.0f, 150.0f, 60.0f);

    // Draw text on the Product block
    drawText("Products", 355.0f, 535.0f, GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 1.0f, 1.0f);

    // Draw customers as red dots on the left
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        drawCircle(customers[i].x, customers[i].y, 15.0f, 16, 0.9f, 0.1f, 0.1f);
        // Number the customers
        char numberBuffer[4];
        sprintf(numberBuffer, "%d", i + 1);
        drawText(numberBuffer, customers[i].x - 10.0f, customers[i].y - 5.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    // Draw shelves as  squares at the sides of the Products block
    for (int i = 1; i <= MAX_PRODUCTS/2; i++) {
        drawRectangle(280.0f, 36.0f + i * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f); //  shelves

        // find product id
        int productIndex = findProductIndex(i);
        if (productIndex == -1) {
            continue;
        }

        char name[255];
        sprintf(name, "%s", ptrAllProducts->products[productIndex].Name.str);   
        // draw product name in bold
        drawText(name, 310.0f, 50.0f + i * 44.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
        // draw product amount
        char amount[255];
        sprintf(amount, "%d", ptrAllProducts->products[productIndex].onShelvesAmount);
        // draw product amount in black
        drawText(amount, 280.0f, 45.0f + i * 44.0f, GLUT_BITMAP_HELVETICA_12, 0.0f, 0.0f, 0.0f);

        // if the product below the threshold, make the shelf red
        if (ptrAllProducts->products[productIndex].onShelvesAmount < productAmountThresh) {
            drawRectangle(280.0f, 36.0f + i * 44.0f, 20.0f, 40.0f, 0.9f, 0.1f, 0.1f); //  shelves
        }

    }
    for (int i = MAX_PRODUCTS/2 +1 ; i <= MAX_PRODUCTS; i++) {
        drawRectangle(500.0f, 36.0f + (i - MAX_PRODUCTS/2) * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f); //  shelves

        // find product id
        int productIndex = findProductIndex(i);
        if (productIndex == -1) {
            continue;
        }

        char name[255];
        sprintf(name, "%s", ptrAllProducts->products[productIndex].Name.str);
        // draw product name in bold
        drawText(name, 420.0f, 50.0f + (i - MAX_PRODUCTS/2) * 44.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
        // draw product amount
        char amount[255];
        sprintf(amount, "%d", ptrAllProducts->products[productIndex].onShelvesAmount);
        drawText(amount, 500.0f, 45.0f + (i - MAX_PRODUCTS/2) * 44.0f, GLUT_BITMAP_HELVETICA_12, 0.0f, 0.0f, 0.0f);

        if (ptrAllProducts->products[productIndex].onShelvesAmount < productAmountThresh) {
            drawRectangle(500.0f, 36.0f + (i - MAX_PRODUCTS/2) * 44.0f, 20.0f, 40.0f, 0.9f, 0.1f, 0.1f); //  shelves
        }
   
    }
    /*
        Draw employees in horizontal lines for each team, with the teams vertically stacked
    */
    for (int i = 0; i < MAX_TEAMS; i++) {
        for (int member = 0; member < MAX_EMPLOYEES; member++) {

            drawCircle(teams[i].employees[member].x, teams[i].employees[member].y, 15.0f, 16, teams[i].employees[member].r, teams[i].employees[member].g, teams[i].employees[member].b);
            // Number the employees
            char numberBuffer[4];
            sprintf(numberBuffer, "%d", member + 1);
            drawText(numberBuffer, teams[i].employees[member].x - 10.0f, teams[i].employees[member].y - 5.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        }
        // draw text with the rolling cart amount
        char amount[255];
        sprintf(amount, "%d", teams[i].rollingCartAmount);
        drawText(amount, teams[i].employees[0].x + 20.0f, teams[i].employees[0].y, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
    }
    
    

    //drawArrowWithText();
}

// function to find product id and return its index
int findProductIndex(int productID) {
    for (int i = 0; i < ptrAllProducts->numProducts; i++) {
        if (ptrAllProducts->products[i].ID == productID) {
            return i;
        }
    }
    return -1;
}

// Display callback
void display() {
    
    glClear(GL_COLOR_BUFFER_BIT);
   
    while (msgrcv(msgqid_gui, msg, sizeof(PositionUpdateMessage), MSG_POS_UPDATE, IPC_NOWAIT) != -1) {
        
    
        if (msg->personType == 1) { // Check if the message is for a customer
            // move the customer
            moveCustomer(msg->id, msg->x, msg->y);
            
        } else if (msg->personType == 2) { // Check if the message is for a team
            moveTeam(msg->id, msg->x, msg->y, msg->state);
        }
    }


    // Draw the supermarket layout
    drawSupermarketLayout();

    glutSwapBuffers();
}

int temp1 = 0;
int temp2 = 0; 
// move the customer
void moveCustomer(int id, int x, int y) {
    if (x<0 && y<0) {

        if (temp1%15 == 0){
            temp2++;
            temp1 = 0;
        }

        // move the customer to the market
        customers[id].x = temp2 * 35.0f;
        customers[id].y = 70.0f + temp1 * 35.0f;

        temp1++;

    } 
    else if (x<0 && y>=0) {
        // move the customer to product shelf
        if(y <= 10){
            customers[id].x = 260.0f;
            customers[id].y = 51.0f + y * 44.0f;
        }
        else{
            customers[id].x = 540.0f;
            customers[id].y = 51.0f + (y-10) * 44.0f;
        }
        
    }
    else {
        // exit the market
        customers[id].x = -10.0f;
        customers[id].y = -10.0f;
        
        
    }
}

int employeeIndex = 1;
int turn = 1;

void moveTeam(int id, int x, int y, int state) {
    // if the team is created
    if (state == 0){
        x++;
        int gapX = 35.0f;
        int gapY = 50.0f;
        teams[id].employees[0].r = 0.9f;
        teams[id].employees[0].g = 0.8f;
        teams[id].employees[0].b = 0.1f;
        for (int member = 0; member < x; member++) {
            teams[id].employees[member].x = 810.0f - member * gapX;
            teams[id].employees[member].y = 200.0f + id * gapY;
        }
    } 
    // if the team manager is moving to the product in the storage
    else if(state == 1){
        int productID = ptrAllProducts->products[x].ID;
        
        // move the team manager to the storage to product y
        if (productID <= 10){
            teams[id].employees[0].x = 930.0f;
            teams[id].employees[0].y = 51.0f + productID * 44.0f;
        }
        else{
            teams[id].employees[0].x = 1110.0f;
            teams[id].employees[0].y = 51.0f + (productID-10) * 44.0f;
        } 

    }
    // if the team manager is moving back from the storage
    else if (state == 2){
        //int productID = ptrAllProducts->products[x].ID;
        
        // move the team manager back from the storage
        teams[id].employees[0].x = 810.0f;
        teams[id].employees[0].y = 200.0f + id * 50.0f;

        // change the rolling cart amount
        teams[id].rollingCartAmount = y;
    }
    // if the employee start working 
    else if(state == 3 || state == 4){
        printf("employeeIndex = %d\n", employeeIndex);
        if (turn ==1){ 
            int productID = ptrAllProducts->products[y].ID;
            // move the employee to the product shelf
            if(y <= 10){
                teams[id].employees[employeeIndex].x = 230.0f;
                teams[id].employees[employeeIndex].y = 51.0f + productID * 44.0f;
            }
            else{
                teams[id].employees[employeeIndex].x = 570.0f;
                teams[id].employees[employeeIndex].y = 51.0f + (productID-10) * 44.0f;
            }
            // decrease the amount of the rolling cart
            teams[id].rollingCartAmount--;
        }
        else{
            // move the employee back to the team manager
            teams[id].employees[employeeIndex].x = 810.0f - employeeIndex * 35.0f;
            teams[id].employees[employeeIndex].y = 200.0f + id * 50.0f;

            employeeIndex++;
            
            
        }
        if (employeeIndex == x+1){
                employeeIndex = 1;
            }
        turn *= -1;
    }
    
    
}

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to draw a circle
void drawCircle(float cx, float cy, float radius, int num_segments, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * M_PI * (float)i / (float)num_segments;
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Function to draw text
void drawText(const char *text, float x, float y, void *font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Function to draw the Palestinian flag
void drawPalestinianFlag(float x, float y, float width, float height) {
    // Draw the black stripe
    drawRectangle(x, y + 2 * (height / 3), width, height / 3, 0.0f, 0.0f, 0.0f);
    // Draw the white strip
    drawRectangle(x, y + (height / 3), width, height / 3, 1.0f, 1.0f, 1.0f);
    // Draw the green stripe
    drawRectangle(x, y, width, height / 3, 0.0f, 0.5f, 0.0f);
    // Draw the red triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(x, y);
    glVertex2f(x, y + height);
    glVertex2f(x + width / 2, y + height / 2);
    glEnd();
}

// Function to draw checkered floor
void drawCheckeredFloor() {
    int tileSize = 50;
    for (int x = 0; x < 1150; x += tileSize) {
        for (int y = 0; y < 650; y += tileSize) {
            float color = (x / tileSize + y / tileSize) % 2 * 0.5f; // Checkerboard pattern
            drawRectangle(x, y, tileSize, tileSize, color, color, color);
        }
    }
}


void drawSouthAfricanFlag(float x, float y, float width, float height) {
    // Draw three horizontal rectangles (blue, white, red)
    drawRectangle(x, y, width, height / 3, 0.0f, 0.0f, 0.50f); // Blue
    drawRectangle(x, y + height / 3, width, height / 3, 1.0f, 1.0f, 1.0f); // White
    drawRectangle(x, y + 2 * height / 3, width, height / 3, 0.8f, 0.0f, 0.0f); // Red

    // Draw the large white triangle
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glVertex2f(x, y);
    glVertex2f(x, y + height);
    glVertex2f(x + 3 * width / 5, y + height / 2);  
    glEnd();
    drawRectangle(x, y + height / 2.5, width, height / 5, 0.0f, 0.5f, 0.0f); 

   glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.5f, 0.0f); // Green
    glVertex2f(x, y + height / 15);
    glVertex2f(x, y + 14 * height / 15);
    glVertex2f(x + 11 * width / 20, y + height / 2);  
    glEnd();

    // Draw the yellow triangle, same size as before
    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.8f, 0.0f); // Yellow
    glVertex2f(x, y + height / 5);
    glVertex2f(x, y + 4 * height / 5);
    glVertex2f(x + width / 3, y + height / 2);  
    glEnd();

    // Draw the black triangle
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 0.0f); 
    glVertex2f(x, y + 3 * height / 10);
    glVertex2f(x, y + 7 * height / 10);
    glVertex2f(x + width / 4, y + height / 2);  
    glEnd();
}
