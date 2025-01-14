//
//  Passenger.h
//  


#ifndef Passenger_H
#define Passenger_H

#include "ECGraphicViewImp.h"

#include <string>

// passenger class
class Passenger
{
public:
    Passenger( int reqFloor, int destFloor, int xPos, int yPos, int walkingSpeed, int timeReq, int timeArrive, int totalFloors );

    // draw a passenger
    void drawPassenger( ECGraphicViewImp &view );
    
    // set variables
    void setServiced( bool serviced );
    void setIsWalking ( bool walking );
    
    // get boolean variables
    bool isServiced() const;
    bool isWalk() const;
    
    // get integer variables
    int getReqFloor() const;
    int getDestFloor() const;
    int getReqTime() const; 
    int getAriveTime() const;
    int getWalkingSpeed() const;

    int xPos; int yPos;

private:
    // floor variables
    int reqFloor;
    int destFloor;
    int walkingSpeed;
    
    // integer related variables
    int timeReq;
    int timeArrive;
    int totalFloors;

    // booleans for tracking
    bool isServed;
    bool isWalking;
};
#endif /* Passenger.h */