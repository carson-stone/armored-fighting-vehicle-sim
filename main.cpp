#include "vehicle.h"
#include "repairBay.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

using namespace std;

/*
TITLE: Junk Vehicle Simulation (project 1)
DESCRIPTION: This program simulates a military garage that creates broken
  vehicles, sorts them into a junkyard and a repair queue, and then fixes
  the vehicles. It then tests them to make sure their parts are good, and
  ultimately moves out (destroys) the vehicle once it is fixed.
CLASS: CS 361 , Professor Jay Morris
AUTHOR: Carson Stone , cston001@odu.edu
*/


///to prioritize vehicle pointers in the PQ
struct compare{
    bool operator() (vehicle* left, vehicle* right)
        {return (*left < *right);}};


///Function prototypes
void printRepairQ(queue<vehicle*>&);
void printPriorityQ(priority_queue<vehicle*,vector<vehicle*>,compare>);
void printSupply(vector<int>);
void printJunkyard(list<vehicle*>&);
void initializeTestSites(vector<vehicle*>&);
void addToTestSite(vector<vehicle*>&, vehicle*, priority_queue<vehicle*,vector<vehicle*>,compare>&);
void initializeSupplyDepot(vector<int>&, default_random_engine*, uniform_int_distribution<int>&,
                            uniform_int_distribution<int>);
void initializeJunkyard(list<vehicle*>& junkyard, default_random_engine*, const normal_distribution<double>&,
                         const normal_distribution<double>&);
void addToSupplyDepot(vector<int>& supply, default_random_engine*, uniform_int_distribution<int>&,
                       uniform_int_distribution<int>);


