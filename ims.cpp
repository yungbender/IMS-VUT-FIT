#include <iostream>
#include <vector>
#include <simlib.h>

long long unsigned landfillPlastics = 6790000000;
long long unsigned bioPlatics = 0;
long long unsigned bioDiesel = 0;
long long unsigned bioPetrol = 0;
long long unsigned bioTurboDiesel = 0;
double rawHemp = 0;

Store DieselFactories(12);
Store FiberFactories(10000);

#define LOWEST_PLASTICS_OIL 13
#define HIGHEST_PLASTICS_OIL 15

class DieselFactoryProc : public Process
{
public:
    void Behavior()
    {
        while(landfillPlastics > 0)
        {
            
            if(landfillPlastics < 13)
            {
                landfillPlastics = 0;
            }
            else
            {
                RandomSeed(time(NULL));
                landfillPlastics -= Uniform(LOWEST_PLASTICS_OIL, HIGHEST_PLASTICS_OIL);
            }

            Enter(DieselFactories, 1);

            Wait(Uniform(28, 31));

            RandomSeed(time(NULL));
            bioDiesel += Uniform(250, 300);
            bioTurboDiesel += Uniform(80, 100);
            bioPetrol += Uniform(40, 50);

            Leave(DieselFactories, 1);
        }
    }
};

class CBDFarmProc : public Process
{
public:
    void Behavior()
    {
        Wait(Uniform(70, 140));

        rawHemp += 4.6;
    }
};

class IndustrialFarmProc : public Process
{
public:
    void Behavior()
    {
        Wait(Uniform(70, 140));

        rawHemp += 13.5;
    }
};

int main()
{
    Init(0, 10000000);

    for(int i = 0; i < 15; i++)
    {
        (new DieselFactoryProc)->Activate();
    }
    Run();
    DieselFactories.Output();
    std::cout << "Trvalo to: " << Time << " dní\n";
    std::cout << bioDiesel;
    return 0;   
}
