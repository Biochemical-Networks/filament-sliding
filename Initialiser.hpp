#ifndef INITIALISER_HPP
#define INITIALISER_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleDynamics.hpp"

#include "Crosslinker.hpp"

#include <string>

/* Initialiser is the classed used for setting the initial SystemState.
 * For example, it sets the initial numbers and locations of bound crosslinkers.
 */

class Initialiser
{
/*public:
    enum class InitialCrosslinkerDistribution
    {
        RANDOM,
        HEADSMOBILE,
        TAILSMOBILE,
        TEST
    };*/
private:
    const double m_initialPositionMicrotubule;
    const MicrotubuleDynamics m_microtubuleDynamics;
    const double m_probabilityPartiallyConnectedTip;
    const double m_probabilityFullyConnectedTip;
    const double m_probabilityPartiallyConnectedBlocked;
    const double m_probabilityFullyConnectedBlocked;
    const double m_probabilityPartialBoundOnTipOutsideOverlap;
    const double m_probabilityTipUnblocked;
    const int32_t m_tipLength;
    /*InitialCrosslinkerDistribution m_initialCrosslinkerDistribution;*/

    void initialiseCrosslinkers(SystemState& systemState, RandomGenerator& generator) const;

    void nCrosslinkersEachTypeToConnect(int32_t& nPassiveCrosslinkersToConnect,
                                        int32_t& nDualCrosslinkersToConnect,
                                        int32_t& nActiveCrosslinkersToConnect,
                                        const int32_t nSitesToConnect,
                                        const int32_t nFreeCrosslinkers,
                                        const int32_t nFreePassiveCrosslinkers,
                                        const int32_t nFreeDualCrosslinkers,
                                        const int32_t nFreeActiveCrosslinkers) const;

    void initialiseBlockedSites(SystemState& systemState, RandomGenerator& generator) const;

    /*Crosslinker::Terminus terminusToConnectToFixedMicrotubule(RandomGenerator &generator);*/ // is always TAIL, HEAD binds to actin by choice

public:
    Initialiser(const double initialPositionMicrotubule,
                const MicrotubuleDynamics microtubuleDynamics,
                const double probabilityPartiallyConnectedTip,
                const double probabilityFullyConnectedTip,
                const double probabilityPartiallyConnectedBlocked,
                const double probabilityFullyConnectedBlocked,
                const double probabilityPartialBoundOnTipOutsideOverlap,
                const double probabilityTipUnblocked,
                const double tipSize,
                const double latticeSpacing);
    ~Initialiser();

    Initialiser(const Initialiser&) = delete;
    Initialiser& operator=(const Initialiser&) = delete;

    void initialise(SystemState& systemState, RandomGenerator& generator) const;

};

#endif // INITIALISER_HPP
