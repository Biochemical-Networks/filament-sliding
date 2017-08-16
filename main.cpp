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

    Initialiser initialiser(initialPositionMicrotubule);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for setting the propagator

    int32_t nTimeSteps;
    input.copyParameter("numberTimeSteps", nTimeSteps);

    double calcTimeStep;
    input.copyParameter("calcTimeStep", calcTimeStep);

    double diffusionConstantMicrotubule;
    input.copyParameter("diffusionConstantMicrotubule", diffusionConstantMicrotubule);

    double springConstant;
    input.copyParameter("springConstant", springConstant);

    Propagator propagator(nTimeSteps, calcTimeStep, diffusionConstantMicrotubule, springConstant);

    //-----------------------------------------------------------------------------------------------------
    // Set the random number generator

    RandomGenerator generator(runName); // Seed with the runName, which is unique

    //-----------------------------------------------------------------------------------------------------
    // Create the output class

    int32_t probePeriod;
    input.copyParameter("probePeriod", probePeriod);

    Output output(runName, probePeriod);

    //=====================================================================================================
    // Using the objects created so far, perform the actions

    initialiser.initialise(systemState, generator);

    propagator.run(systemState, generator, output);


    return 0;
}
