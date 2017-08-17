#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

class Initialiser
{
public:
    enum class InitialCrosslinkerDistribution
    {
        RANDOM,
        HEADSMOBILE,
        TAILSMOBILE,
        ALLCONNECTED
    };
private:
    const double m_initialPositionMicrotubule;
    const double m_fractionConnectedCrosslinkers;
    const InitialCrosslinkerDistribution m_method;
public:
    Initialiser(const double initialPositionMicrotubule, const double fractionConnectedCrosslinkers, const InitialCrosslinkerDistribution method);
    ~Initialiser();

    void initialise(SystemState& systemState, RandomGenerator& generator);
};

#endif // INITIALISER_HPP
