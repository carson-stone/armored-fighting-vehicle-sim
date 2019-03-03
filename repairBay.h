#ifndef REPAIRBAY_H
#define REPAIRBAY_H
#include <vector>
#include "vehicle.h"

///repairBay class
class repairBay
{
    public:
        repairBay(){isAvailable=true; counter=0;
                    repairRate=5; currentVehicle=nullptr;}
        ~repairBay(){delete currentVehicle; currentVehicle=0;}
        void display();
        void processVehicle(list<vehicle*>&, vector<int>&);
        void reset(){isAvailable=true; counter=0; currentVehicle=nullptr;}
        //get and set
        bool getAvailable(){return isAvailable;}
        void setAvailable(bool a){isAvailable=a;}
        int getCounter(){return counter;}
        void setCounter(int c){counter=c;}
        int getRepairRate(){return repairRate;}
        void setRepairRate(int r){repairRate=r;}
        vehicle* getCurrentVehicle(){return currentVehicle;}
        void setCurrentVehicle(vehicle* vPtr){currentVehicle=vPtr;}

    private:
        bool isAvailable;
        int counter;
        int repairRate;
        vehicle* currentVehicle;
};

///non-member functions related to repair bays

#endif // REPAIRBAY_H
