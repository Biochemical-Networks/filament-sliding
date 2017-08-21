#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <string>

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
    InitialCrosslinkerDistribution m_initialCrosslinkerDistribution;
public:
    Initialiser(const double initialPositionMicrotubule, const double fractionConnectedCrosslinkers, const std::string initialCrosslinkerDistributionString);
    ~Initialiser();

    void initialise(SystemState& systemState, RandomGenerator& generator);
};

#endif // INITIALISER_HPP
