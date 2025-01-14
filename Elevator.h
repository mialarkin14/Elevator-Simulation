//
//  Elevator.h
//  


#ifndef Elevator_H
#define Elevator_H

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "ECElevatorSim.h"
#include "Passenger.h"

#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <map>


// elevator class
class ElevatorObserver : public ECObserver
{
public:
    ElevatorObserver( ECGraphicViewImp &viewIn, int numFloors, int windowHeight, 
    vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history, int time, int floorSize ); 
    virtual void Update();

    void drawBackground();
    void drawFloor( int baseY, int floorNum, ECGVColor color );

    void moveElevator( int targFloor, string state );
    void displayLabels( int time, int numPass );
    void handleElevator(tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>> &data);
    

private:
    ECGraphicViewImp &view;
    // position of the elevator and elevator related vairbales
    int numFloors;
    int currPos;
    int targPos;

    // booleans for moving
    bool isMoving;
    bool isStopped;
    bool isLoad;

    // variables related to time
    int stopTime;
    int totalTime;
    int countdown;
    int displayTime;

    // boolean to pause the animation
    bool isAnimationPaused;
    
    // variables to track the scale 
    int windowHeight;
    int floorSize;

    // history vector
    vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history;

    // passengers vector
    vector<Passenger> passengersOffEle;
    vector<Passenger> passengersOnEle;

    // map of buttons
    map<int, bool> upButtons;
    map<int, bool> downButtons;
};


#endif /* Elevator.h */