///Main
/////////////////////////////////////////////////////////////////////////
int main()
{
    //open the output file, "vehicles.out"
    ofstream outFile;
    outFile.open("vehicles.out");
    if (!outFile)
    {
        cerr << "ERROR OPENING OUT FILE\n";
        terminate();
    }
    // Backup streambuffers of  cout
    streambuf* stream_buffer_cout = cout.rdbuf();
    streambuf* stream_buffer_cin = cin.rdbuf();
    // Get the streambuffer of the file
    streambuf* stream_buffer_outFile = outFile.rdbuf();
    // Redirect cout to file
    cout.rdbuf(stream_buffer_outFile);


    //variable declarations
    list<vehicle*> vList, junkyard;
    queue<vehicle*> repairQ;
    priority_queue<vehicle*,vector<vehicle*>,compare> PQ;
    vector<repairBay> bays(3);
    vector<vehicle*> testSites(3);
    list<vehicle*>::iterator vItr = vList.begin();
    vehicle* vPtr = nullptr;
    default_random_engine* gPtr = new default_random_engine(time(nullptr));
    normal_distribution<double> tankDist(3,1); //mean=3, stdDev=1
    normal_distribution<double> wheelDist(5,2); //mean=5, stdDev=2
    normal_distribution<double> vGenerationDist(5,2); //mean=5, stdDev=2
    uniform_int_distribution<int> vTypeGeneration(1,6);
    int choice = 0, days = 0;
    //supply depot declarations
    vector<int> supply;
    uniform_int_distribution<int> supplyDist(2,4); //a random 90% will have 2-4 parts
    uniform_int_distribution<int> probabilityDist(1,100); //to create that 90%


    //initialize supply depot
    initializeSupplyDepot(supply,gPtr,probabilityDist,supplyDist);
    //initialize junkyard
    initializeJunkyard(junkyard,gPtr,tankDist,wheelDist);



    cout << "\n\n\n VEHICLE MANAGEMENT SYSTEM\n-----------------------------\n";
    // Redirect cout back to cout
    cout.rdbuf(stream_buffer_cout);
    cout << " VEHICLE MANAGEMENT SYSTEM\n-----------------------------\n DAYS TO SIMULATE: ";

    // Redirect cin back to cin
    cin.rdbuf(stream_buffer_cin);
    cin >> days;

    cout << "\n JUNKYARD INITIALIZED WITH 1 OF EACH VEHICLE TYPE\n"
         << " VEHICLES WITH 40% OF ITS PART TOTAL BEING BROKEN ARE SENT TO JUNKYARD\n"
         << " 5% OF PART TYPES ARE RESUPPLIED IN THE SUPPLY DEPOT EVERY DAY\n\n"
         << " DATA WRITTEN TO \"vehicles.out\"\n\n";
    outFile << " DAYS TO SIMULATE: " << days;

    // Redirect cout back to file
    cout.rdbuf(stream_buffer_outFile);
    cout <<"\n-----------------------------\n";

    //start simulation loop
    for (int i=1; i<=days; i++)
    {
        cout << " SIMULATING DAY " << i
        << "\n-----------------------------\n";
        //create a random amount of vehicles
        int numVs = vGenerationDist(*gPtr);
        if (numVs <= 0) {numVs = 1;} //we want at least one

        cout << "***MAKING " << numVs << " VEHICLES***\n";
        for (int i=0; i<numVs; i++)
        {
            //randomly generate the vehicle's type
            choice = vTypeGeneration(*gPtr);
            switch (choice)
            {
                case 1:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 2:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 3:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 4:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                case 5:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                case 6:
                    vPtr = createVehicle(choice,vList);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                default:
                    break;
            }
            cout << "Made "; //display the vehicle and parts info
            vPtr->display();

            //score with yardmaster and sort
            vPtr->yardMaster();
            if (vPtr->getTotaled() == true) //add to junk yard
            {
                junkyard.push_front(vPtr);
                cout <<"--Adding to junkyard--\n";
            }
            else //add to repair queue
            {
                repairQ.push(vPtr);
                cout <<"--Adding to repair queue--\n";
            }
        }//end of vehicle creation loop


        //place vehicles into repair bays
        priority_queue<vehicle*,vector<vehicle*>,compare> tempPQ; //restrict it from using broken v's
        for (int i=0; i<3; i++)
        {
            //if bay is empty, place vehicle into it
            if (bays[i].getAvailable())
            {
                //if there is a v in the PQ, use that
                if (!PQ.empty())
                {
                    cout << "\n***Adding from priority queue into bay "
                         << i+1 << "***";
                    vPtr = PQ.top();
                    bays[i].setCurrentVehicle(vPtr);
                    bays[i].setAvailable(false);
                    bays[i].processVehicle(junkyard,supply);
                    PQ.pop();
                }
                //else use v from the repair queue, if not empty
                else if (!repairQ.empty())
                {
                    cout << "\n***Adding from repair queue into bay "
                         << i + 1 << "***";
                    vPtr = repairQ.front();
                    bays[i].setCurrentVehicle(vPtr);
                    bays[i].setAvailable(false);
                    bays[i].processVehicle(junkyard,supply);
                    repairQ.pop();
                }
                else
                    cout << "\n***No vehicles to repair***\n";
            }
            //else process the vehicle in the bay
            else
            {
                cout << "\n***No new vehicle added to bay " << i+1 << ". Repairing current***\n";
                bays[i].processVehicle(junkyard,supply);
            }

            //if the bay is occupied
            vPtr = bays[i].getCurrentVehicle();
            if (vPtr != nullptr)
            {
                if (vPtr->getBrokenParts() == 0)
                    cout << "number of broken parts = 0" << endl;
                //if vehicle is fixed and there is no wait time, move to a test site
                if (vPtr->getBrokenParts()==0 && bays[i].getCounter()==0)
                {
                    cout << "***Moving vehicle to testing site***\n";
                    addToTestSite(testSites,vPtr,PQ);
                    bays[i].reset();
                    continue; //so the next if statement is not processed
                }
                //if vehicle is still broken and counter = 0, move it to the PQ
                if (vPtr->getBrokenParts()>0 && bays[i].getCounter()==0)
                {
                    cout << "***Moving vehicle to priority queue***\n";
                    tempPQ.push(vPtr);
                    bays[i].reset();
                }
            }
            else //else it's unoccupied
                cout << "***BAY " << i+1 << " IS EMPTY***\n";

        }//end repair bay loop

        //add the broken vehicles back into the PQ
        while (!tempPQ.empty())
        {
            vPtr = tempPQ.top();
            //reset its time score
            vPtr->yardMaster();
            tempPQ.pop();
            PQ.push(vPtr);
        }


        //testing fixed vehicles
        for (int i=0; i<3; i++)
        {
            if (testSites[i] == nullptr) //if empty
                cout << "\n***TESTING SITE " << i+1 << " IS EMPTY***\n";

            else //otherwise
            {
                vPtr = testSites[i];
                list<int> parts = vPtr->getGoodPartList();
                list<int> temp1, temp2;
                list<int>::iterator pItr = parts.begin();
                int good = 0, bad = 0;

                //print for record
                cout << "\n***TESTING SITE " << i+1 << "***\nList of good parts: ";
                while (pItr != parts.end())
                {
                    cout << *pItr << " ";
                    pItr++;
                }
                cout << endl;
                pItr = parts.begin();

                //test each good part in the current vehicle
                while (pItr != parts.end())
                {
                    //97% of parts pass
                    int prob = probabilityDist(*gPtr);
                    if (prob < 98)
                    {
                        cout << "--Part " << right << setw(2) << *pItr << " passed--\n";
                        temp1.push_front(*pItr);
                        good++;
                    }
                    //if it fails
                    else
                    {
                        cout << "--Part " << right << setw(2) << *pItr << " failed--\n";
                        //add the part to the broken parts list
                        temp2.push_front(*pItr);
                        vPtr->setBrokenParts(vPtr->getBrokenParts()+1);
                        bad++;
                    }
                    pItr++;
                }//end parts testing

                //set the lists
                vPtr->setGoodPartList(temp1);
                vPtr->setBrokenPartList(temp2);
                vPtr->setGoodParts(good);
                vPtr->setBrokenParts(bad);

                //its time score will equal 0 i f all parts passed
                vPtr->yardMaster(); //redo its time score since it's repaired
                if ((vPtr->getTimeScore()) == 0)
                {
                    //delete the vehicle
                    cout << "***This vehicles is fixed***\n";
                    delete testSites[i];
                }
                else
                {
                    //send to PQ
                    cout << "***Still broken. Sending to priority queue***\n";
                    PQ.push(vPtr);
                }
                testSites[i] = nullptr;
            }
        }//end testing loop



        //print status of simulation
        cout << "\n***Printing repair bay statuses***\n";
        for (int i=0; i<3; i++)
        {
            cout << "Repair bay "<< i+1 << ":\n";
            bays[i].display();
        }
        cout << "***End repair bay printout***\n";
        printRepairQ(repairQ);
        printPriorityQ(PQ);
        printJunkyard(junkyard);
        addToSupplyDepot(supply,gPtr,probabilityDist,supplyDist); //resupply the depot
        printSupply(supply);
        cout << "-----------------------------\n***END OF DAY " << i
             << "***\n-----------------------------\n\n";


        //clear the vehicle list
        vList.clear();
    }//end simulation loop

    cout << "\n\tFINISHED\n-----------------------------\n";


    outFile.close();

    return 0;
}
/////////////////////////////////////////////////////////////////////////
///End main



