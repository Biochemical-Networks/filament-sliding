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

    defineParameter("numberActiveCrosslinkers", 0, "crosslinkers", ">=0");
    defineParameter("numberDualCrosslinkers", 0, "crosslinkers", ">=0");
    defineParameter("numberPassiveCrosslinkers", 1000, "crosslinkers", ">=0");
    defineParameter("fractionConnectedCrosslinkers", 1.0, "unitless", ">0");
    defineParameter("initialCrosslinkerDistribution", "unitless", "RANDOM", "RANDOM,HEADSMOBILE,TAILSMOBILE,ALLCONNECTED"); // Can only hold the values set by Initialiser::InitialCrosslinkerDistribution

    // Initial state parameters
    defineParameter("initialPositionMicrotubule", 10., "micron"); // The position of the mobile microtubule relative to the fixed one, where 0. means that the two have the same (left) boundary position

    // Dynamics parameters
    defineParameter("diffusionConstantMicrotubule", 10., "micron^(2)*s^(-1)", ">=0");
    defineParameter("springConstant", 1., "kT*micron^(-2)", ">=0");
}
