#include <iostream>
#include "repairBay.h"

using namespace std;

///display
void repairBay::display()
{
    cout << "Available? ";
    if (isAvailable)
        cout << "yes";
    else
        cout << "no";
    cout << "\tTime left for repair job: ";
    if (counter != 0)
        cout << counter;
    else
        cout << "empty";
    cout << "\nCurrent vehicle:\n";
    if (isAvailable)
        cout << "empty\n";
    else
        currentVehicle->display();
}

///process vehicle in the repair bay
void repairBay::processVehicle(list<vehicle*>& junkyard, vector<int>& supply)
{
    if (currentVehicle->getBrokenParts() > 0)
    {
        list<int> partList = currentVehicle->getBrokenPartList();
        list<int>::iterator pItr = partList.begin();
        list<int> tempList;
        int currentPart = 0;

        //print for record
        cout << endl;
        int num = partList.size();
        cout << "number of broken parts = " << num << endl;
        num = 0;
        while (pItr != partList.end())
        {
            cout << "partList[" << num << "] = " << *pItr << "   ";
            num++;
            pItr++;
            if (pItr == partList.end()) {cout << endl;}
        }

        //loop through the list of broken parts
        pItr = partList.begin();
        while (pItr != partList.end())
        {
            currentPart = *pItr;
            bool found = false;

            cout << "\n***Searching junkyard for part " << currentPart << "***\n";

            //loop through the junkyard vehicles to find the part
            list<vehicle*>::iterator jyItr = junkyard.begin();
            while (jyItr!=junkyard.end())
            {
                found = false;

                //print for record
                (*jyItr)->display();

                //loop through the junk v's good parts list
                list<int> jyPartList = (*jyItr)->getGoodPartList();
                list<int>::iterator jyPartItr = jyPartList.begin();
                while (jyPartItr != jyPartList.end())
                {
                    //part found
                    if ((*jyPartItr) == currentPart)
                    {
                        cout << "--MATCH. ";
                        //remove the part from the junk vehicle's good parts list
                        jyPartList.erase(jyPartItr);
                        cout << "JUNK V'S PART REMOVED--\n";
                        //if there are no good parts left destroy it
                        num = (*jyItr)->getGoodParts();
                        num--;
                        if (num == 0)
                        {
                            jyItr = junkyard.erase(jyItr);
                            cout << "--JUNK V DESTROYED--";
                        }
                        //else update the junk vehicle's good parts list
                        else
                        {
                            (*jyItr)->setGoodParts(num);
                            (*jyItr)->setGoodPartList(jyPartList);
                            cout << "List of good parts: ";
                            tempList = (*jyItr)->getGoodPartList();
                        }
                        while (!tempList.empty())
                        {
                            cout << tempList.front() << " ";
                            tempList.pop_front();
                        }
                        cout << endl;
                        //remove the part from the current v's broken parts
                        num = currentVehicle->getBrokenParts();
                        currentVehicle->setBrokenParts(num-1);
                        pItr = partList.erase(pItr);
                        pItr--;
                        currentVehicle->setBrokenPartList(partList);
                        cout << "--CURRENT V'S BAD PART REMOVED ";
                        //add the part to the current v's good parts list
                        num = currentVehicle->getGoodParts();
                        currentVehicle->setGoodParts(num+1);
                        tempList = currentVehicle->getGoodPartList();
                        tempList.push_front(currentPart);
                        currentVehicle->setGoodPartList(tempList);
                        cout << "AND ADDED TO GOOD PARTS--\n"
                             << "List of good parts: ";
                        while (!tempList.empty())
                        {
                            cout << tempList.front() << " ";
                            tempList.pop_front();
                        }
                        cout << endl;
                        //exit the junk yard parts list
                        jyPartItr = jyPartList.end();
                        //exit the junkyard vehicles list
                        jyItr = junkyard.end();
                        //set found to true
                        found = true;
                    }

                    //else, part not found
                    else
                        jyPartItr++;
                }//end junk part list
                if (!found)
                    jyItr++;
            }//end junk vehicles list

            //if every junk vehicle was searched and the part is not found
            if (jyItr==junkyard.end() && !found)
            {
                cout << "--Part " << currentPart << " not found in junkyard--\n";

                //search the supply depot for it
                cout << "***Searching supply depot***\n";
                for (int i=1; i<=45; i++)
                    if (i==currentPart && supply[i]!=0) //found in depot
                    {
                        supply[i]--;
                        cout << "--Taking part " << currentPart << " from supply depot--\n"
                             << "*[" << i << "] = " << supply[i] << " remain*\n";
                        found = true;
                        break;
                    }

                //if found
                if (found)
                {
                    //remove the part from the current v's broken parts
                    num = currentVehicle->getBrokenParts();
                    currentVehicle->setBrokenParts(num-1);
                    tempList = currentVehicle->getBrokenPartList();
                    pItr = partList.erase(pItr);
                    pItr--;
                    currentVehicle->setBrokenPartList(partList);
                    //add the part to the v's good parts list
                    num = currentVehicle->getGoodParts();
                    currentVehicle->setGoodParts(num+1);
                    tempList = currentVehicle->getGoodPartList();
                    tempList.push_front(currentPart);
                    currentVehicle->setGoodPartList(tempList);
                }

                //if still not found
                else
                    cout << "--Part " << currentPart << " not found in depot--\n";
            }
            //if found, increase the counter
            if (found)
                counter += currentPart;

            pItr++;
        }//end parts loop
    }

    //decrease counter by the repair rate (5)
    counter -= repairRate;
    //prevent it from going below 0
    if (counter < 0)
        counter = 0;
}
