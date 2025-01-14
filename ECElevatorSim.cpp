
//
//  ECElevatorSim.cpp
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation


#include "ECElevatorSim.h"
#include <cmath>
#include <memory>

using namespace std;


/*
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) 
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
*/








// handle the simulation of the elevator

// set the constructor and intialize
ECElevatorSim :: ECElevatorSim(int numFloors, vector<ECElevatorSimRequest> &listRequests) 
: numFloors(numFloors), currDirct(EC_ELEVATOR_STOPPED), currFloor(1), currState(make_unique<ECElavtorStop>()), 
currTime(1), allRequests(listRequests) {}


// delete the state for the deconstructor
ECElevatorSim :: ~ECElevatorSim() {};


// simulate the elevator
void ECElevatorSim :: Simulate(int lenSim)
{
    // only loop through time for the length of the simulation
    for (int t = 0; t <= lenSim; t++)
    {
        cout << "\nTime: " << currTime << "\n------\n" << endl;
        // set the current time to the loop time
        currTime = t;
        // execute the current stat'es operation
        currState->CurExecution(*this, allRequests);
        // see if there is a state change needed
        currState->ChangeState(*this, allRequests);

        // add to the history
        string state;
        auto& stateRef = *currState;
        if (typeid(stateRef) == typeid(ECElavtorStop)) { state = "Stopped"; } 
        else if (typeid(stateRef) == typeid(ECElavtorLoad_Unload)) { state = "Loading/Unloading"; } 
        else if (typeid(stateRef) == typeid(ECElavtorMoving)) { state = "Moving"; }
        tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>> hisTime (currTime, currFloor, state, currDirct, allRequests);
        history.push_back(hisTime);
    }
}


// get the number of floors
int ECElevatorSim :: GetNumFloors() const { return numFloors; }

// get current floor
int ECElevatorSim :: GetCurrFloor() const { return currFloor; }

// set current floor
void ECElevatorSim :: SetCurrFloor(int f) { currFloor = f; }

// get current direction
EC_ELEVATOR_DIR ECElevatorSim :: GetCurrDir() const { return currDirct; }

// set the current direction
void ECElevatorSim :: SetCurrDir(EC_ELEVATOR_DIR dir) { currDirct = dir; }

// set the current state
void ECElevatorSim :: SetCurrState(unique_ptr<ECElevatorState> newState) { currState = std::move(newState); }

// get the current time
int ECElevatorSim :: GetCurrTime() const { return currTime; }

// get the history
vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> ECElevatorSim :: GetHistory() const { return history; }

//**********************************************************************************************************************

