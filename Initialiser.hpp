#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include "Crosslinker.hpp"

#include <string>

class Initialiser
{
public:
    enum class InitialCrosslinkerDistribution
    {
        RANDOM,
        HEADSMOBILE,
        TAILSMOBILE
    };
private:
    const double m_initialPositionMicrotubule;
    const double m_fractionOverlapSitesConnected;
    InitialCrosslinkerDistribution m_initialCrosslinkerDistribution;

    void initialiseCrosslinkers(SystemState& systemState, RandomGenerator& generator);

    void nCrosslinkersEachTypeToConnect(int32_t& nPassiveCrosslinkersToConnect,
                                        int32_t& nDualCrosslinkersToConnect,
                                        int32_t& nActiveCrosslinkersToConnect,
                                        const int32_t nSitesToConnect,
                                        const int32_t nFreeCrosslinkers,
                                        const int32_t nFreePassiveCrosslinkers,
                                        const int32_t nFreeDualCrosslinkers,
                                        const int32_t nFreeActiveCrosslinkers) const;

    Crosslinker::Terminus terminusToConnectToFixedMicrotubule(RandomGenerator &generator);

public:
    Initialiser(const double initialPositionMicrotubule, const double fractionOverlapSitesConnected, const std::string initialCrosslinkerDistributionString);
    ~Initialiser();

    void initialise(SystemState& systemState, RandomGenerator& generator);

};

#endif // INITIALISER_HPP
