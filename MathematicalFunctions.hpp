#ifndef MATHEMATICALFUNCTIONS_HPP
#define MATHEMATICALFUNCTIONS_HPP

#include <cstdint>

/* Define useful functions that are not present in the standard library or in the GNU Scientific Library (GSL).
 * Also, if a function defined in one of those libraries does not behave as required in a program,
 * a modified version of the function can be defined here.
 */

namespace MathematicalFunctions
{
    double mod(const double x, const double y);

    // The following ceil/floor functions round a real number to the closest integer higher/lower. When the argument is exactly the integer, it is returned as is.
    int32_t intCeil(const double arg);
    int32_t intFloor(const double arg);

    // The alternative ceil/floor functions give the same results as the intCeil/Floor functions, except when the argument is exactly an integer. Then, arg+/-1 is reported.
    // Hence, the answer is always larger/smaller than the argument
    int32_t alternativeIntCeil(const double arg);
    int32_t alternativeIntFloor(const double arg);
}

#endif // MATHEMATICALFUNCTIONS_HPP
