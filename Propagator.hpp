#ifndef PROPAGATOR_HPP
#define PROPAGATOR_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Reaction.hpp"

#include <memory>

class Propagator
{
private:
    const int32_t m_nTimeSteps;
    const double m_calcTimeStep;
    const int32_t m_probePeriod; // Holds the number of time steps that are between two probes of the system, meaning that data will be written every so many time steps
    const double m_diffusionConstantMicrotubule;
    const double m_springConstant;

    const double m_deviationMicrotubule; // sqrt(2 D t), with t the time step size and D the diffusion constant of the microtubule. Stored to prevent this calculation every time step

    double m_currentReactionRateThreshold;

    // std::unique_ptr deletes the thing it is pointing to when going out of scope, meaning that we don't have to worry about memory leaks
    std::unordered_map<std::string, std::unique_ptr<Reaction>> m_reactions;

    void moveMicrotubule(SystemState& systemState, RandomGenerator& generator);

    void performReaction(SystemState& systemState, RandomGenerator& generator);

    double getNewReactionRateThreshold(const double probability);

public:
    Propagator(const int32_t nTimeSteps,
               const double calcTimeStep,
               const int32_t probePeriod,
               const double diffusionConstantMicrotubule,
               const double springConstant,
               const double rateZeroToOneExtremitiesConnected,
               const double rateOneToZeroExtremitiesConnected,
               const double rateOneToTwoExtremitiesConnected,
               const double rateTwoToOneExtremitiesConnected);
    ~Propagator();

    // Enter the SystemState as a reference into the run function, such that the propagator can propagate it.
    void run(SystemState& systemState, RandomGenerator& generator, Output& output);

};

#endif // PROPAGATOR_HPP
