#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "SystemState.hpp"
#include "Initialiser.hpp"
#include "Propagator.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Log.hpp"
#include "Graphics.hpp"
#include "GeneralException/GeneralException.hpp"

#include <iostream>
#include <cstdint>
#include <string>
#include <algorithm>

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
    // Get the parameters needed for defining the general systemState.
    double lengthMobileMicrotubule;
    input.copyParameter("lengthMobileMicrotubule", lengthMobileMicrotubule);
    if(lengthMobileMicrotubule<=0.0)
    {
        throw GeneralException("The parameter lengthMobileMicrotubule contains a wrong value.");
    }

    double lengthFixedMicrotubule;
    input.copyParameter("lengthFixedMicrotubule", lengthFixedMicrotubule);
    if(lengthFixedMicrotubule<=0.0)
    {
        throw GeneralException("The parameter lengthFixedMicrotubule contains a wrong value.");
    }

    double latticeSpacing;
    input.copyParameter("latticeSpacing", latticeSpacing);
    if(latticeSpacing<=0.0)
    {
        throw GeneralException("The parameter latticeSpacing contains a wrong value.");
    }

    double maximumStretchPerLatticeSpacing;
    input.copyParameter("maximumStretch", maximumStretchPerLatticeSpacing);
    if(maximumStretchPerLatticeSpacing<=0.0)
    {
        throw GeneralException("The parameter maximumStretchPerLatticeSpacing contains a wrong value.");
    }

    int32_t nActiveCrosslinkers;
    input.copyParameter("numberActiveCrosslinkers", nActiveCrosslinkers);
    if(nActiveCrosslinkers<0)
    {
        throw GeneralException("The parameter nActiveCrosslinkers contains a wrong value.");
    }

    int32_t nDualCrosslinkers;
    input.copyParameter("numberDualCrosslinkers", nDualCrosslinkers);
    if(nDualCrosslinkers<0)
    {
        throw GeneralException("The parameter nDualCrosslinkers contains a wrong value.");
    }

    int32_t nPassiveCrosslinkers;
    input.copyParameter("numberPassiveCrosslinkers", nPassiveCrosslinkers);
    if(nPassiveCrosslinkers<0)
    {
        throw GeneralException("The parameter nPassiveCrosslinkers contains a wrong value.");
    }

    double springConstant;
    input.copyParameter("springConstant", springConstant);
    if(springConstant<0.0)
    {
        throw GeneralException("The parameter springConstant contains a wrong value.");
    }

    std::string addTheoreticalCounterForceString;
    input.copyParameter("addTheoreticalCounterForce", addTheoreticalCounterForceString);
    const bool addTheoreticalCounterForce = (addTheoreticalCounterForceString == "TRUE"); // Whenever it is not TRUE, assume it is false

    SystemState systemState(lengthMobileMicrotubule, lengthFixedMicrotubule, latticeSpacing, maximumStretchPerLatticeSpacing,
                            nActiveCrosslinkers, nDualCrosslinkers, nPassiveCrosslinkers, springConstant, addTheoreticalCounterForce);

    //-----------------------------------------------------------------------------------------------------
    // Create the output class. Needs to be done before the propagator, since this needs samplePositionalDistribution as well

    std::string samplePositionalDistributionString;
    input.copyParameter("samplePositionalDistribution", samplePositionalDistributionString);
    const bool samplePositionalDistribution = (samplePositionalDistributionString == "TRUE"); // Whenever it is not TRUE, assume it is false

    double positionalHistogramBinSize;
    input.copyParameter("positionalHistogramBinSize", positionalHistogramBinSize);
    if(samplePositionalDistribution&&positionalHistogramBinSize<=0.0)
    {
        throw GeneralException("The parameter positionalHistogramBinSize contains a wrong value.");
    }

    double positionalHistogramLowestValue;
    input.copyParameter("positionalHistogramLowestValue", positionalHistogramLowestValue);

    double positionalHistogramHighestValue;
    input.copyParameter("positionalHistogramHighestValue", positionalHistogramHighestValue);

    std::string recordTransitionPathsString;
    input.copyParameter("recordTransitionPaths", recordTransitionPathsString);
    const bool recordTransitionPaths = (recordTransitionPathsString == "TRUE");

    std::string bindingDynamicsString;
    input.copyParameter("bindingDynamics", bindingDynamicsString);
    const bool bindingDynamics = (bindingDynamicsString == "TRUE");

    int32_t maxNFullCrosslinkers;
    if(bindingDynamics)
    {
        maxNFullCrosslinkers = std::min(systemState.getNFreeSitesFixed(), systemState.getNFreeSitesMobile());
    }
    else
    {
        maxNFullCrosslinkers = nActiveCrosslinkers+nDualCrosslinkers+nPassiveCrosslinkers;
    }

    #ifdef MYDEBUG
    std::cout << "The maximum number of full linkers is given by " << maxNFullCrosslinkers << ".\n";
    #endif // MYDEBUG

    Output output(runName,
                  samplePositionalDistribution,
                  recordTransitionPaths,
                  positionalHistogramBinSize,
                  positionalHistogramLowestValue,
                  positionalHistogramHighestValue,
                  maxNFullCrosslinkers);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for initialising the state.

    double initialPositionMicrotubule;
    input.copyParameter("initialPositionMicrotubule", initialPositionMicrotubule);

    double fractionOverlapSitesConnected;
    input.copyParameter("fractionOverlapSitesConnected", fractionOverlapSitesConnected);
    if(fractionOverlapSitesConnected<0.0 || fractionOverlapSitesConnected > 1.0)
    {
        throw GeneralException("The parameter fractionOverlapSitesConnected contains a wrong value.");
    }

    std::string initialCrosslinkerDistributionString;
    input.copyParameter("initialCrosslinkerDistribution", initialCrosslinkerDistributionString);


    Initialiser initialiser(initialPositionMicrotubule, fractionOverlapSitesConnected, initialCrosslinkerDistributionString);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for setting the propagator

    int32_t numberEquilibrationBlocks;
    input.copyParameter("numberEquilibrationBlocks", numberEquilibrationBlocks);
    if(numberEquilibrationBlocks<0)
    {
        throw GeneralException("The parameter numberEquilibrationBlocks contains a wrong value.");
    }

    int32_t numberRunBlocks;
    input.copyParameter("numberRunBlocks", numberRunBlocks);
    if(numberRunBlocks<0)
    {
        throw GeneralException("The parameter numberRunBlocks contains a wrong value.");
    }

    int32_t nTimeSteps;
    input.copyParameter("timeStepsPerBlock", nTimeSteps);
    if(nTimeSteps<=0)
    {
        throw GeneralException("The parameter nTimeSteps contains a wrong value.");
    }

    double calcTimeStep;
    input.copyParameter("calcTimeStep", calcTimeStep);
    if(calcTimeStep<=0.0)
    {
        throw GeneralException("The parameter calcTimeStep contains a wrong value.");
    }

    int32_t positionProbePeriod;
    input.copyParameter("positionProbePeriod", positionProbePeriod);
    if(positionProbePeriod<=0)
    {
        throw GeneralException("The parameter positionProbePeriod contains a wrong value.");
    }

    int32_t transitionPathProbePeriod;
    input.copyParameter("transitionPathProbePeriod", transitionPathProbePeriod);
    if(transitionPathProbePeriod<=0)
    {
        throw GeneralException("The parameter transitionPathProbePeriod contains a wrong value.");
    }

    double diffusionConstantMicrotubule;
    input.copyParameter("diffusionConstantMicrotubule", diffusionConstantMicrotubule);
    if(diffusionConstantMicrotubule<0.0)
    {
        throw GeneralException("The parameter diffusionConstantMicrotubule contains a wrong value.");
    }

    double ratePassivePartialHop;
    input.copyParameter("ratePassivePartialHop", ratePassivePartialHop);
    if(ratePassivePartialHop<0.0)
    {
        throw GeneralException("The parameter ratePassivePartialHop contains a wrong value.");
    }

    double ratePassiveFullHop;
    input.copyParameter("ratePassiveFullHop", ratePassiveFullHop);
    if(ratePassiveFullHop<0.0)
    {
        throw GeneralException("The parameter ratePassiveFullHop contains a wrong value.");
    }

    double baseRateActivePartialHop;
    input.copyParameter("baseRateActivePartialHop", baseRateActivePartialHop);
    if(baseRateActivePartialHop<0.0)
    {
        throw GeneralException("The parameter baseRateActivePartialHop contains a wrong value.");
    }

    double baseRateActiveFullHop;
    input.copyParameter("baseRateActiveFullHop", baseRateActiveFullHop);
    if(baseRateActiveFullHop<0.0)
    {
        throw GeneralException("The parameter baseRateActiveFullHop contains a wrong value.");
    }

    double activeHopToPlusBiasEnergy;
    input.copyParameter("activeHopToPlusBiasEnergy", activeHopToPlusBiasEnergy);

    double baseRateZeroToOneExtremitiesConnected;
    input.copyParameter("baseRateZeroToOneExtremitiesConnected", baseRateZeroToOneExtremitiesConnected);
    if(baseRateZeroToOneExtremitiesConnected<0.0)
    {
        throw GeneralException("The parameter baseRateZeroToOneExtremitiesConnected contains a wrong value.");
    }

    double baseRateOneToZeroExtremitiesConnected;
    input.copyParameter("baseRateOneToZeroExtremitiesConnected", baseRateOneToZeroExtremitiesConnected);
    if(baseRateOneToZeroExtremitiesConnected<0.0)
    {
        throw GeneralException("The parameter baseRateOneToZeroExtremitiesConnected contains a wrong value.");
    }

    double baseRateOneToTwoExtremitiesConnected;
    input.copyParameter("baseRateOneToTwoExtremitiesConnected", baseRateOneToTwoExtremitiesConnected);
    if(baseRateOneToTwoExtremitiesConnected<0.0)
    {
        throw GeneralException("The parameter baseRateOneToTwoExtremitiesConnected contains a wrong value.");
    }

    double baseRateTwoToOneExtremitiesConnected;
    input.copyParameter("baseRateTwoToOneExtremitiesConnected", baseRateTwoToOneExtremitiesConnected);
    if(baseRateTwoToOneExtremitiesConnected<0.0)
    {
        throw GeneralException("The parameter baseRateTwoToOneExtremitiesConnected contains a wrong value.");
    }

    // Force the (un)binding rates to zero when binding dynamics is turned off, because then the maximum number of full linkers can be properly set before
    if(!bindingDynamics)
    {
        baseRateZeroToOneExtremitiesConnected = 0.0;
        baseRateOneToZeroExtremitiesConnected = 0.0;
        baseRateOneToTwoExtremitiesConnected = 0.0;
        baseRateTwoToOneExtremitiesConnected = 0.0;
    }

    double headBindingBiasEnergy;
    input.copyParameter("headBindingBiasEnergy", headBindingBiasEnergy);

    Propagator propagator(numberEquilibrationBlocks,
                          numberRunBlocks,
                          nTimeSteps,
                          calcTimeStep,
                          positionProbePeriod,
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
                          samplePositionalDistribution,
                          recordTransitionPaths,
                          transitionPathProbePeriod,
                          addTheoreticalCounterForce,
                          log);

    //-----------------------------------------------------------------------------------------------------
    // Get the parameters needed for setting the Graphics

    std::string showGraphicsString;
    input.copyParameter("showGraphics", showGraphicsString);
    const bool showGraphics = (showGraphicsString == "TRUE");

    int32_t timeStepsDisplayInterval;
    input.copyParameter("timeStepsDisplayInterval", timeStepsDisplayInterval);
    if(showGraphics && timeStepsDisplayInterval<=0)
    {
        throw GeneralException("The parameter timeStepsDisplayInterval contains a wrong value.");
    }

    int32_t updateDelayInMilliseconds;
    input.copyParameter("updateDelayInMilliseconds", updateDelayInMilliseconds);
    if(showGraphics && updateDelayInMilliseconds<0)
    {
        throw GeneralException("The parameter updateDelayInMilliseconds contains a wrong value.");
    }

    //=====================================================================================================
    // Using the objects created so far, perform the actions

    initialiser.initialise(systemState, generator); // initialise the system state

    propagator.equilibrate(systemState, generator, output); // run the dynamics for a certain time such that an equilibrium distribution can be reached (not guaranteed to be done)

    if(showGraphics)
    {
        Graphics graphics(runName, systemState, propagator, generator, output, timeStepsDisplayInterval, updateDelayInMilliseconds);

        graphics.performMainLoop();
    }
    else
    {
        propagator.run(systemState, generator, output);
    }

    return 0;
}
