#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "SystemState.hpp"
#include "Initialiser.hpp"
#include "Propagator.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Log.hpp"
#include "Graphics.hpp"

#include <iostream>
#include <cstdint>
#include <string>


#include "Crosslinker.hpp"

int main()
{
    #ifdef MYDEBUG
    std::cout<< "You are running the MYDEBUG version of the program.\n";
    #endif // MYDEBUG
    Clock clock; // Counts time from creation to destruction
    Input input; // Read the input file, ask to create a default one when anything is wrong with it (e.g. nonexistent)

    //=====================================================================================================
    // Instantiate all objects, using parameters from the input file
    //-----------------------------------------------------------------------------------------------------
    // Get the name of the current run

    std::string runName = input.getRunName(); // copyParameter("runName", runName) would give the runName as it is in the input file, while this is the unique version

    Log log(runName, clock);

    //-----------------------------------------------------------------------------------------------------
    // Set the random number generator

    RandomGenerator generator(runName); // Seed with the runName, which is unique

    //-----------------------------------------------------------------------------------------------------
    // Create the output class

    std::string samplePositionalDistributionString;
    input.copyParameter("samplePositionalDistribution", samplePositionalDistributionString);
    const bool samplePositionalDistribution = (samplePositionalDistributionString == "TRUE"); // Whenever it is not TRUE, assume it is false

    double positionalHistogramBinSize;
    input.copyParameter("positionalHistogramBinSize", positionalHistogramBinSize);

    double positionalHistogramLowestValue;
    input.copyParameter("positionalHistogramLowestValue", positionalHistogramLowestValue);

    double positionalHistogramHighestValue;
    input.copyParameter("positionalHistogramHighestValue", positionalHistogramHighestValue);

    Output output(runName, samplePositionalDistribution, positionalHistogramBinSize, positionalHistogramLowestValue, positionalHistogramHighestValue);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for defining the general systemState.
    double lengthMobileMicrotubule;
    input.copyParameter("lengthMobileMicrotubule", lengthMobileMicrotubule);

    double lengthFixedMicrotubule;
    input.copyParameter("lengthFixedMicrotubule", lengthFixedMicrotubule);

    double latticeSpacing;
    input.copyParameter("latticeSpacing", latticeSpacing);

    double maximumStretchPerLatticeSpacing;
    input.copyParameter("maximumStretch", maximumStretchPerLatticeSpacing);

    int32_t nActiveCrosslinkers;
    input.copyParameter("numberActiveCrosslinkers", nActiveCrosslinkers);

    int32_t nDualCrosslinkers;
    input.copyParameter("numberDualCrosslinkers", nDualCrosslinkers);

    int32_t nPassiveCrosslinkers;
    input.copyParameter("numberPassiveCrosslinkers", nPassiveCrosslinkers);

    double springConstant;
    input.copyParameter("springConstant", springConstant);

    std::string addTheoreticalCounterForceString;
    input.copyParameter("addTheoreticalCounterForce", addTheoreticalCounterForceString);
    const bool addTheoreticalCounterForce = (addTheoreticalCounterForceString == "TRUE"); // Whenever it is not TRUE, assume it is false

    SystemState systemState(lengthMobileMicrotubule, lengthFixedMicrotubule, latticeSpacing, maximumStretchPerLatticeSpacing,
                            nActiveCrosslinkers, nDualCrosslinkers, nPassiveCrosslinkers, springConstant, addTheoreticalCounterForce);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for initialising the state.

    double initialPositionMicrotubule;
    input.copyParameter("initialPositionMicrotubule", initialPositionMicrotubule);

    double fractionOverlapSitesConnected;
    input.copyParameter("fractionOverlapSitesConnected", fractionOverlapSitesConnected);

    std::string initialCrosslinkerDistributionString;
    input.copyParameter("initialCrosslinkerDistribution", initialCrosslinkerDistributionString);



    Initialiser initialiser(initialPositionMicrotubule, fractionOverlapSitesConnected, initialCrosslinkerDistributionString);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for setting the propagator

    int32_t numberEquilibrationBlocks;
    input.copyParameter("numberEquilibrationBlocks", numberEquilibrationBlocks);

    int32_t numberRunBlocks;
    input.copyParameter("numberRunBlocks", numberRunBlocks);

    int32_t nTimeSteps;
    input.copyParameter("timeStepsPerBlock", nTimeSteps);

    double calcTimeStep;
    input.copyParameter("calcTimeStep", calcTimeStep);

    int32_t probePeriod;
    input.copyParameter("probePeriod", probePeriod);

    double diffusionConstantMicrotubule;
    input.copyParameter("diffusionConstantMicrotubule", diffusionConstantMicrotubule);

    double ratePassivePartialHop;
    input.copyParameter("ratePassivePartialHop", ratePassivePartialHop);

    double ratePassiveFullHop;
    input.copyParameter("ratePassiveFullHop", ratePassiveFullHop);

    double baseRateActivePartialHop;
    input.copyParameter("baseRateActivePartialHop", baseRateActivePartialHop);

    double baseRateActiveFullHop;
    input.copyParameter("baseRateActiveFullHop", baseRateActiveFullHop);

    double activeHopToPlusBiasEnergy;
    input.copyParameter("activeHopToPlusBiasEnergy", activeHopToPlusBiasEnergy);

    double baseRateZeroToOneExtremitiesConnected;
    input.copyParameter("baseRateZeroToOneExtremitiesConnected", baseRateZeroToOneExtremitiesConnected);

    double baseRateOneToZeroExtremitiesConnected;
    input.copyParameter("baseRateOneToZeroExtremitiesConnected", baseRateOneToZeroExtremitiesConnected);

    double baseRateOneToTwoExtremitiesConnected;
    input.copyParameter("baseRateOneToTwoExtremitiesConnected", baseRateOneToTwoExtremitiesConnected);

    double baseRateTwoToOneExtremitiesConnected;
    input.copyParameter("baseRateTwoToOneExtremitiesConnected", baseRateTwoToOneExtremitiesConnected);

    double headBindingBiasEnergy;
    input.copyParameter("headBindingBiasEnergy", headBindingBiasEnergy);

    Propagator propagator(numberEquilibrationBlocks,
                          numberRunBlocks,
                          nTimeSteps,
                          calcTimeStep,
                          probePeriod,
                          diffusionConstantMicrotubule,
                          springConstant,
                          latticeSpacing,
                          ratePassivePartialHop,
                          ratePassiveFullHop,
                          baseRateActivePartialHop,
                          baseRateActiveFullHop,
                          activeHopToPlusBiasEnergy,
                          baseRateZeroToOneExtremitiesConnected,
                          baseRateOneToZeroExtremitiesConnected,
                          baseRateOneToTwoExtremitiesConnected,
                          baseRateTwoToOneExtremitiesConnected,
                          headBindingBiasEnergy,
                          generator,
                          samplePositionalDistribution);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for setting the Graphics

    std::string showGraphicsString;
    input.copyParameter("showGraphics", showGraphicsString);
    const bool showGraphics = (showGraphicsString == "TRUE");

    int32_t timeStepsDisplayInterval;
    input.copyParameter("timeStepsDisplayInterval", timeStepsDisplayInterval);

    //=====================================================================================================
    // Using the objects created so far, perform the actions

    initialiser.initialise(systemState, generator); // initialise the system state

    propagator.equilibrate(systemState, generator, output); // run the dynamics for a certain time such that an equilibrium distribution can be reached (not guaranteed to be done)

    if(showGraphics)
    {
        Graphics graphics(runName, systemState, propagator, timeStepsDisplayInterval);

        graphics.performMainLoop(generator, output);
    }
    else
    {
        propagator.run(systemState, generator, output);
    }

    return 0;
}
