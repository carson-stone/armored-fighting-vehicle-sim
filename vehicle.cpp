#include <iostream>
#include <iomanip>
#include "vehicle.h"

using namespace std;

///create vehicles in the vehicle list
vehicle* createVehicle(int vehicleType, list<vehicle*>& vList)
{
    vehicle* vPtr = 0;

    switch (vehicleType)
    {
        case 1:
            vPtr = new tank1;
            break;
        case 2:
            vPtr = new tank2;
            break;
        case 3:
            vPtr = new tank3;
            break;
        case 4:
            vPtr = new wheel1;
            break;
        case 5:
            vPtr = new wheel2;
            break;
        case 6:
            vPtr = new halftrack;
            break;
        default:
            break;
    }
    vList.push_back(vPtr);
    return vPtr;
}

///make the vehicle's parts
void makeParts(vehicle* vPtr, default_random_engine* gPtr,
               normal_distribution<double> partDist)
{
    int val = 0;

    //create a random number of parts that will be broken
    do
    {
        val = partDist(*gPtr); //partDist depends on tank, wheeled, or HT
    } while (val < 1); //every vehicle must be broken
    vPtr->setBrokenParts(val);

    //set the IDs of each broken part based on its vehicle type
        //variable declarations
    int* possiblePartIDs = nullptr;
    int* brokenPartIDs = nullptr;
    int* goodPartIDs = nullptr;
    uniform_int_distribution<int> tankIndices(0,10); //tank parts have 11 different IDs
    uniform_int_distribution<int> wheelIndices(0,9); //wheel parts have 10 different IDs
    uniform_int_distribution<int> halftrackIndices(0,14); //wheel parts have 15 different IDs
    uniform_int_distribution<int>* useThisSet = 0;
    int index = 0, leftover = 0, sz = 0;
    bool matched = false;

    switch (vPtr->getID()) //switch on vehicle type
    {
        case 1:
            possiblePartIDs = new int[11];
            sz = 11;
            for (int i=1; i<12; i++)
                possiblePartIDs[i-1] = i;
            useThisSet = &tankIndices;
            leftover = 11 - val; //leftover = number of good parts
            break;
        case 2:
            possiblePartIDs = new int[11];
            sz = 11;
            for (int i=1; i<12; i++)
                possiblePartIDs[i-1] = i*2;
            useThisSet = &tankIndices;
            leftover = 11 - val; //leftover = number of good parts
            break;
        case 3:
            possiblePartIDs = new int[11];
            sz = 11;
            for (int i=1; i<12; i++)
                possiblePartIDs[i-1] = i*3;
            useThisSet = &tankIndices;
            leftover = 11 - val; //leftover = number of good parts
            break;
        case 4:
            possiblePartIDs = new int[10];
            sz = 10;
            for (int i=31; i<41; i++)
                possiblePartIDs[i-31] = i;
            useThisSet = &wheelIndices;
            leftover = 10 - val; //leftover = number of good parts
            break;
        case 5:
            possiblePartIDs = new int[10];
            sz = 10;
            for (int i=36; i<46; i++)
                possiblePartIDs[i-36] = i;
            useThisSet = &wheelIndices;
            leftover = 10 - val; //leftover = number of good parts
            break;
        case 6:
            possiblePartIDs = new int[15];
            sz = 15;
            for (int i=1; i<6; i++)
                possiblePartIDs[i-1] = i;
            for (int i=31; i<41; i++)
                possiblePartIDs[i-31+5] = i;
            useThisSet = &halftrackIndices;
            leftover = 15 - val; //leftover = number of good parts
            break;
        default:
            break;
    }

    val = vPtr->getBrokenParts(); //number of broken parts
    brokenPartIDs = new int[val];

    //create broken parts
    for (int i=0; i<val; i++)
        do
        {
            index = (*useThisSet)(*gPtr); //random number in the appropriate set
            brokenPartIDs[i] = possiblePartIDs[index];
            if (i == 0) //don't check the first part for a match
                break;

            for (int j=0; j<i; j++) //prevent repeats
            {
                if (brokenPartIDs[i] == brokenPartIDs[j])
                {
                    matched = true;
                    break;
                }
                else
                    matched = false;
            }
        } while (matched == true);

    vPtr->setBrokenPartList(brokenPartIDs);

    for (int i=0; i<sz; i++) //don't reuse bad part IDs
        for (int j=0; j<val; j++)
            if (possiblePartIDs[i] == brokenPartIDs[j])
                possiblePartIDs[i] = 0;

    vPtr->setGoodParts(leftover);
    goodPartIDs = new int[leftover];

    //create good parts
    for (int i=0; i<leftover; i++)
        for (int j=i; j<sz; j++)
            if (possiblePartIDs[j] != 0)
            {
                goodPartIDs[i] = possiblePartIDs[j];
                possiblePartIDs[j] = 0; //don't reuse that part
                break;
            }

    vPtr->setGoodPartList(goodPartIDs);

    //cleanup
    delete possiblePartIDs;
    delete brokenPartIDs;
    delete goodPartIDs;
}

///yard master
void vehicle::yardMaster()
{
    int brokenScore = 0, totalScore = 0;
    list<int>::iterator partItr = brokenPartList.begin();

    //calculate the vehicle's broken parts score
    while (partItr != brokenPartList.end())
    {
        brokenScore += *partItr;
        partItr++;
    }

    //calculate the vehicle's total parts score
    partItr = goodPartList.begin();
    while (partItr != goodPartList.end())
    {
        totalScore += *partItr;
        partItr++;
    }
    totalScore += brokenScore;

    //set totaled based on if broken >= .4(total)
    if (brokenScore >= (.4*totalScore))
        totaled = true;

    //set time score to the broken score
    setTimeScore(brokenScore);
}

///set broken part list
void vehicle::setBrokenPartList(int* a)
{
    this->brokenPartList.clear(); //clear the old list

    for (int i=0; i<this->brokenParts; i++)
        brokenPartList.push_back(a[i]);
}

///set broken part list (with a list)
void vehicle::setBrokenPartList(list<int> pList)
{
    this->brokenPartList.clear(); //clear the old list

    brokenPartList = pList;
}

///set good part list (with an array)
void vehicle::setGoodPartList(int* a)
{
    this->goodPartList.clear(); //clear the old list

    for (int i=0; i<this->goodParts; i++)
        goodPartList.push_back(a[i]);
}

///set good part list (with a list)
void vehicle::setGoodPartList(list<int> pList)
{
    this->goodPartList.clear(); //clear the old list

    goodPartList = pList;
}

///display
void vehicle::display()
{
    if (this->name != "") //any type except base class
    {
        list<int>::iterator pItr = brokenPartList.begin();
        cout << left << setw(6) << name << " with " << brokenParts
             << " broken parts ( ";
        while (pItr != brokenPartList.end())
        {
            cout << *pItr << " ";
            pItr++;
        }
        cout << ") and " << goodParts << " good parts ( ";
        pItr = goodPartList.begin();
        while (pItr != goodPartList.end())
        {
            cout << *pItr << " ";
            pItr++;
        }
        cout << ")\n";
    }
}

