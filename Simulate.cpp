//
//  Simulate the elevator
//

#include "ECGraphicViewImp.h"
#include "Elevator.h"
#include "ECElevatorSim.h"
#include "Passenger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;


// run the GUI
void animate(int numFloors, vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history, int time)
{
    // change the floor size based on the number of floors 
    int floorSize;
    if ( numFloors > 16 ) { floorSize = 75; }
    else if ( numFloors >= 11 && numFloors <= 16 ) { floorSize = 95; }
    else if ( numFloors <= 10 ) { floorSize = 155; }
    
    int widthWin = 1000;
    int heightWin = numFloors * floorSize + 60;
    ECGraphicViewImp view(widthWin, heightWin);
  
    // create an elevator observer and show it
    ElevatorObserver eleOb(view, numFloors, heightWin, history, time, floorSize);
    view.Attach(&eleOb);
    view.Show();
}


// helper function to get the numebr of floors and the time
vector<int> GetFloorsTime(ifstream& file)
{
    // skip the first line

    string line; 
    getline( file, line ); 
    if ( line[0] == '#') {getline( file, line );}
    
    vector<int> floorTimeData;

    // grab the values and put in vector
    int i = 0;     
    while (i < line.length())
    {
        // create a string that will be each integer from the original string
        string str_int;

        while ( line[i] != ' ' && line[i] != '\0' )
        { str_int += line[i]; i++; }
        // add to the vector in integer form and increase i
        int new_int = stoi(str_int);
        floorTimeData.push_back(new_int);
        i++;
    }
    return floorTimeData;
}


// helper function to generate a vector of requests
vector<ECElevatorSimRequest> GetRequests(ifstream& file)
{
    // skip the first two lines
    string line;
    vector<ECElevatorSimRequest> requests;
    while (getline(file, line)) 
    {
        vector<int> values;
        // grab the values and put in vector
        int i = 0;     
        while (i < line.length())
        {
            // create a string that will be each integer from the original string
            string str_int;

            while ( line[i] != ' ' && line[i] != '\0' )
            { str_int += line[i]; i++; }
            // add to the vector in integer form and increase i
            int new_int = stoi(str_int);
            values.push_back(new_int);
            i++;
        }
        // create a new passanger and push to request list
        ECElevatorSimRequest req(values[0], values[1], values[2]);
        requests.push_back(req);
    }
    return requests;
}




int main(int argc, char **argv)
{
    // make sure we take in one argument otherwise return error
    if ( argc != 2 )
    { cerr << "Please input a file" << endl; return 1; }

    // get the filename and open the file
    string filename = argv[1]; ifstream file( filename );
    // if can't open file return error
    if ( !file )
    { cerr << "Error: Could not open file " << filename << endl; return 1; }

    // get the number of floors and the time
    vector<int> floorsTime;
    floorsTime = GetFloorsTime(file);

    // create a vector of elevator requests
    vector<ECElevatorSimRequest> requests;
    requests = GetRequests(file);

    // close the file
    file.close();

    // sort by time
    sort(requests.begin(), requests.end(), [](const ECElevatorSimRequest& a, const ECElevatorSimRequest& b) { return a.GetTime() < b.GetTime(); });

    // run through the simulation 
    ECElevatorSim sim(floorsTime[0], requests );
    sim.Simulate(floorsTime[1]);
    vector<tuple<int, int, string, EC_ELEVATOR_DIR, vector<ECElevatorSimRequest>>> history = sim.GetHistory();            

    // animate the simulation
    animate(floorsTime[0], history, floorsTime[1]);

    return 0;
}