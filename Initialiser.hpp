#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

class Initialiser
{
private:
    const double m_initialPositionMicrotubule;
public:
    Initialiser(const double initialPositionMicrotubule);
    ~Initialiser();

    void initialise(SystemState& systemState, RandomGenerator& generator);
};

#endif // INITIALISER_HPP
