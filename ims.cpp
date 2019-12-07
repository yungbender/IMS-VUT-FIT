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

Store FiberFactories(100);
Store HarvestCompany(50);
Store CornBioplasticFactory(50);

Stat bioPlastics("All bioplastics created");
Stat bioDiesel("Biodiesel created");
Stat bioPetrol("Biopetrol created");
Stat bioTurboDiesel("Bioturbodiesel created");
Stat bioPlasticsCorn("Bioplastics created by corn");
Stat bioPlasticsHemp("Bioplastics created by hemp");
Stat burntPlastics("Plastics that were burnt");


class WatchDog : public Event
{
public:
    void Behavior()
    {
        std::cout << "Simulation ran for " << Time << " days (" << (Time / 365) << "years)\n";
        std::cout << "Currently there is " << bioPlastics.Sum() << " Tons of bioplastics \n";
        std::cout << "Currently there is " << bioPlasticsCorn.Sum() << " Tons of bioplastics made from corn \n";
        std::cout << "Currently there is " << bioPlasticsHemp.Sum() << " Tons of bioplastics made from hemp \n";
        std::cout << "There were burnt " << burntPlastics.Sum() << " Tons of bioplastics\n";
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
                burntPlastics(landfillPlastics);
                landfillPlastics = 0;
            }
            else
            {
                // Roll between 13 - 15 Tons of plastics, that it take to create oils
                double burnt = Uniform(13, 15);
                burntPlastics(burnt);
                landfillPlastics -= burnt;
            }
            
            // It takes over month to create oil
            Wait(Uniform(28, 31));

            // 250 - 300 litres of biodiesel
            bioDiesel(Uniform(250, 300));

            // 80 - 100 litres of bioturbodiesel
            bioTurboDiesel(100);

            // 40 - 50 litres of biopetrol
            bioPetrol(50);
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

        if(rawHemp == 0)
        {
            return;
        }
        // If there is some remaining hemp take it
        else if(rawHemp < 20) 
        {
            bioPlastics(rawHemp);
            bioPlastics(rawHemp);
            rawHemp = 0;
        }
        // 1 Ton of hemp fiber takes 2 - 8 hours to create
        // Else make 20 tons 
        // (Yes, documentation talks about 1 ton per 2 - 8 hours, but we needed to multiply the count, otherwise the calculation
        //  would take too long)
        else
        {
            bioPlasticsHemp(20);
            bioPlastics(20);
            rawHemp -= 20;
        }
        Wait(Uniform(1.6, 6.6));
        
        // End the job
        Leave(FiberFactories, 1);
    }
};

class CBDFarmProc : public Event
{
public:
    void Behavior()
    {
        // Take the unproccessed hemp and put it to the hemp
        rawHemp += 0.54;

        // Perform next growing
        Activate(Time + Uniform(70, 140));

        // Call factory for each ton to process the raw hemp to fiber
        (new FiberFactoryProc)->Activate();
    }
};

class IndustrialFarmProc : public Event
{
public:
    void Behavior()
    {
        // Take the unproccessed hemp and put it to the hemp
        rawHemp += 135;

        // Perform next growing
        Activate(Time + Uniform(70, 140));

        // Call factory to process the raw hemp to fiber
        double createdHemp = 135;
        while(createdHemp > 20)
        {
            (new FiberFactoryProc)->Activate();
            createdHemp -= 20;
        }
        (new FiberFactoryProc)->Activate();
    }
};

class PlasticFromCornFactoryProc : public Process
{
public:
    void Behavior()
    {
        Enter(CornBioplasticFactory, 1);

        // Corn bioplastic production time
        Wait(3.13);

        // Take corn grain from resourcess
        cornGrain -= 162;
        
        // Take corn stover from resourcess
        cornStover -= 81;

        // Make 1 ton of bioplastic
        bioPlastics(100);
        bioPlasticsCorn(100);

        Leave(CornBioplasticFactory, 1);
    }
};

class HarvestCompaniesProc : public Process
{
public:
    void Behavior()
    {
        // Harvest company starts to harvest
        Enter(HarvestCompany, 1);

        // Remove from unharvested fields
        cornFields -= 38426;

        // Harvest for 20 days
        Wait(20);

        // Add harvested corn grain
        cornGrain += Uniform(145.8, 939);

        // Add harvested corn stover
        cornStover += Uniform(469.5, 1257.8);

        // Harvest company is done
        Leave(HarvestCompany, 1);
        double cornGrainTemp = cornGrain;
        double cornStoverTemp = cornStover;
        while(cornGrainTemp >= 162 && cornStoverTemp >= 81)
        {
            cornGrainTemp -= 162;
            cornStoverTemp -= 81;
            (new PlasticFromCornFactoryProc)->Activate();
        }
    }
};

class CornFieldsProc : public Event
{
public:
    void Behavior()
    {
        // Create newly grown corn
        cornFields += 192130000;
        double cornFieldsTemp = 192130000;

        // Create new corna after one year
        Activate(Time + 365);

        // Let harvest comapanies know there is corn to harvest
        while(cornFieldsTemp >= 38426)
        {
            cornFieldsTemp -= 38426; 
            (new HarvestCompaniesProc)->Activate();
        }
    }
};

int main(int argc, char *argv[])
{
    std::cout << std::fixed;
    char option;

    // default values
    long unsigned days = 365;
    long unsigned cbdFarms = 723;
    long unsigned hempFarms = 1687;
    long unsigned oilFactories = 100;
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
        (new CBDFarmProc)->Activate(Uniform(70, 140));
    }

    // Activate the Industrial hemp farms for bioplastics only
    for(int i = 0; i < hempFarms; i++)
    {
        (new IndustrialFarmProc)->Activate(Uniform(70, 140));
    }

    // Simulation of 1 year corn growing
    (new CornFieldsProc)->Activate(365);

    // Run simulations
    Run();

    bioPlastics.Output();
    burntPlastics.Output();
    bioDiesel.Output();
    bioPetrol.Output();
    bioTurboDiesel.Output();
    FiberFactories.Output();

    std::cout << "Simulation ran for " << Time << " days (" << (Time / 365) << "years)\n";
    std::cout << "There were created " << bioPlastics.Sum() << " Tons of bioplastics \n";
    std::cout << "There were burnt " << burntPlastics.Sum() << " Tons of bioplastics\n";
    std::cout << "Created bioplastics with corn " << bioPlasticsCorn.Sum() << " Tons of bioplastics \n";
    std::cout << "Created bioplastics with hemp " << bioPlasticsHemp.Sum() << " Tons of bioplastics \n";
    std::cout << "There were created " << bioDiesel.Sum() << " Litres of biodiesel \n";
    std::cout << "There were created " << bioPetrol.Sum() << " Litres of biopetrol \n";
    std::cout << "There were created " << bioTurboDiesel.Sum() << " Litres of bioturbodiesel \n";
    std::cout << "There are: " << landfillPlastics << " Tons of plastics left on landfill" << "\n";

    return 0;
}
