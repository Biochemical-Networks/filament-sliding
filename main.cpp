#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "SystemState.hpp"
#include "Propagator.hpp"
#include "RandomGenerator.hpp"

#include <iostream>
#include <cstdint>
#include <string>


int main()
{
    Clock clock; // Counts time from creation to destruction
    Input input; // Read the input file, ask to create a default one when anything is wrong with it (e.g. nonexistent)

    /*************************************************************************************************/
    // Get the name of the current run

    std::string runName;
    input.copyParameter("runName", runName);

    /*************************************************************************************************/
    // Get the parameters needed for initialising the systemState.
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

    /*************************************************************************************************/
    // Get the parameters needed for initialising the propagator

    int32_t nTimeSteps;
    input.copyParameter("numberTimeSteps", nTimeSteps);

    double calcTimeStep;
    input.copyParameter("calcTimeStep", calcTimeStep);

    double diffusionConstantMicrotubule;
    input.copyParameter("diffusionConstantMicrotubule", diffusionConstantMicrotubule);

    double springConstant;
    input.copyParameter("springConstant", springConstant);

    Propagator propagator(nTimeSteps, calcTimeStep, diffusionConstantMicrotubule, springConstant);

    /*************************************************************************************************/
    // Set the random number generator

    RandomGenerator generator(runName); // Seed with the runName, which is unique


    return 0;
}
