//
//  Passenger.cpp
//  

#include "Passenger.h"

using namespace std;

Passenger :: Passenger (int reqFloor, int destFloor, int xPos, int yPos, int walkingSpeed, int timeReq, int timeArrive, int totalFloors) : reqFloor(reqFloor), destFloor(destFloor), 
xPos(xPos), yPos(yPos), walkingSpeed(walkingSpeed), timeReq(timeReq), timeArrive(timeArrive), totalFloors(totalFloors), isWalking(false), isServed(false) {}

void Passenger :: drawPassenger( ECGraphicViewImp &view )
{
    // generate the color for the destination floor
    ECGVColor color;

    if ( destFloor % 6 == 0 ) { color = ECGV_WHITE; }
    else if ( destFloor % 5 == 0 ) { color = ECGV_YELLOW; }
    else if ( destFloor % 4 == 0 ) { color = ECGV_RED; }
    else if ( destFloor % 3 == 0 ) { color = ECGV_PURPLE; }
    else if ( destFloor % 2 == 0 ) { color = ECGV_CYAN; }
    else { color = ECGV_GREEN; }
    
    // get the right radius and heights
    int heightAdd; int radius; int posAdd;
    if (totalFloors >= 11 ) { heightAdd = 25; radius = 8; posAdd = 60; }
    else { heightAdd = 30; radius = 10; posAdd = 65; }
    
    // draw the passenger
    view.DrawFilledCircle(xPos, yPos - posAdd, radius, color);
    view.DrawFilledEllipse(xPos, yPos - 30, radius+2, heightAdd, color);
    // write its destination floor on it
    string num = to_string(destFloor);
    const char *text = num.c_str();
    view.DrawText(xPos, yPos - 50, text); 

    view.SetRedraw(true);
}

void Passenger :: setServiced( bool serviced ) { isServed = serviced; }

void Passenger :: setIsWalking ( bool walking ) { isWalking = walking; }

bool Passenger :: isServiced() const { return isServed; }

bool Passenger :: isWalk() const { return isWalking; }

int Passenger :: getReqFloor() const { return reqFloor; }

int Passenger :: getDestFloor() const { return destFloor; }

int Passenger :: getReqTime() const { return timeReq; }

int Passenger :: getAriveTime() const { return timeArrive; }

int Passenger :: getWalkingSpeed() const { return walkingSpeed; }