#include "MathematicalFunctions.hpp"

#include <cmath>

double MathematicalFunctions::mod(const double x, const double y)
{
    // Both std::fmod and std::remainder do not give the results I need, need mod(1.1,1) = 0.1, mod(-0.1, 1) = 0.9
    // Use the Euclidean definition of the modulo operation.
    return x - (std::floor(x/std::abs(y))*std::abs(y));
}
