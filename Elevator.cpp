//
//  Elevator.cpp
//  


#include "Elevator.h"
#include "ECElevatorSim.h"
#include "Passenger.h"

using namespace std;

// initialize the elevator
ElevatorObserver :: ElevatorObserver( ECGraphicViewImp &viewIn, int numFloors, int windowHeight, 
vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history, int time, int floorSize ) : view(viewIn), numFloors(numFloors), 
windowHeight(windowHeight), history(history), totalTime(time), floorSize(floorSize), currPos(windowHeight), targPos(windowHeight), stopTime(0), countdown(65),
isMoving(false), isStopped(false), isAnimationPaused(false) 
{
    // get passenger data
    vector<ECElevatorSimRequest> requests = get<4>(history.back());
    // generate the passengers
    for ( auto request : requests )
    {
        // get speed
        int speed = 1 + rand() % 3;
        // get starting position
        int startingPos;
        if ( numFloors < 10 ) { startingPos = 900; }
        else { startingPos = 850; }
        // create a passenger and add to passengersOffElevator vector
        Passenger pas(request.GetFloorSrc(), request.GetFloorDest(), startingPos, windowHeight  - (request.GetFloorSrc() - 1) * floorSize, 
        speed, request.GetTime(), request.GetArriveTime(), numFloors ); 
        passengersOffEle.push_back(pas);
    }
}

// helper function draw each floor
void ElevatorObserver :: drawFloor( int baseY, int floorNum, ECGVColor color )
{
    // floor line
    view.DrawLine(0, baseY, 1000, baseY, 10);

    // elevator outline
    view.DrawFilledRectangle(385, baseY-5, 395, baseY-floorSize+5, ECGV_WHITE);
    view.DrawFilledRectangle(585, baseY-5, 595, baseY-floorSize+5, ECGV_WHITE);
    string floorLabel = "Floor " + to_string(floorNum);
    const char *text = floorLabel.c_str();
    view.DrawText(80, baseY - floorSize + 14, text, ECGV_RED);

    // table
    view.DrawFilledRectangle(300, baseY-5, 310, baseY-15);
    view.DrawFilledEllipse(305, baseY-20, 30, 5);

    // flower
    view.DrawFilledRectangle(303, baseY-20, 307, baseY-45, ECGV_GREEN);
    view.DrawFilledCircle(305, baseY-50, 15, color);
    
    // addition related variables
    int windowAdd; int upAdd; int downAdd; int radius;

    if ( numFloors > 16 ) { upAdd = 22; downAdd = 53; windowAdd = 70; radius = 15; }
    else if ( numFloors >= 11 && numFloors <= 16 ) { upAdd = 30; downAdd = 72; windowAdd = 80; radius = 15; }
    else if ( numFloors <= 10 ) { upAdd = 50; downAdd = 100; windowAdd = 90; radius = 20; }

    // up button and add to the map with state off
    ECGVColor upColor;
    if ( upButtons[floorNum] ) { upColor = ECGV_GREEN; } else { upColor = ECGV_WHITE; }
    view.DrawFilledCircle(650, baseY-floorSize + upAdd, radius, upColor);
    view.DrawText(650, baseY-floorSize + (upAdd-20), "▲");
    upButtons[floorNum] = false;

    // down button and add to the map with state off
    ECGVColor downColor;
    if ( downButtons[floorNum] ) { downColor = ECGV_GREEN; } else { downColor = ECGV_WHITE; }
    view.DrawFilledCircle(650, baseY-floorSize + downAdd, radius, downColor);
    view.DrawText(650, baseY-floorSize + (downAdd-15), "▼");
    downButtons[floorNum] = false;
    
    // window
    int windowTop = baseY - floorSize + windowAdd;
    view.DrawFilledRectangle(754, windowTop-4, 896, windowTop-60);
    view.DrawFilledRectangle(760, windowTop-10, 822, windowTop-30, ECGV_CYAN);
    view.DrawFilledRectangle(828, windowTop-10, 890, windowTop-30, ECGV_CYAN);
    view.DrawFilledRectangle(760, windowTop-35, 822, windowTop-55, ECGV_CYAN);
    view.DrawFilledRectangle(828, windowTop-35, 890, windowTop-55, ECGV_CYAN);

    view.SetRedraw(true);
}

