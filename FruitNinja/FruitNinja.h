#pragma once

#include "resource.h"

//Sizes:
#define PROGRESS_BAR_THICK				25          //progress bar thickness
#define FRUIT_SIZE						40          //the size of the fruit
#define SQUARE_SIZE                     50          //size of chessboard square

//Timers id:
#define MOUSE							1           //after not moving mouse
#define MOVEMENT						2           //changing positions of fruits
#define FREQUENCY						3           //the time when the next fruit is created
#define PROGRESS						4           //used for managing progress bar and polygonal chain of the mouse
#define END								5           //end of game

//Timers values:
#define MOUSE_T                         3000
#define MOVEMENT_T                      10
#define FREQUENCY_T                     1500
#define PROGRESS_T                      1
#define END_T                           30010


//Class for fruits:
class balls
{
public:

    int size;   //size of the fruit
    double x;   //x coordinate
    double y;   //y coordinate
    double dx;  //delta x 
    double dy;  //delta y

    COLORREF color = RGB(rand() % 255, rand() % 255, rand() % 255); //color of the fruit


    balls(int _size = 0, double _x = 0, double _y = 0, double _dx = 0, double _dy = 0) :size(_size), x(_x), y(_y), dx(_dx), dy(_dy) {}

};

