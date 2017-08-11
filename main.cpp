#include <iostream>
#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "SystemState.hpp"
#include "Propagator.hpp"
#include <cstdint>
#include <string>

int main()
{
    Clock clock; // Counts time from creation to destruction
    Input input;

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


    std::string runName;
    input.copyParameter("runName", runName);

    int32_t nTimeSteps;
    input.copyParameter("numberTimeSteps", nTimeSteps);

    // Use the (unique) run name as the seed for the random number generator
    Propagator propagator(runName, nTimeSteps);

    return 0;
}
