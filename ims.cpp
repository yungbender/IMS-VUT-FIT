#include <iostream>
#include <vector>
#include <unistd.h>
#include <simlib.h>

long long unsigned landfillPlastics = 6790000000;
double rawHemp = 0;
double cornFields = 0;
double cornGrain = 0;
double cornStover = 0;
long long unsigned dayWatch = 50;

Store DieselFactories(12);
Store FiberFactories(10000);
Store HarvestCompany(50);

Stat bioPlastics("Vyrobené bioplasty");
Stat bioDiesel("Vyrobený bio diesel");
Stat bioPetrol("Vyrobený bio petrolej");
Stat bioTurboDiesel("Vyrobený bio turbodiesel");

#define LOWEST_PLASTICS_OIL 13
#define HIGHEST_PLASTICS_OIL 15

class WatchDog : public Event
{
public:
    void Behavior()
    {
        std::cout << "Simulation ran for " << Time << " days (" << (Time / 365) << "years)\n";
        std::cout << "Currently there is " << bioPlastics.Sum() << " Tons of bioplastics \n";
        std::cout << "Currently there is " << bioDiesel.Sum() << " Litres of biodiesel \n";
        std::cout << "Currently there is " << bioPetrol.Sum() << " Litres of biopetrol \n";
        std::cout << "Currently there is " << bioTurboDiesel.Sum() << " Litres of bioturbodiesel \n";
        std::cout << "Currently there is " << landfillPlastics << " Tons of plastics left on landfill" << "\n\n";
        Activate(Time + dayWatch);
    }
};

class DieselFactoryProc : public Process
{
public:
    void Behavior()
    {
        // Until there is landfillPlastics or end of simulation
        while(landfillPlastics > 0)
        {
            if(landfillPlastics < 13)
            {
                // Take the remaining plastics
                landfillPlastics = 0;
            }
            else
            {
                // Roll between 13 - 15 Tons of plastics, that it take to create oils
                landfillPlastics -= Uniform(LOWEST_PLASTICS_OIL, HIGHEST_PLASTICS_OIL);
            }

            Enter(DieselFactories, 1);
            
            // It takes over month to create oil
            Wait(Uniform(28, 31));

            // 250 - 300 litres of biodiesel

            bioDiesel(Uniform(250, 300));

            // 80 - 100 litres of bioturbodiesel
            bioTurboDiesel(Uniform(80, 100));

            // 40 - 50 litres of biopetrol
            bioPetrol(Uniform(40, 50));

            Leave(DieselFactories, 1);
        }
    }
};

class FiberFactoryProc : public Process
{
public:
    void Behavior()
    {
        // Start the hemp factory job
        Enter(FiberFactories, 1);

        // 1 Ton of hemp fiber takes 2 - 8 hours to create
        Wait(Uniform(0.08, 0.33));     
        // 1 Ton of hemp fiber, is already bioplastics 
        bioPlastics(1);
        rawHemp -= 1;

        // End the job
        Leave(FiberFactories, 1);
    }
};

class CBDFarmProc : public Process
{
public:
    void Behavior()
    {
        // Until end of the simulation
        while(true)
        {
            // Perform growing the hemp 70 - 140 days
            Wait(Uniform(70, 140));

            // Take the unproccessed hemp and put it to the hemp
            rawHemp += 4.6;

            // Call factory to process the raw hemp to fiber
            (new FiberFactoryProc)->Activate();
        }
    }
};

class IndustrialFarmProc : public Process
{
public:
    void Behavior()
    {
        // Until end of the simulation
        while(true)
        {
            // Perform growing the hemp
            Wait(Uniform(70, 140));

            // Take the unproccessed hemp and put it to the hemp
            rawHemp += 13.5;

            // Call factory to process the raw hemp to fiber
            (new FiberFactoryProc)->Activate();
        }
    }
};

