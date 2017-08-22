#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "SystemState.hpp"
#include "Initialiser.hpp"
#include "Propagator.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"

#include <iostream>
#include <cstdint>
#include <string>


int main()
{
    Clock clock; // Counts time from creation to destruction
    Input input; // Read the input file, ask to create a default one when anything is wrong with it (e.g. nonexistent)

    //=====================================================================================================
    // Instantiate all objects, using parameters from the input file
    //-----------------------------------------------------------------------------------------------------
    // Get the name of the current run

    std::string runName = input.getRunName(); // copyParameter("runName", runName) would give the runName as it is in the input file, while this is the unique version

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for defining the general systemState.
    double lengthMobileMicrotubule;
    input.copyParameter("lengthMobileMicrotubule", lengthMobileMicrotubule);

    double lengthFixedMicrotubule;
    input.copyParameter("lengthFixedMicrotubule", lengthFixedMicrotubule);

    double latticeSpacing;
    input.copyParameter("latticeSpacing", latticeSpacing);

    int32_t nActiveCrosslinkers;
    input.copyParameter("numberActiveCrosslinkers", nActiveCrosslinkers);

    int32_t nDualCrosslinkers;
    input.copyParameter("numberDualCrosslinkers", nDualCrosslinkers);

    int32_t nPassiveCrosslinkers;
    input.copyParameter("numberPassiveCrosslinkers", nPassiveCrosslinkers);

    SystemState systemState(lengthMobileMicrotubule, lengthFixedMicrotubule, latticeSpacing,
                            nActiveCrosslinkers, nDualCrosslinkers, nPassiveCrosslinkers);

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

    int32_t nTimeSteps;
    input.copyParameter("numberTimeSteps", nTimeSteps);

    double calcTimeStep;
    input.copyParameter("calcTimeStep", calcTimeStep);

    int32_t probePeriod;
    input.copyParameter("probePeriod", probePeriod);

    double diffusionConstantMicrotubule;
    input.copyParameter("diffusionConstantMicrotubule", diffusionConstantMicrotubule);

    double springConstant;
    input.copyParameter("springConstant", springConstant);

    double rateZeroToOneExtremitiesConnected;
    input.copyParameter("rateZeroToOneExtremitiesConnected", rateZeroToOneExtremitiesConnected);

    double rateOneToZeroExtremitiesConnected;
    input.copyParameter("rateOneToZeroExtremitiesConnected", rateOneToZeroExtremitiesConnected);

    double rateOneToTwoExtremitiesConnected;
    input.copyParameter("rateOneToTwoExtremitiesConnected", rateOneToTwoExtremitiesConnected);

    double rateTwoToOneExtremitiesConnected;
    input.copyParameter("rateTwoToOneExtremitiesConnected", rateTwoToOneExtremitiesConnected);

    Propagator propagator(nTimeSteps,
                          calcTimeStep,
                          probePeriod,
                          diffusionConstantMicrotubule,
                          springConstant,
                          rateZeroToOneExtremitiesConnected,
                          rateOneToZeroExtremitiesConnected,
                          rateOneToTwoExtremitiesConnected,
                          rateTwoToOneExtremitiesConnected);

    //-----------------------------------------------------------------------------------------------------
    // Set the random number generator

    RandomGenerator generator(runName); // Seed with the runName, which is unique

    //-----------------------------------------------------------------------------------------------------
    // Create the output class

    Output output(runName);

    //=====================================================================================================
    // Using the objects created so far, perform the actions

    initialiser.initialise(systemState, generator);

    propagator.run(systemState, generator, output);


    return 0;
}
