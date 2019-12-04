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

class WatchDog : public Event
{
public:
    void Behavior()
    {
        if(landfillPlastics <= 0)
        {
            std::cout << "Koniec, plasty su spotrebovane";
            Abort();
        }
        else
        {
            std::cout << "checkujem, aktualny cas " << Time;
            std::cout << "\n aktualne plastov " << landfillPlastics;
        }
        

        Activate(Time + 500);
    }
};

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

class FiberFactoryProc : public Process
{
public:
    void Behavior()
    {
        Enter(FiberFactories, 1);

        std::cout << "idem robiť fiber\n";

        Wait(Uniform(0.08, 0.33));      
        bioPlatics += 1;
        rawHemp -= 1;

        Leave(FiberFactories, 1);
        std::cout << "fiber done\n" << Time << "\n";
    }
};

class CBDFarmProc : public Process
{
public:
    void Behavior()
    {
        while(true)
        {
            std::cout << "idem sadiť\n";
            Wait(Uniform(70, 140));
            std::cout << "dosadene\n";

            rawHemp += 4.6;

            (new FiberFactoryProc)->Activate();
        }
    }
};

class IndustrialFarmProc : public Process
{
public:
    void Behavior()
    {
        while(true)
        {
            std::cout << "idem sadiť\n";
            Wait(Uniform(70, 140));
            std::cout << "dosadene\n";

            rawHemp += 13.5;

            (new FiberFactoryProc)->Activate();
        }
    }
};

int main()
{
    Init(0, 200);

    (new WatchDog)->Activate();

    for(int i = 0; i < 15; i++)
    {
        (new DieselFactoryProc)->Activate();
    }

    for(int i = 0; i < 7233; i++)
    {
        (new CBDFarmProc)->Activate();
    }

    for(int i = 0; i < 16878; i++)
    {
        (new IndustrialFarmProc)->Activate();
    }


    Run();
    DieselFactories.Output();
    FiberFactories.Output();
    std::cout << "Trvalo to: " << Time << " dní\n";
    std::cout << "Diesel :" << bioDiesel << "\n";
    std::cout << "Petrol :" << bioPetrol << "\n";
    std::cout << "Turbodiesel :" << bioTurboDiesel << "\n";
    std::cout << "Za tu dobu sa stihlo urobit bioplastov:" << bioPlatics << "\n";
    std::cout << "Plastov na skladne zostalo:" << landfillPlastics << "\n";
    return 0;   
}
