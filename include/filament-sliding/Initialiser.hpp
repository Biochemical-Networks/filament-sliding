#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include <string>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/SystemState.hpp"

/* Initialiser is the classed used for setting the initial SystemState.
 * For example, it sets the initial numbers and locations of bound crosslinkers.
 */

class Initialiser {
  public:
    enum class InitialCrosslinkerDistribution {
        RANDOM,
        HEADSMOBILE,
        TAILSMOBILE,
        TEST
    };

  private:
    const double m_initialPositionMicrotubule;
    const double m_fractionOverlapSitesConnected;
    InitialCrosslinkerDistribution m_initialCrosslinkerDistribution;

    void initialiseCrosslinkers(
            SystemState& systemState,
            RandomGenerator& generator);

    void nCrosslinkersEachTypeToConnect(
            int32_t& nPassiveCrosslinkersToConnect,
            int32_t& nDualCrosslinkersToConnect,
            int32_t& nActiveCrosslinkersToConnect,
            const int32_t nSitesToConnect,
            const int32_t nFreeCrosslinkers,
            const int32_t nFreePassiveCrosslinkers,
            const int32_t nFreeDualCrosslinkers,
            const int32_t nFreeActiveCrosslinkers) const;

    Crosslinker::Terminus terminusToConnectToFixedMicrotubule(
            RandomGenerator& generator);

  public:
    Initialiser(
            const double initialPositionMicrotubule,
            const double fractionOverlapSitesConnected,
            const std::string initialCrosslinkerDistributionString);
    ~Initialiser();

    Initialiser(const Initialiser&) = delete;
    Initialiser& operator=(const Initialiser&) = delete;

    void initialise(SystemState& systemState, RandomGenerator& generator);
};

#endif // INITIALISER_HPP
