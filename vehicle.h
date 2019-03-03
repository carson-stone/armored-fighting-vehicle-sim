#ifndef VEHICLE_H
#define VEHICLE_H
#include <list>
#include <queue>
#include <string>
#include <random>

using namespace std;

///vehicle base class
class vehicle
{
    public:
        vehicle(){ID=0; brokenParts=0; goodParts=0;
                  name=""; timeScore=0; totaled = false;}
        virtual ~vehicle(){}
        void yardMaster();
        void display();
        //to give the vehicle with the lowest time score highest priority
        bool operator< (const vehicle& right) const
            {return (timeScore > right.timeScore);}
        //get and set
        int getID(){return ID;}
        void setID(int id){ID=id;}
        int getBrokenParts(){return brokenParts;}
        void setBrokenParts(int bp){brokenParts=bp;}
        int getGoodParts(){return goodParts;}
        void setGoodParts(int gp){goodParts=gp;}
        int getTimeScore(){return timeScore;}
        void setTimeScore(int ts){timeScore=ts;}
        bool getTotaled(){return totaled;}
        void setTotaled(int t){totaled=t;}
        list<int> getBrokenPartList(){return brokenPartList;}
        void setBrokenPartList(int* a);
        void setBrokenPartList(list<int> pList);
        list<int> getGoodPartList(){return goodPartList;}
        void setGoodPartList(int* a);
        void setGoodPartList(list<int> pList);

    protected:
        int ID;
        int brokenParts;
        int goodParts;
        int timeScore;
        bool totaled;
        string name;
        list<int> brokenPartList;
        list<int> goodPartList;
};

///tank1 class
class tank1 : public vehicle
{
    public:
        tank1(){ID=1; name="tank1";}
        void display();
};

///tank2 class
class tank2 : public vehicle
{
    public:
        tank2(){ID=2; name="tank2";}
        void display();
};

///tank3 class
class tank3 : public vehicle
{
    public:
        tank3(){ID=3; name="tank3";}
        void display();
};

///wheel1 class
class wheel1 : public vehicle
{
    public:
        wheel1(){ID=4; name="wheel1";}
        void display();
};

///wheel2 class
class wheel2 : public vehicle
{
    public:
        wheel2(){ID=5; name="wheel2";}
        void display();
};


///half-track class
class halftrack : public vehicle
{
    public:
        halftrack(){ID=6; name="HT";}
        void display();
};

///non-member functions related to vehicles
vehicle* createVehicle(int vehicleType, list<vehicle*>& vList);
void makeParts(vehicle* vPtr, default_random_engine* gPtr,
               normal_distribution<double> partDist);

#endif // VEHICLE_H
