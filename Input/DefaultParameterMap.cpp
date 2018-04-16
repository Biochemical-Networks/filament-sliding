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
    defineParameter("numberEquilibrationBlocks", 0, "blocks", ">=0");
    defineParameter("numberRunBlocks", 1, "blocks", ">=0");
    defineParameter("calcTimeStep", 1.e-7, "s", ">0");
    defineParameter("timeStepsPerBlock", 20000, "steps", ">=0");
    defineParameter("probePeriod", 10000, "steps", ">0");

    // General state parameters
    defineParameter("lengthMobileMicrotubule", 1., "micron", ">0");
    defineParameter("lengthFixedMicrotubule", 1., "micron", ">0");
    defineParameter("latticeSpacing", 8.e-3, "micron", ">0");
    defineParameter("maximumStretch", 1.499999999999999, "latticeSpacing", ">0");

    defineParameter("numberActiveCrosslinkers", 1000, "crosslinkers", ">=0");
    defineParameter("numberDualCrosslinkers", 1000, "crosslinkers", ">=0");
    defineParameter("numberPassiveCrosslinkers", 1000, "crosslinkers", ">=0");

    // Initial state parameters
    defineParameter("initialPositionMicrotubule", 0., "micron"); // The position of the mobile microtubule relative to the fixed one, where 0. means that the two have the same (left) boundary position
    defineParameter("fractionOverlapSitesConnected", 1.0, "unitless", ">0");
    defineParameter("initialCrosslinkerDistribution", "RANDOM", "unitless", "RANDOM,HEADSMOBILE,TAILSMOBILE,TEST"); // Can only hold the values set by Initialiser::InitialCrosslinkerDistribution

    // Dynamics parameters
    defineParameter("diffusionConstantMicrotubule", 0.01, "micron^(2)*s^(-1)", ">=0"); // also sets the microtubule mobility via the Einstein relation (units micron^2*(kT)^(-1)*s^(-1))
    defineParameter("springConstant", 1.1e5, "kT*micron^(-2)", ">=0");


    // The rates for hopping of passive extremities when the linker is either partially or fully connected
    defineParameter("ratePassivePartialHop", 1562.5, "s^(-1)", ">=0");
    defineParameter("ratePassiveFullHop", 1562.5, "s^(-1)", ">=0");
    // The rates for hopping of active extremities when the linker is either partially or fully connected
    // are calculated using the following parameters.
    // baseRate is the geometric mean between the forward and backward rate,
    // whereas activeHopToPlusBiasEnergy gives the free energy driving a single hop towards the plus tip of the microtubule.
    // For a minus end walking motor, use a negative bias free energy
    defineParameter("baseRateActivePartialHop", 1562.5, "s^(-1)", ">=0");
    defineParameter("baseRateActiveFullHop", 1562.5, "s^(-1)", ">=0");
    defineParameter("activeHopToPlusBiasEnergy", 1., "kT", "real");

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
    defineParameter("baseRateZeroToOneExtremitiesConnected", 0.0, "s^(-1)", ">=0");
    defineParameter("baseRateOneToZeroExtremitiesConnected", 0.0, "s^(-1)", ">=0");
    defineParameter("baseRateOneToTwoExtremitiesConnected", 0.0, "s^(-1)", ">=0");
    defineParameter("baseRateTwoToOneExtremitiesConnected", 0.0, "s^(-1)", ">=0");
    defineParameter("headBindingBiasEnergy", 0.0, "kT", "real");

    // Parameters to turn analysis on or off
    defineParameter("addTheoreticalCounterForce", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("samplePositionalDistribution", "FALSE", "unitless", "TRUE,FALSE");
    defineParameter("positionalHistogramBinSize", 8.e-3, "micron", ">=0");
    defineParameter("positionalHistogramLowestValue", 0.0, "micron", "real");
    defineParameter("positionalHistogramHighestValue", 8.e-3, "micron", "real");
}
