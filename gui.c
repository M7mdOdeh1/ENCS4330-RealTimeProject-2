#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define MAX_CUSTOMERS 10
#define MAX_EMPLOYEES 36
#define MAX_SHELVES 10
#define TEAM_SIZE 6

Person customers[MAX_CUSTOMERS];
Person employees[MAX_EMPLOYEES];

int msgqid_gui; // Message queue ID

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
void drawText(const char *text, float x, float y, void *font) {
    glColor3f(1.0f, 1.0f, 1.0f); // White color text
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
    for (int x = 0; x < 800; x += tileSize) {
        for (int y = 0; y < 600; y += tileSize) {
            float color = (x / tileSize + y / tileSize) % 2 * 0.5f; // Checkerboard pattern
            drawRectangle(x, y, tileSize, tileSize, color, color, color);
        }
    }
}

// Function to initialize positions for customers and employees
void initPositions() {
    // access the msg queue for gui
    msgqid_gui = createMessageQueue(MSGQKEY_GUI, src.str);



    // Initialize customer positions
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        customers[i].x = 50.0f;
        customers[i].y = 100.0f + i * 45.0f;
    }

    // Initialize employee positions
    float startX = 650.0f; // Starting X position for the first team member
    float startY = 500.0f; // Starting Y position for the first team
    float gapX = 20.0f;    // Horizontal gap between team members
    float gapY = 80.0f;   // Vertical gap between teams

    for (int team = 0; team < MAX_EMPLOYEES / TEAM_SIZE; team++) {
        for (int member = 0; member < TEAM_SIZE; member++) {
            int index = team * TEAM_SIZE + member;
            employees[index].x = startX + member * gapX;
            employees[index].y = startY - team * gapY;
           
              if (member == TEAM_SIZE - 1) {
                employees[index].r = 0.9f; 
                employees[index].g = 0.8f;
                employees[index].b = 0.1f;
            } else {
                employees[index].r = 0.15f; 
                employees[index].g = 0.15f;
                employees[index].b = 1.0f;
            }
      
        }
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
    drawText("Customers Go to Buy", 20.0f, 45.0f, GLUT_BITMAP_HELVETICA_18);
}

// draw supermarket layout
void drawSupermarketLayout(){
    glClear(GL_COLOR_BUFFER_BIT);

    drawCheckeredFloor(); // Draw the checkered floor first

    // Draw item block in the middle  brown color
    drawRectangle(300.0f, 80.0f, 200.0f, 440.0f, 0.2f, 0.2f, 0.3f); 

    // Draw Palestinian flag above the items block with the same width as the item block
    drawPalestinianFlag(300.0f, 520.0f, 200.0f, 40.0f); // Flag width same as items block

    // Draw text on the items block
    drawText("Items", 370.0f, 490.0f, GLUT_BITMAP_TIMES_ROMAN_24);

    // Draw customers as red dots on the left
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        drawCircle(customers[i].x, customers[i].y, 15.0f, 16, 0.9f, 0.1f, 0.1f);
        // Number the customers
        char numberBuffer[4];
        sprintf(numberBuffer, "%d", i + 1);
        drawText(numberBuffer, customers[i].x - 10.0f, customers[i].y - 5.0f, GLUT_BITMAP_HELVETICA_18);
    }

    // Draw shelves as  squares at the sides of the items block
    for (int i = 0; i < MAX_SHELVES; i++) {
        drawRectangle(280.0f, 80.0f + i * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f); //  shelves
        drawRectangle(500.0f, 80.0f + i * 44.0f, 20.0f, 40.0f, 0.8f, 0.57f, 0.20f);//  shelves
    }

    // Draw employees in horizontal lines for each team, with the teams vertically stacked
    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        drawCircle(employees[i].x, employees[i].y, 10.0f, 16, employees[i].r, employees[i].g, employees[i].b);
    }

    drawArrowWithText();
}



// Display callback
void display() {
    PositionUpdateMessage msg;
    while (msgrcv(msgQueueId, &msg, sizeof(msg) - sizeof(long), MSG_POS_UPDATE, IPC_NOWAIT) != -1){
        


    }

    drawSupermarketLayout();
    glutSwapBuffers();
}

// Main function
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Simple Supermarket Simulation");

    // Initialize positions
    initPositions();

    // OpenGL initialization
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
