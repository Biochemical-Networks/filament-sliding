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
     */
    defineParameter("runName", "run"); // The code relies on this parameter being called "runName"
    defineParameter("numberEquilibrationBlocks", 50, "blocks");
    defineParameter("numberRunBlocks", 100, "blocks");
    defineParameter("calcTimeStep", 1.e-3, "seconds");

    defineParameter("lengthMobileMicrotubule", 50., "micrometer");
    defineParameter("lengthFixedMicrotubule", 50., "micrometer");
    defineParameter("latticeSpacing", 8.e-3, "micrometer");
    defineParameter("diffusionConstantMicrotubule", 10., "micrometer^(2) second^(-1)");

    defineParameter("numberActiveCrosslinkers", 0, "crosslinkers");
    defineParameter("numberDualCrosslinkers", 0, "crosslinkers");
    defineParameter("numberPassiveCrosslinkers", 1000, "crosslinkers");
    defineParameter("springConstant", 1., "kT micrometer^(-2)");
}
