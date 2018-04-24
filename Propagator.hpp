#ifndef PROPAGATOR_HPP
#define PROPAGATOR_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory> // std::unique_ptr

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Reaction.hpp"

/* Propagator takes a SystemState, which is properly initialised, and propagates its dynamics.
 * In the process, it can report about the current SystemState, for example about the microtubule position.
 * The class owns the reactions that are possible, and decides which reaction happens when.
 * The dynamics is cut up into time steps for the dynamics of the mobile microtubule movement,
 * since the continuity of the diffusion does not allow for an exact computational model.
 * In contrast, the reactions are modelled using Kinetic Monte Carlo, with dynamic rates integrated over time giving an "action".
 * Here, it is assumed that rates are invariant during a time step.
 */

class Propagator
{
private:
    const int32_t m_nEquilibrationBlocks;
    const int32_t m_nRunBlocks;
    const int32_t m_nTimeSteps;
    const double m_calcTimeStep;
    const int32_t m_probePeriod; // Holds the number of time steps that are between two probes of the system, meaning that data will be written every so many time steps
    const double m_diffusionConstantMicrotubule;
    const double m_springConstant;
    const double m_latticeSpacing;

    const double m_deviationMicrotubule; // sqrt(2 D t), with t the time step size and D the diffusion constant of the microtubule. Stored to prevent this calculation every time step

    double m_currentTime; // Time 0 is at the beginning of the run blocks: the equilibration blocks simulate negative time
    double m_currentReactionRateThreshold; // units s^(-1), so in terms of the (accumulated) rate

    const bool m_samplePositionalDistribution;

    // Store pointers to Reactions in the map m_reactions, because we want to store instances of inherited classes in there. That would not be possible with just the objects.
    // std::unique_ptr deletes the thing it is pointing to when going out of scope, meaning that we don't have to worry about memory leaks
    std::unordered_map<std::string, std::unique_ptr<Reaction>> m_reactions;

    void moveMicrotubule(SystemState& systemState, RandomGenerator& generator);

    void performReaction(SystemState& systemState, RandomGenerator& generator);

    void setNewReactionRateThreshold(const double probability);

    double getTotalAction() const; // The single actions are stored in the Reactions, contained in m_reactions

    void updateAction();

    void resetAction();

    void setRates(const SystemState& systemState);

    double getTotalRate() const;

    Reaction& getReactionToHappen(RandomGenerator& generator) const;

    void advanceTimeStep(SystemState& systemState, RandomGenerator& generator);

    void propagateBlock(SystemState& systemState, RandomGenerator& generator, Output& output, const bool writeOutput, const int32_t nTimeSteps);

public:
    Propagator(const int32_t numberEquilibrationBlocks,
               const int32_t numberRunBlocks,
               const int32_t nTimeSteps,
               const double calcTimeStep,
               const int32_t probePeriod,
               const double diffusionConstantMicrotubule,
               const double springConstant,
               const double latticeSpacing,
               const double ratePassivePartialHop,
               const double ratePassiveFullHop,
               const double baseRateActivePartialHop,
               const double baseRateActiveFullHop,
               const double activeHopToPlusBiasEnergy,
               const double baseRateZeroToOneExtremitiesConnected,
               const double baseRateOneToZeroExtremitiesConnected,
               const double baseRateOneToTwoExtremitiesConnected,
               const double baseRateTwoToOneExtremitiesConnected,
               const double headBindingBiasEnergy,
               RandomGenerator& generator,
               const bool samplePositionalDistribution);
    ~Propagator();

    Propagator(const Propagator&) = delete;
    Propagator& operator=(const Propagator&) = delete;

    // Enter the SystemState as a reference into the run function, such that the propagator can propagate it.
    void run(SystemState& systemState, RandomGenerator& generator, Output& output);
    void equilibrate(SystemState& systemState, RandomGenerator& generator, Output& output);
    void propagateGraphicsInterval(SystemState& systemState, RandomGenerator& generator, Output& output, const int32_t nTimeStepsInterval);
};

#endif // PROPAGATOR_HPP
