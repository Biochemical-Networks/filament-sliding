#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

class Initialiser
{
private:

public:
    Initialiser();
    ~Initialiser();

    void initialise(SystemState& systemState, RandomGenerator& generator);
};

#endif // INITIALISER_HPP