class PlasticFromCornFactoryProc : public Process
{
public:
    void Behavior()
    {
        while(cornGrain >= 1.62 && cornStover >= 0.81)
        {
            std::cout << "PLASTY\n";
            // Take corn grain from resourcess
            cornGrain -= 1.62;
            
            // Take corn stover from resourcess
            cornStover -= 0.81;

            // Make 1 ton of bioplastic
            bioPlastics(1);
        }
        
    }
};

class HarvestCompaniesProc : public Process
{
public:
    void Behavior()
    {
        while(cornFields >= 38426)
        {
            std::cout << "HARVEST\n";
            // Remove from unharvested fields
            cornFields -= 38426;

            // Harvest company starts to harvest
            Enter(HarvestCompany);

            // Harvest for 20 days
            Wait(20);

            // Add harvested corn grain
            cornGrain += Uniform(145.8, 939);

            // Add harvested corn stover
            cornStover += Uniform(469.5, 1257.8);

            // Harvest company is done
            Leave(HarvestCompany);
            (new PlasticFromCornFactoryProc)->Activate();
        }
    }
};

class CornFieldsProc : public Event
{
public:
    void Behavior()
    {
        std::cout << "KUKURICA\n";
        // Create newly grown corn
        cornFields += 192130000;

        Activate(Time + 365);
        (new HarvestCompaniesProc)->Activate();
    }
};

int main(int argc, char *argv[])
{
    char option;

    long unsigned days = 365;
    long unsigned cbdFarms = 7233;
    long unsigned hempFarms = 16878;
    long unsigned oilFactories = 15;
    long unsigned watchDays = 50;
    bool wasWatchdog = false;

    while((option = getopt(argc, argv, "d:c:f:o:w:p:")) != -1)
    {
        switch(option)
        {
            case 'd':
                days = strtoul(optarg, NULL, 10);
                break;
            case 'c':
                cbdFarms = strtoul(optarg, NULL, 10);
                break;
            case 'f':
                hempFarms = strtoul(optarg, NULL, 10);
                break;
            case 'o':
                oilFactories = strtoul(optarg, NULL, 10);
                break;
            case 'w':
                wasWatchdog = true;
                dayWatch = strtoul(optarg, NULL, 10);
                break;
            case 'p':
                landfillPlastics = strtoul(optarg, NULL, 10);
                break;
            case '?':
                break;
            default:
                break;
        }
    }
    // set simulation time to 0 to days
    Init(0, days);
    // Seed so the randomness is real
    RandomSeed(time(NULL));

    // Little watchdog to inform us about plastics state every X days
    if(wasWatchdog)
        (new WatchDog())->Activate();

    // Activate the diesel factories for X factories, to produce
    for(int i = 0; i < oilFactories; i++)
    {
        (new DieselFactoryProc)->Activate();
    }

    // Activate the hemp farms for hemp products and bioplastics
    for(int i = 0; i < cbdFarms; i++)
    {
        (new CBDFarmProc)->Activate();
    }

    // Activate the Industrial hemp farms for bioplastics only
    for(int i = 0; i < hempFarms; i++)
    {
        (new IndustrialFarmProc)->Activate();
    }

    (new CornFieldsProc)->Activate(365);

    // Run simulations
    Run();


    bioPlastics.Output();
    bioDiesel.Output();
    bioPetrol.Output();
    bioTurboDiesel.Output();
    DieselFactories.Output();
    FiberFactories.Output();

    std::cout << "Simulation ran for " << Time << " days (" << (Time / 365) << "years)\n";
    std::cout << "There were created " << bioPlastics.Sum() << " Tons of bioplastics \n";
    std::cout << "There were created " << bioDiesel.Sum() << " Litres of biodiesel \n";
    std::cout << "There were created " << bioPetrol.Sum() << " Litres of biopetrol \n";
    std::cout << "There were created " << bioTurboDiesel.Sum() << " Litres of bioturbodiesel \n";
    std::cout << "There are: " << landfillPlastics << " Tons of plastics left on lanfill" << "\n";

    return 0;
}