// draw the background
void ElevatorObserver :: drawBackground()
{
    ECGVColor color;
    
    // draw the display bar
    view.DrawFilledRectangle(0, 0, 1000, 65);
    
    // draw the floors
    for ( int i = numFloors; i >= 1; i-- )
    {
        if ( (( numFloors + 1 ) - i) % 6 == 0 ) { color = ECGV_WHITE; }
        else if ( (( numFloors + 1 ) - i) % 5 == 0 ) { color = ECGV_YELLOW; }
        else if ( (( numFloors + 1 ) - i) % 4 == 0 ) { color = ECGV_RED; }
        else if ( (( numFloors + 1 ) - i) % 3 == 0 ) { color = ECGV_PURPLE; }
        else if ( (( numFloors + 1 ) - i) % 2 == 0 ) { color = ECGV_CYAN; }
        else { color = ECGV_GREEN; }
        drawFloor( windowHeight - (( numFloors + 1) - i-1)*floorSize, ( numFloors + 1 ) - i, color);
    }
}

// display the labels at the top
void ElevatorObserver :: displayLabels( int time, int numPass )
{
    // display number of passengers
    string numPassLabel = "Number of Passengers: " + to_string(numPass);
    const char *text = numPassLabel.c_str();
    view.DrawText(280, 15, text, ECGV_GREEN);
    
    // display the time 
    if ( time == totalTime - 1 && countdown == 0)
    {
        time = totalTime;
        string floorLabel = "Time: " + to_string(time) + "/" + to_string(totalTime);
        const char *text1 = floorLabel.c_str();
        view.DrawText(840, 15, text1, ECGV_GREEN);
    }
    else 
    {
        string floorLabel = "Time: " + to_string(time) + "/" + to_string(totalTime);
        const char *text2 = floorLabel.c_str();
        view.DrawText(840, 15, text2, ECGV_GREEN);
    }
    view.SetRedraw(true);
}

// helper function to move the elevator
void ElevatorObserver :: moveElevator( int targFloor, string state )
{
    // calculate the target position
    targPos = (windowHeight - targFloor * floorSize) + 155;
    
    // change elevator based on state
    if ( state == "Stopped" || state == "Loading/Unloading" ) { isStopped = true; stopTime = 65; }
    else { isMoving = true; isStopped = false; } 
}

// move and stop the elevator
void ElevatorObserver :: handleElevator(tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>> &data)
{
    // if we're not moving, move
    if (!isMoving && !isStopped && !history.empty()) 
    {
        // erase front data from history and grab needed numbers and states
        history.erase(history.begin());
        auto data2 = history.front();
        int targetFloor = get<1>(data2);
        string state = get<2>(data);
        // move the elevator
        moveElevator(targetFloor, state);
    }

    // handle when we are stopped
    if ( isStopped && stopTime-- <= 0 ) { isStopped = false; }

    // move to targte position
    if (isMoving && currPos < targPos) { currPos += 2; if (currPos > targPos) currPos = targPos; } 
    else if (isMoving && currPos > targPos) { currPos -= 2; if (currPos < targPos) currPos = targPos; }

    // stop movement when target is reached
    if (isMoving && currPos == targPos) { isMoving = false; }
}

