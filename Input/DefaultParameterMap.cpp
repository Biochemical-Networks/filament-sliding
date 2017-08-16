#include "ParameterMap.hpp"

#include "GenericValue.hpp"
#include <string>


ParameterMap::ParameterMap()
{
    /* Define all input parameters here.
     * defineParameter takes the name (string), value (T), unit (string) (optional). If no unit is given, "unitless" is added.
     * unit cannot be an empty string.
     * Use template argument deduction to call defineParameter. The type does not need to be given.
     * If a parameter is to be of type double, always add a floating point (e.g. "4.") to make it double
     * The strings (for example the unit!) are NOT allowed to contain white space, since reading will then try to read the next field
     */
     // Run parameters
    defineParameter("runName", "run"); // The code relies on this parameter being called "runName"
    defineParameter("numberEquilibrationBlocks", 50, "blocks");
    defineParameter("numberRunBlocks", 100, "blocks");
    defineParameter("calcTimeStep", 1.e-3, "s");
    defineParameter("numberTimeSteps", 1000, "steps");
    defineParameter("probePeriod", 10, "steps");

    // General state parameters
    defineParameter("lengthMobileMicrotubule", 50., "micron");
    defineParameter("lengthFixedMicrotubule", 50., "micron");
    defineParameter("latticeSpacing", 8.e-3, "micron");

    defineParameter("numberActiveCrosslinkers", 0, "crosslinkers");
    defineParameter("numberDualCrosslinkers", 0, "crosslinkers");
    defineParameter("numberPassiveCrosslinkers", 1000, "crosslinkers");

    // Initial state parameters
    defineParameter("initialPositionMicrotubule", 10., "micron"); // The position of the mobile microtubule relative to the fixed one, where 0. means that the two have the same (left) boundary position

    // Dynamics parameters
    defineParameter("diffusionConstantMicrotubule", 10., "micron^(2)*s^(-1)");
    defineParameter("springConstant", 1., "kT*micron^(-2)");
}
