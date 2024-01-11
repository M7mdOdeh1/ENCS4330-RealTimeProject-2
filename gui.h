/*
header file for gui.c
this file used to define structs and constants related for gui.c (graphical user interface)
*/

#ifndef GUI_H
#define GUI_H

#include <GL/glut.h>
#include <math.h>

#define MSGQKEY_GUI 5555

// Structure for both Custmers and Employees
typedef struct {
    float x, y;
    float r, g, b; // Color components for drawing
} Person;


#define MSG_POS_UPDATE 1

typedef struct {
    long msgType; // type of the message
    int personType; // 1 for customer, 2 for team
    int id;       // id of the customer or team
    int x, y;   // product index and quantity
    int state;    // 
} PositionUpdateMessage;




#endif