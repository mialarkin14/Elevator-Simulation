//
//  ECElevatorSim.h
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation

#ifndef ECElevatorSim_h
#define ECElevatorSim_h

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <tuple>
#include <cmath>
#include <typeinfo>

using namespace std;

//*****************************************************************************
// DON'T CHANGE THIS CLASS
// 
// Elevator simulation request: 
// (i) time: when the request is made
// (ii) floorSrc: which floor the user is at at present
// (iii) floorDest floor: where the user wants to go; we assume floorDest != floorSrc
// 
// Note: a request is in three stages:
// (i) floor request: the passenger is waiting at floorSrc; once the elevator arrived 
// at the floor (and in the right direction), move to the next stage
// (ii) inside request: passenger now requests to go to a specific floor once inside the elevator
// (iii) Once the passenger arrives at the floor, this request is considered to be "serviced"
//
// two sspecial requests:
// (a) maintenance start: floorSrc=floorDest=-1; put elevator into maintenance 
// starting at the specified time; elevator starts at the current floor
// (b) maintenance end: floorSrc=floorDest=0; put elevator back to operation (from the current floor)

class ECElevatorSimRequest
{
public:
    ECElevatorSimRequest(int timeIn, int floorSrcIn, int floorDestIn) : time(timeIn), floorSrc(floorSrcIn), floorDest(floorDestIn), fFloorReqDone(false), fServiced(false), timeArrive(-1) {} 
    ECElevatorSimRequest(const ECElevatorSimRequest &rhs) : time(rhs.time), floorSrc(rhs.floorSrc), floorDest(rhs.floorDest), fFloorReqDone(rhs.fFloorReqDone), fServiced(rhs.fServiced), timeArrive(rhs.timeArrive) {}
    int GetTime() const {return time; }
    int GetFloorSrc() const { return floorSrc; }
    int GetFloorDest() const { return floorDest; }
    bool IsGoingUp() const { return floorDest >= floorSrc; }

    // Is this passenger in the elevator or not
    bool IsFloorRequestDone() const { return fFloorReqDone; }
    void SetFloorRequestDone(bool f) { fFloorReqDone = f; }

    // Is this event serviced (i.e., the passenger has arrived at the desstination)?
    bool IsServiced() const { return fServiced; }
    void SetServiced(bool f) { fServiced = f; }

    // Get the floor to service
    // If this is in stage (i): waiting at a floor, return that floor waiting at
    // If this is in stage (ii): inside an elevator, return the floor going to
    // Otherwise, return -1
    int GetRequestedFloor() const {
        if( IsServiced() )  {
            return -1;
        }
        else if( IsFloorRequestDone() )   {
            return GetFloorDest();
        }
        else {
            return GetFloorSrc();
        }
    }

    // Wait time: get/set. Note: you need to maintain the wait time yourself!
    int GetArriveTime() const { return timeArrive; }
    void SetArriveTime(int t) { timeArrive = t; }

    // Check if this is the special maintenance start request
    bool IsMaintenanceStart() const { return floorSrc==-1 && floorDest==-1; }
    bool IsMaintenanceEnd() const { return floorSrc==0 && floorDest==0; }

private:
    int time;           // time of request made
    int floorSrc;       // which floor the request is made
    int floorDest;      // which floor is going
    bool fFloorReqDone;   // is this passenger passing stage one (no longer waiting at the floor) or not
    bool fServiced;     // is this request serviced already?
    int timeArrive;     // when the user gets to the desitnation floor
};

//*****************************************************************************
// Elevator moving direction

typedef enum
{
    EC_ELEVATOR_STOPPED = 0,    // not moving
    EC_ELEVATOR_UP,             // moving up
    EC_ELEVATOR_DOWN            // moving down
} EC_ELEVATOR_DIR;

//*****************************************************************************
// Add your own classes here...



// forward declartion
class ECElevatorSim;
class ECElevatorState;


// a class for the elevator's state -> this is abstract
class ECElevatorState
{
  public:
    
    // deconstructor (virtual since it could change for subclasses)
    virtual ~ECElevatorState() = default;

    // abstract methods to determine what to do next
    // method that decides what to do at this time
    virtual void CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) = 0;
    // method that decides if there is a change of the state in the elevator
    virtual void ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) = 0;
};



// a class for when the elevator is stopped that public inherits from the abstract state class and overrides those methods
class ECElavtorStop : public ECElevatorState
{
    public:
        void CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
        void ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
};


// a class for when the elevator is moving that public inherits from the abstract state class and overrides those methods
class ECElavtorMoving : public ECElevatorState
{
    void CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
    void ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
};


// a class for when the elevator is unloading/loading that public inherits from the abstract state class and overrides those methods
class ECElavtorLoad_Unload : public ECElevatorState
{
    void CurExecution(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
    void ChangeState(ECElevatorSim &elevator, vector<ECElevatorSimRequest>& requests) override;
};





//*****************************************************************************
// Simulation of elevator

class ECElevatorSim
{
public:
    // numFloors: number of floors serviced (floors numbers from 1 to numFloors)
    ECElevatorSim(int numFloors, vector<ECElevatorSimRequest> &listRequests);

    // free buffer
    ~ECElevatorSim();

    // Simulate by going through all requests up to certain period of time (as specified in lenSim)
    // starting from time 0. For example, if lenSim = 10, simulation stops at time 10 (i.e., time 0 to 9)
    // Caution: the list of requests contain all requests made at different time;
    // at a specific time of simulation, some events may be made in the future (which you shouldn't consider these future requests)
    void Simulate(int lenSim);

    // The following methods are about querying/setting states of the elevator
    // which include (i) number of floors of the elevator, 
    // (ii) the current floor: which is the elevator at right now (at the time of this querying). Note: we don't model the tranisent states like when the elevator is between two floors
    // (iii) the direction of the elevator: up/down/not moving

    // Get num of floors
    int GetNumFloors() const;
    // Get current floor
    int GetCurrFloor() const;
    // Set current floor
    void SetCurrFloor(int f);
    // Get current direction
    EC_ELEVATOR_DIR GetCurrDir() const;
    // Set current direction
    void SetCurrDir(EC_ELEVATOR_DIR dir);
    // Set the current state
    void SetCurrState(unique_ptr<ECElevatorState> newState);
    // Get the current time
    int GetCurrTime() const;

    // Get the history of the elevator
    vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> GetHistory() const;


private:
    // the number of floors in the buidling
    int numFloors;
    // the current direction of the elevator
    EC_ELEVATOR_DIR currDirct;
    // the current floor the elevator's on
    int currFloor;
    // the current state of the elevator
    unique_ptr<ECElevatorState> currState;
    // a vector of all the requests
    vector<ECElevatorSimRequest>& allRequests;
    // the current time 
    int currTime; 
    // the history
    vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history;
};


#endif /* ECElevatorSim_h */
