#include "MathematicalFunctions.hpp"

#include <cmath>
#include <cstdint>

double MathematicalFunctions::mod(const double x, const double y)
{
    // Both std::fmod and std::remainder do not give the results I need, need mod(1.1,1) = 0.1, mod(-0.1, 1) = 0.9
    // Use the Euclidean definition of the modulo operation.
    return x - (std::floor(x/std::abs(y))*std::abs(y));
}

int32_t MathematicalFunctions::intCeil(const double arg)
{
    return static_cast<int32_t>(std::ceil(arg));
}

int32_t MathematicalFunctions::intFloor(const double arg)
{
    return static_cast<int32_t>(std::floor(arg));
}

int32_t MathematicalFunctions::alternativeIntCeil(const double arg)
{
    return intFloor(arg)+1;
}

int32_t MathematicalFunctions::alternativeIntFloor(const double arg)
{
    return intCeil(arg)-1;
}
