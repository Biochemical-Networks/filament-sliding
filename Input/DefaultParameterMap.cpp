#include "ParameterMap.hpp"

#include "GenericValue.hpp"
#include <string>


ParameterMap::ParameterMap()
{
    /* Define all input parameters here.
     * defineParameter takes the name (string), value (T), unit (string) (optional), allowed values (string) (optional).
     * If no unit is given, "unitless" is added. If no allowed values are given, "all" is added.
     * If allowed values are added, it is necessary to give the unit. In the case that there is no unit, but there are allowed values, add "unitless" manually
     * Empty strings are not allowed for any of the fields.
     * Use template argument deduction to call defineParameter. The type does not need to be given.
     * If a parameter is to be of type double, always add a floating point (e.g. "4.") to make it double
     * The strings (for example the unit or the allowed parameters!) are NOT allowed to contain white space, since reading will then try to read the next field
     */
    // Run parameters
    defineParameter("runName", "run", "unitless"); // The code relies on this parameter being called "runName"
    defineParameter("numberOfRuns", 1, "unitless", ">0");
    defineParameter("numberEquilibrationBlocks", 0, "blocks", ">=0");
    defineParameter("numberRunBlocks", 1, "blocks", ">=0");
    defineParameter("calcTimeStep", 1.e-10, "s", ">0");
    defineParameter("timeStepsPerBlock", 1000000, "steps", ">0");
    defineParameter("positionProbePeriod", 1000000, "steps", ">0");

    // General state parameters
    defineParameter("lengthMobileMicrotubule", 1., "micron", ">0");
    defineParameter("lengthFixedMicrotubule", 1., "micron", ">0");
    defineParameter("latticeSpacing", 8.e-3, "micron", ">0");
    defineParameter("maximumStretch", 1.499999999999999, "latticeSpacing", ">0");

    defineParameter("numberActiveCrosslinkers", 0, "crosslinkers", ">=0");
    defineParameter("numberDualCrosslinkers", 0, "crosslinkers", ">=0");
    defineParameter("numberPassiveCrosslinkers", 1000, "crosslinkers", ">=0");

    // Initial state parameters
    defineParameter("initialPositionMicrotubule", 0., "micron"); // The position of the mobile microtubule relative to the fixed one, where 0. means that the two have the same (left) boundary position
    /*defineParameter("fractionOverlapSitesConnected", 1.0, "unitless", "[0,1]");*/
    /*defineParameter("initialCrosslinkerDistribution", "RANDOM", "unitless", "RANDOM,HEADSMOBILE,TAILSMOBILE,TEST"); // Can only hold the values set by Initialiser::InitialCrosslinkerDistribution*/

    // Dynamics parameters
    defineParameter("diffusionConstantMicrotubule", 0.01, "micron^(2)*s^(-1)", ">=0"); // also sets the microtubule mobility via the Einstein relation (units micron^2*(kT)^(-1)*s^(-1))
    defineParameter("actinDisconnectTime", 0.01, "s", ">=0");
    defineParameter("springConstant", 1.1e5, "kT*micron^(-2)", ">=0");

    // Rates for connecting crosslinkers to the microtubules. The rates from zero to one are per crosslinker in solution per microtubule site.
    // So the full rate for connecting to any site is baseRateZeroToOneExtremitiesConnected * #free crosslinkers * # free microtubule sites
    // For unbinding a full linker, the rate is given per linker, not per extremity.
    // So the rate baseRateTwoToOneExtremitiesConnected = unbindHeadOfFull+unbindTailOfFull. (1)
    // Similarly, baseRateZeroToOneExtremitiesConnected = bindHeadOfFree+bindTailOfFree. (2)
    // The other rates coming from a partially connected linker are also per linker, and then automatically per extremity. Hence,
    // baseRateOneToZeroExtremitiesConnected = 0.5*(unbindHeadOfPartial + unbindTailOfPartial) (3)
    // baseRateOneToTwoExtremitiesConnected = 0.5*(bindHeadOfPartial + bindTailOfPartial) (4)
    // Then, four energy equations exist:
    // bindHeadOfFree/unbindHeadOfPartial = baseRateZeroToOneExtremitiesConnected/(2*baseRateOneToZeroExtremitiesConnected)*exp(headBindingBiasEnergy) (5)
    // bindTailOfFree/unbindTailOfPartial = baseRateZeroToOneExtremitiesConnected/(2*baseRateOneToZeroExtremitiesConnected)*exp(-headBindingBiasEnergy) (6)
    // bindHeadOfPartial/unbindHeadOfPartial = 2*baseRateOneToTwoExtremitiesConnected/baseRateTwoToOneExtremitiesConnected *exp(headBindingBiasEnergy) (7)
    // bindTailOfPartial/unbindTailOfPartial = 2*baseRateOneToTwoExtremitiesConnected/baseRateTwoToOneExtremitiesConnected *exp(-headBindingBiasEnergy) (8)
    // There are eight equations for the eight (un)bind... rates, so a unique solution is found.
    // Detailed balance is obeyed because forward and backward rates are set such that their ratio always depends on the free energy difference between the two states.
    // Hence, the free energy difference between the fully connected and free crosslinker does not depend on the path (first head or first tail),
    // and there is no net flux through the cycle.
    // Normally this sets only one of the eight degrees of freedom (the rates), but we only have five degrees of freedom to set.
    // This is because we assume that the transitions to and from a HEAD state are similar to the transitions to and from a TAIL state.
    // Specifically, we let the base rates equal each other, and only make them differ through a bias energy; there is no difference in the energy barriers.
    // For more information, including solutions to the equations, see the document "Crosslink_Mapping_Binding_Rates_Bias_Energy.pdf"
    defineParameter("bindingDynamics", "TRUE", "unitless", "TRUE,FALSE");
    defineParameter("baseRateZeroToOneExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("baseRateOneToZeroExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("baseRateOneToTwoExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("baseRateTwoToOneExtremitiesConnected", 1.0, "s^(-1)", ">=0");

    defineParameter("bindingDynamicsOnBlocked", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("baseRateZeroToOneOnBlocked", 0.0, "s^(-1)", ">=0");
    defineParameter("baseRateOneToZeroOnBlocked", 0.0, "s^(-1)", ">=0");

    /*defineParameter("headBindingBiasEnergy", 0.0, "kT", "real");*/

    // Parameters for the dynamics of the fixed microtubule.
    // Dynamics is either stochastic (random site addition at front, exponential blocking), or deterministic (fixed tip area, step function)
    defineParameter("rateFixedMicrotubuleGrowth", 0.0, "s^(-1)", ">=0");
    defineParameter("microtubuleDynamics", "STOCHASTIC", "unitless", "STOCHASTIC,DETERMINISTIC");
    // Parameters for stochastic system
    defineParameter("rateBlockBoundSites", 0.0, "s^(-1)", ">=0");
    defineParameter("rateBlockUnboundSites", 0.0, "s^(-1)", ">=0");
    // Parameters for deterministic system
    defineParameter("tipSize", 0.0, "micron", ">=0,<=lengthFixedMicrotubule");

    // Parameters to turn analysis on or off
    defineParameter("addExternalForce", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("externalForceType", "BARRIERFREE", "unitless", "BARRIERFREE,QUADRATIC");
    defineParameter("samplePositionalDistribution", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("positionalHistogramBinSize", 8.e-7, "micron", ">0");
    defineParameter("positionalHistogramLowestValue", 0.0, "micron", ">=0");
    defineParameter("positionalHistogramHighestValue", 8.e-3, "micron", ">positionalHistogramLowestValue");
    defineParameter("showGraphics", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("timeStepsDisplayInterval", 100000, "steps", ">0");
    defineParameter("updateDelayInMilliseconds", 100, "milliseconds", ">=0");
    /*defineParameter("recordTransitionPaths", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("transitionPathProbePeriod", 10000, "steps", ">0"); // The probe period is used for writing, not for recording in the histogram
    defineParameter("maxNumberTransitionPaths", 100, "paths", ">=0");*/
    defineParameter("maxPeriodPositionTracking", 10.0, "s", ">=0");
    /*defineParameter("estimateTimeEvolutionAtPeak", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("timeStepsPerDistributionEstimate", 25, "steps", ">0");
    defineParameter("nEstimatesDistribution", 200, "sets", ">0");
    defineParameter("dynamicsEstimationInitialRegionWidth", 0.0002, "unitless", "(0,1]");
    defineParameter("dynamicsEstimationFinalRegionWidth", 0.05, "unitless", "(0,1]");*/
}