///Function definitions
///print repair queue
void printRepairQ(queue<vehicle*>& repairQ)
{
    int total = 0;
    queue<vehicle*> temp;

    cout << "\n***Printing vehicle repair queue***\n";
    while (!repairQ.empty())
    {
        vehicle* v = repairQ.front();
        v->display();
        temp.push(v);
        repairQ.pop();
        total++;
    }
    repairQ = temp;
    cout << "***There are " << total << " vehicles total***\n";
}

///print priority queue
void printPriorityQ(priority_queue<vehicle*,vector<vehicle*>,compare> PQ)
{
    int total = 0;

    cout << "\n***Printing priority queue***\n";
    while (!PQ.empty())
    {
        vehicle* vPtr;
        vPtr = PQ.top();
        vPtr->display();
        cout << "Time score = " << vPtr->getTimeScore() << endl;
        PQ.pop();
        total++;
    }
    cout << "***There are " << total << " vehicles total***\n";
}

///print supply depot
void printSupply(vector<int> supply)
{
    int perLine = 0;

    cout << "\n***Printing supply depot***\n";

    for (int i=1; i<=45; i++)
    {
        cout << "[" << i << "]: ";
        if (i <= 9) {cout << " ";}
        cout << supply[i] << " remain\t";
        perLine++;
        if (perLine == 5)
        {
            perLine = 0;
            cout << endl;
        }
    }

    cout << "***End supply depot printout***\n";
}

