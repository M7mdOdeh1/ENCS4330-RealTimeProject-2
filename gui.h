/*
header file for gui.c
this file used to define structs and constants related for gui.c (graphical user interface)
*/

#ifndef GUI_H
#define GUI_H

#include <GL/glut.h>
#include <math.h>

#define MSGQKEY_GUI 5555
#define MAX_EMPLOYEES 10 // maximum number of employees in the team


void moveCustomer(int id, int x, int y);
void drawRectangle(float x, float y, float width, float height, float r, float g, float b);
void drawCircle(float cx, float cy, float radius, int num_segments, float r, float g, float b);
void drawText(const char *text, float x, float y, void *font, float r, float g, float b);
void drawPalestinianFlag(float x, float y, float width, float height);
void display(void);
void drawStorageBlock();
void drawArrowWithText();
void initPositions();
void drawSupermarketLayout();
void drawCheckeredFloor();
int findProductIndex(int productID);
void sendPositionUpdateMsg(int x, int y, int state);
void moveTeam(int id, int x, int y, int state);
void drawSouthAfricanFlag(float x, float y, float width, float height);
void catchSIGUSR2(int signum);


// Structure for both Custmers and Employees
typedef struct {
    float x, y;
    float r, g, b; // Color components for drawing
} Object;

typedef Object Customer;
typedef Object Employee;
typedef Object ProductObject;

typedef struct {
    Employee employees[MAX_EMPLOYEES];
    int rollingCartAmount;
} TeamObject;

#define MSG_POS_UPDATE 1

typedef struct {
    long msgType; // type of the message
    int personType; // 1 for customer, 2 for team
    int id;       // id of the customer or team
    int x, y;   // product index and quantity
    int state;    // 
} PositionUpdateMessage;




#endif