// helper function to find the closest floor for when the elevator
int findClosestFloor(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{
    // if multiple requests at the same time, go to the nearest floor
    // if there is a tie, i.e. two requests from the same distance to the current floor but on different side, prefer going up
    
    int currentFloor = elevator.GetCurrFloor();
    int closestFloor = -1;
    int minDist = elevator.GetNumFloors();

    // loop through the requests to find the closest floor
    for ( auto &request : requests )
    {   
        // make sure the request is not serviced already
        if ( request.IsServiced() == false )
        {
            // get that request's requested floor and calculate the distance 
            int requestedFloor = request.GetRequestedFloor();
            int distance = abs(requestedFloor - currentFloor);

            // determine a closer request is found or if the same distance but one is going up
            if ( (distance < minDist || ( distance == minDist && 
            ((request.GetRequestedFloor()  > elevator.GetCurrFloor()  && elevator.GetCurrDir() == EC_ELEVATOR_UP) || 
            (request.GetRequestedFloor()  < elevator.GetCurrFloor()  && elevator.GetCurrDir() == EC_ELEVATOR_DOWN )) )  ) 
                && request.GetTime() <= elevator.GetCurrTime() && request.GetRequestedFloor() != elevator.GetCurrFloor() )
            {
                closestFloor = requestedFloor;
                minDist = distance;
            }
        }
    }
    return closestFloor;
} 

// handle if the elevator is in a stopped state
void ECElavtorStop :: CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{
    cout << "Elevator current state is Stopped" << endl;
    
    // if there is a floor request move toward that floor

    // if there are no requests just keep the elevator in a stopped state
    if ( requests.empty() == true )
    {
        elevator.SetCurrDir(EC_ELEVATOR_STOPPED);
        return;
    }
    
    // find the closest floor request based on the current floor we're at
    int currentFloor = elevator.GetCurrFloor();
    int closestFloor = findClosestFloor(elevator, requests);

    cout << "The current floor is: " << currentFloor << endl;

    // set the direction of the elevator based on the request
    cout << "All requests: " << endl;
    for ( auto request : requests )
    {
        cout << "Request Time: " << request.GetTime() 
        << " | Floor: " << request.GetRequestedFloor() 
        << " | Finish Time: " << request.GetArriveTime()
        << " | Serviced: " << request.IsServiced()
        << " | Request Done: " << request.IsFloorRequestDone() << endl;
    }
    cout << "***********" << endl;
    cout << "The closest floor is: " << closestFloor << endl;
    

    cout << "The elevator is now ";

    if ( closestFloor == -1 )
    {
        cout << "stopped" << endl;
        return;
    }
    else if ( closestFloor > currentFloor )
    {
        elevator.SetCurrDir(EC_ELEVATOR_UP);
        cout << "moving up" << endl;
    }
    else if (closestFloor < currentFloor )
    {
        elevator.SetCurrDir(EC_ELEVATOR_DOWN);
        cout << "moving down" << endl;
    }
}

void ECElavtorStop :: ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{   
    // change the state to be moving only if the request meets the current time and is not serviced 
    for ( auto &request : requests )
    {
        if ( request.GetTime() == elevator.GetCurrTime() && request.IsServiced() == false )
        {   
            // if not on that floor move
            if ( request.GetRequestedFloor() != elevator.GetCurrFloor() )
            {
                cout << "Changing elevator state to moving" << endl;
                cout << "\n------------------------------------- \n \n";
                elevator.SetCurrState( make_unique<ECElavtorMoving>() );
            }
            // if on that floor change to load/unload 
            else if ( request.GetRequestedFloor() == elevator.GetCurrFloor() )
            {
                cout << "Changing elevator state to loading/unloading" << endl;
                cout << "\n------------------------------------- \n \n";
                elevator.SetCurrState( make_unique<ECElavtorLoad_Unload>() ); 
            }
        }
    }  
}

//**********************************************************************************************************************

// handle if the elevator is moving
void ECElavtorMoving :: CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{   
    cout << " Elevator in moving state" << endl;
    
    // change the current floor based on the elevator direction
    if ( elevator.GetCurrDir() == EC_ELEVATOR_UP )
    {
        elevator.SetCurrFloor( elevator.GetCurrFloor() + 1 );
    }
    else if ( elevator.GetCurrDir() == EC_ELEVATOR_DOWN )
    {
        elevator.SetCurrFloor( elevator.GetCurrFloor() - 1 );
    }

    cout << "The current floor is: " << elevator.GetCurrFloor() << endl;

    cout << "All requests: " << endl;
    for ( auto request : requests )
    {
        cout << "Request Time: " << request.GetTime() 
        << " | Floor: " << request.GetRequestedFloor() 
        << " | Finish Time: " << request.GetArriveTime()
        << " | Serviced: " << request.IsServiced()
        << " | Request Done: " << request.IsFloorRequestDone() << endl;
    }
    cout << "***********" << endl;

    // handle if the floor request is done if so then set the arrival time
    for ( auto &request : requests)
    {
        if ( request.IsFloorRequestDone() == true && request.GetRequestedFloor() == elevator.GetCurrFloor() )
        {
            request.SetArriveTime( elevator.GetCurrTime() );
            cout << "The arrival time: " << request.GetArriveTime() << endl;
        }
    }
}

void ECElavtorMoving :: ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{    
    for ( auto &request : requests )
    {
        // check if the request is serviced yet and if the request's floor request is the current floor
        if ( request.GetRequestedFloor() == elevator.GetCurrFloor() && request.IsServiced() == false && request.GetTime() <= elevator.GetCurrTime() )
        {
            cout << "Changing elevator state to unloading/loading" << endl;
            cout << "\n------------------------------------- \n \n";
            elevator.SetCurrState( make_unique<ECElavtorLoad_Unload>() );
            return;
        }
    }
}




//**********************************************************************************************************************




// handle if the elevator is loading/unloading
void ECElavtorLoad_Unload :: CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{   
    cout << " Elevator in load/unload state" << endl;
    cout << "The current floor is: " << elevator.GetCurrFloor() << endl;

    cout << "All requests: " << endl;
    for ( auto request : requests )
    {
        cout << "Request Time: " << request.GetTime() 
        << " | Floor: " << request.GetRequestedFloor() 
        << " | Finish Time: " << request.GetArriveTime()
        << " | Serviced: " << request.IsServiced()
        << " | Request Done: " << request.IsFloorRequestDone() << endl;
    }
    cout << "***********" << endl;
    
    // loop through all the requests and check if the request has already been serviced at that floor to unload/load
    for ( auto &request : requests )
    {   
        if ( request.IsServiced() == false && elevator.GetCurrFloor() == request.GetRequestedFloor() && request.GetTime() < elevator.GetCurrTime() )
        {    
            // load the passengers if they are waiting on this floor from a request 
            // set the request to be done
            if ( request.IsFloorRequestDone() == false ) { request.SetFloorRequestDone( true ); }
            // set the request as serviced
            else { request.SetServiced( true ); }
        }
    }
}


vector<bool>stillReq( ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests )
{
    // check if there are still requests in the current direction 
    int currentFloor = elevator.GetCurrFloor();
    bool stillReqUp = false;
    bool stillReqDown = false;

    for (const auto& request : requests)
    {
        if (request.IsServiced() == false && request.GetTime() <= elevator.GetCurrTime())
        {
            if (request.GetRequestedFloor() > currentFloor) { stillReqUp = true; }
            if (request.GetRequestedFloor() < currentFloor) { stillReqDown = true; }
        }
    }
    vector<bool> returnVec;
    returnVec.push_back(stillReqUp);
    returnVec.push_back(stillReqDown);

    // return a vector of the booleans
    return returnVec;
}

void ECElavtorLoad_Unload :: ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests)
{
    cout << "All requests: " << endl;
    for ( auto request : requests )
    {
        cout << "Request Time: " << request.GetTime() 
        << " | Floor: " << request.GetRequestedFloor() 
        << " | Finish Time: " << request.GetArriveTime()
        << " | Serviced: " << request.IsServiced()
        << " | Request Done: " << request.IsFloorRequestDone() << endl;
    }
    cout << "***********" << endl << endl;
    
    // find the closest floor request based on the current floor we're at
    int currentFloor = elevator.GetCurrFloor();
    int closestFloor = findClosestFloor(elevator, requests);

    // find out if there are any requests going in the same direction we were previously
    vector<bool> stillReqVal = stillReq( elevator, requests);

    cout << "The closest floor is: " << closestFloor << endl;

    cout << "The elevator is ";

    // continue in the same direction if there are pending requests in that direction
    if ( elevator.GetCurrDir() == EC_ELEVATOR_UP && stillReqVal[0] )
    {
        cout << " now moving up | changing state to moving" << endl;
        elevator.SetCurrDir(EC_ELEVATOR_UP);
        elevator.SetCurrState(make_unique<ECElavtorMoving>());
    }
    else if (elevator.GetCurrDir() == EC_ELEVATOR_DOWN && stillReqVal[1])
    {
        cout << " now moving down | changing state to moving" << endl;
        elevator.SetCurrDir(EC_ELEVATOR_DOWN);
        elevator.SetCurrState(make_unique<ECElavtorMoving>());
    }

    // otherwise choose a new direction based on the closest floor
    else if (closestFloor == -1) 
    {
        cout << " now stopped | changing state to stopped" << endl;
        elevator.SetCurrDir(EC_ELEVATOR_STOPPED);
        elevator.SetCurrState(make_unique<ECElavtorStop>());
    } 
    else if (closestFloor > currentFloor) 
    {
        cout << " now moving up | changing state to moving" << endl;
        elevator.SetCurrDir(EC_ELEVATOR_UP);
        elevator.SetCurrState(make_unique<ECElavtorMoving>());
    } 
    else if (closestFloor < currentFloor) 
    {
        cout << " now moving down | changing state to moving" << endl;
        elevator.SetCurrDir(EC_ELEVATOR_DOWN);
        elevator.SetCurrState(make_unique<ECElavtorMoving>());
    }
}