/*
header file for gui.c
this file used to define structs and constants related for gui.c (graphical user interface)
*/

#ifndef GUI_H
#define GUI_H

#define MSGQKEY_TEAM 

// Structure for both Custmers and Employees
typedef struct {
    float x, y;
    float r, g, b; // Color components for drawing
} Person;


#define MSG_POS_UPDATE 1

typedef struct {
    long msgType; // Message type, used to filter messages
    int id;       // Identifier for cashier or customer
    int x, y;   // Position coordinates
    int state;    // State (e.g., busy, idle for cashier; shopping, in queue for customer)
} PositionUpdateMessage;




#endif