void ElevatorObserver :: Update()
{   
    // get the current event
    ECGVEventType evt = view.GetCurrEvent(); 

    // handle pausing the elevator on the space key
    if ( evt == ECGV_EV_KEY_DOWN_SPACE ) { isAnimationPaused = !isAnimationPaused; return; }
    if ( isAnimationPaused ) { return; }

    // ************************************************** Drawing on the timer **************************************************
    if( evt == ECGV_EV_TIMER ) 
    {
        // if the history vector is empty then there's nothing left to draw so stop animation
        if (history.size() == 0) { return; }
        
        // draw the background
        drawBackground();

        // display the time based on the first data input
        auto data = history.front();
        int time = get<0>(data); 
        displayLabels(time - 1, passengersOnEle.size());
        if (time == totalTime) {countdown--;}

        // draw the elevator
        int eleAdd; int eleAdd1;
        if ( time != 1) { eleAdd = 155; eleAdd1 = floorSize-160; }
        else { eleAdd = floorSize; eleAdd1 = 0; }
        view.DrawFilledRectangle(400, currPos + eleAdd1, 580, currPos - eleAdd);
        view.SetRedraw(true);

        // ***************************************** update and move passengers *****************************************


        // loop through all the passengers off the elevator
        for (  int j = 0; j < passengersOffEle.size(); j++ )
        {   
            // make the passenger walk toward the elevator one second ahead of their request time
            if ( time - 1 == passengersOffEle[j].getReqTime() - 1 ) { passengersOffEle[j].setIsWalking(true); }
            if ( passengersOffEle[j].isWalk() ) { passengersOffEle[j].xPos -= 2; passengersOffEle[j].drawPassenger(view); }

            // stop walking once hit destination and then turn on button
            if ( passengersOffEle[j].xPos == 700 + (30 * j) ) { passengersOffEle[j].setIsWalking(false);  }
            
            if ( time - 1 >= passengersOffEle[j].getReqTime() )
            {
                // make sure to show multiple passengers by offsetting their xPos
                if ( !passengersOffEle[j].isServiced() ) { passengersOffEle[j].xPos = 700 + (30 * j); }
                
                // turn on the correct button based on destination floor
                if ( passengersOffEle[j].getDestFloor() > passengersOffEle[j].getReqFloor() && !passengersOffEle[j].isServiced() ) 
                { upButtons[passengersOffEle[j].getReqFloor()] = true; }
                else if ( passengersOffEle[j].getDestFloor() < passengersOffEle[j].getReqFloor() && !passengersOffEle[j].isServiced() ) 
                { downButtons[passengersOffEle[j].getReqFloor()] = true; }
                
                // draw the passenger
                passengersOffEle[j].drawPassenger(view); 

                // create a step for offset
                int step;
                if ( numFloors > 16 ) { step = 80; }
                else if ( numFloors >= 11 && numFloors <= 16 ) { step = 60; }
                else if ( numFloors <= 10 ) { step = 0; }
                
                if ( currPos ==  ((windowHeight  - (passengersOffEle[j].getReqFloor() - 1) * floorSize) + step) && !passengersOffEle[j].isServiced() )
                {
                    // put the passenger on the elevator
                    passengersOffEle[j].xPos = 420;

                    // remove from the vector and add to the other vector
                    passengersOnEle.push_back(passengersOffEle[j]);
                    passengersOffEle.erase(passengersOffEle.begin() + j);
                }

                // if the passenger is off of the elevator and serviced make it move off the screen until it hits 0
                if (passengersOffEle[j].xPos <= 380 && passengersOffEle[j].xPos >= 0 && passengersOffEle[j].isServiced()) { passengersOffEle[j].xPos -= passengersOffEle[j].getWalkingSpeed(); }

                // remove the passenger when off screen
                if (passengersOffEle[j].xPos <= 0) { passengersOffEle.erase(passengersOffEle.begin() + j); }    
            }
        }

        // loop through all the passengers on the elevator
        for ( int i = 0; i < passengersOnEle.size(); i++ )
        {
            // turn off the buttons
            if ( time - 1 >  passengersOnEle[i].getReqTime() ) { upButtons[passengersOnEle[i].getReqFloor()] = false; downButtons[passengersOnEle[i].getReqFloor()] = false; }
            
            // create a step for offset
            int step;
            if ( numFloors > 16 ) { step = 80; }
            else if ( numFloors >= 11 && numFloors <= 16 ) { step = 60; }
            else if ( numFloors <= 10 ) { step = 5; }
            
            // draw the passenger
            passengersOnEle[i].drawPassenger(view); 
            // move the passengers onto the right spot
            passengersOnEle[i].xPos = 420 + i * 30;
            passengersOnEle[i].yPos = currPos-step;
            
            // get off the elevator
            if ( time - 1 == passengersOnEle[i].getAriveTime() )
            {
                // move passenger off the elevator
                passengersOnEle[i].xPos = 200 + i * 30;
                passengersOnEle[i].setServiced(true);
                // push onto the other vector and remove from this vector
                passengersOffEle.push_back(passengersOnEle[i]);
                passengersOnEle.erase(passengersOnEle.begin() + i);
            }
        }

        // ******************** move and stop the elevator ********************
        handleElevator(data);
        
    }
}