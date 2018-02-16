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
    defineParameter("numberEquilibrationBlocks", 50, "blocks", ">=0");
    defineParameter("numberRunBlocks", 100, "blocks", ">=0");
    defineParameter("calcTimeStep", 1.e-3, "s", ">0");
    defineParameter("numberTimeSteps", 1000, "steps", ">=0");
    defineParameter("probePeriod", 10, "steps", ">0");

    // General state parameters
    defineParameter("lengthMobileMicrotubule", 50., "micron", ">0");
    defineParameter("lengthFixedMicrotubule", 50., "micron", ">0");
    defineParameter("latticeSpacing", 8.e-3, "micron", ">0");

    defineParameter("numberActiveCrosslinkers", 5000, "crosslinkers", ">=0");
    defineParameter("numberDualCrosslinkers", 5000, "crosslinkers", ">=0");
    defineParameter("numberPassiveCrosslinkers", 5000, "crosslinkers", ">=0");
    defineParameter("fractionOverlapSitesConnected", 1.0, "unitless", ">0");
    defineParameter("initialCrosslinkerDistribution", "RANDOM", "unitless", "RANDOM,HEADSMOBILE,TAILSMOBILE,TEST"); // Can only hold the values set by Initialiser::InitialCrosslinkerDistribution

    // Initial state parameters
    defineParameter("initialPositionMicrotubule", 0., "micron"); // The position of the mobile microtubule relative to the fixed one, where 0. means that the two have the same (left) boundary position
    defineParameter("numberInitiallyBoundCrosslinkers", 100, "crosslinkers", ">0");

    // Dynamics parameters
    defineParameter("diffusionConstantMicrotubule", 10., "micron^(2)*s^(-1)", ">=0");
    defineParameter("springConstant", 1., "kT*micron^(-2)", ">=0");

    // Rates for connecting crosslinkers to the microtubules. The rates from zero to one are per crosslinker in solution per site.
    // So the full rate for connecting to any site is r * #free crosslinkers * # free microtubule sites
    // Detailed balance is obeyed, because the free energy is the same whether a head or a tail is connected, and whether you fully connect from a head or a tail.
    // Hence, the free energy difference between the fully connected and free crosslinker does not depend on the path (first head or first tail), and there is no net flux through the cycle.
    defineParameter("rateZeroToOneExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToZeroExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToTwoExtremitiesConnected", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTwoToOneExtremitiesConnected", 1.0, "s^(-1)", ">=0");

/*    // For if the rates should be different for different types. Notice that much can be achieved by setting different rates for
    defineParameter("rateZeroToOneConnectedPassive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToZeroConnectedPassive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToTwoConnectedPassive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTwoToOneConnectedPassive", 1.0, "s^(-1)", ">=0");
    // Detailed balance is obeyed, because the free energy is the same whether a head or a tail is connected, and whether you fully connect from a head or a tail.
    // Hence, the free energy difference between the fully connected and free crosslinker does not depend on the path, and there is no net flux through the cycle.

    defineParameter("rateZeroToHeadConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateHeadToZeroConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateZeroToTailConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTailToZeroConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateHeadToTwoConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTwoToHeadConnectedDual", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTailToTwoConnectedDual", 1.0, "s^(-1)", ">=0");
    // rateTailToTwoConnectedDual is set by the other 7 rates, using detailed balance

    defineParameter("rateZeroToOneConnectedActive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToZeroConnectedActive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateOneToTwoConnectedActive", 1.0, "s^(-1)", ">=0");
    defineParameter("rateTwoToOneConnectedActive", 1.0, "s^(-1)", ">=0");*/
}