///print junkyard
void printJunkyard(list<vehicle*>& junkyard)
{
    list<vehicle*>::iterator vItr = junkyard.begin();
    int total = 0;

    cout << "\n***Printing junkyard***\n";
    while (vItr != junkyard.end())
    {
        (*vItr)->display();
        total++;
        vItr++;
    }
    cout << "***There are " << total << " vehicles total***\n";
}

///initialize testing sites
void initializeTestSites(vector<vehicle*>& TS)
{
    for (int i=0; i<3; i++)
        TS[i] = nullptr;
}

///initialize supply depot
void initializeSupplyDepot(vector<int>& supply, default_random_engine* gPtr,
                            uniform_int_distribution<int>& probabilityDist,
                             uniform_int_distribution<int> supplyDist)
{
    int temp = 0;

    for (int i=0; i<=45; i++)
    {
        //clear out 10% of the supply depot
        temp = probabilityDist(*gPtr);
        if (temp > 10)  //temp <= 10 for 10% of the iterations
            supply.push_back(supplyDist(*gPtr)); //= 2 to 4 parts
        else
            supply.push_back(0);
    }

    //set supply[0] to 0
        supply[0] = 0;

    //display for record
    cout << "***Supply depot initialized***";
    printSupply(supply);
}

///initialize junkyard
void initializeJunkyard(list<vehicle*>& junkyard, default_random_engine* gPtr,
                         const normal_distribution<double>& tankDist,
                          const normal_distribution<double>& wheelDist)
{
    vehicle* vPtr = nullptr;

    for (int i=1; i<2; i++) //create 1 of each vehicle type
        for (int j=1; j<=6; j++)
        {
            switch (j)
            {
                case 1:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 2:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 3:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,tankDist);
                    break;
                case 4:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                case 5:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                case 6:
                    vPtr = createVehicle(j,junkyard);
                    makeParts(vPtr,gPtr,wheelDist);
                    break;
                default:
                    break;
            }
        }

    //display for record
    cout << "\n***Junkyard initialized***";
    printJunkyard(junkyard);
}

///add parts to the supply depot
void addToSupplyDepot(vector<int>& supply, default_random_engine* gPtr,
                            uniform_int_distribution<int>& probabilityDist,
                             uniform_int_distribution<int> supplyDist)
{
    int temp = 0;

    cout << "\n***Adding to supply depot***\n";

    for (int i=1; i<=45; i++)
    {
        //add to a twentieth of the parts
        temp = probabilityDist(*gPtr);
        if (temp > 95)  //temp > 95 for 5% of the iterations
        {
            temp = supplyDist(*gPtr); //= 2 to 4 parts
            cout << "--" << temp << " added to [" << i << "]--\n";
        }
        else
            temp = 0;
        supply[i] += temp;
    }
    cout << "***End of resupply***\n";
}

///add a vehicle to a test site
void addToTestSite(vector<vehicle*>& testSites, vehicle* vPtr,
                            priority_queue<vehicle*,vector<vehicle*>,compare>& PQ)
{
    for (int i=0; i<3; i++)
        if ((testSites[i] == nullptr)) //i.e. empty
        {
            cout << "--Added to test site " << i+1 << "--\n";
            testSites[i] = vPtr;
            return;
        }
}
