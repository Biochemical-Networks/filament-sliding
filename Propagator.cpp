#include "Propagator.hpp"

#include "RandomGenerator.hpp"
#include "SystemState.hpp"
#include "Output.hpp"
#include "Reaction.hpp"

#include <cstdint>
#include <string>
#include <random>
#include <cmath>



Propagator::Propagator(const int32_t nTimeSteps,
                       const double calcTimeStep,
                       const int32_t probePeriod,
                       const double diffusionConstantMicrotubule,
                       const double springConstant,
                       const double rateZeroToOneExtremitiesConnected,
                       const double rateOneToZeroExtremitiesConnected,
                       const double rateOneToTwoExtremitiesConnected,
                       const double rateTwoToOneExtremitiesConnected)
    :   m_nTimeSteps(nTimeSteps),
        m_calcTimeStep(calcTimeStep),
        m_probePeriod(probePeriod),
        m_diffusionConstantMicrotubule(diffusionConstantMicrotubule),
        m_springConstant(springConstant),
        m_deviationMicrotubule(std::sqrt(2*m_diffusionConstantMicrotubule*m_calcTimeStep)),
        m_reactions({
                        {"bindingFreePassiveCrosslinker", Reaction(rateZeroToOneExtremitiesConnected)},
                        {"bindingFreeDualCrosslinker", Reaction(rateZeroToOneExtremitiesConnected)},
                        {"bindingFreeActiveCrosslinker", Reaction(rateZeroToOneExtremitiesConnected)}
                    })
{
}

Propagator::~Propagator()
{
}

void Propagator::run(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    m_currentReactionRateThreshold = getNewReactionRateThreshold(generator.getProbability()); // Initialise the threshold, which is used to decide when a reaction will fire

    for (int32_t timeStep = 0; timeStep < m_nTimeSteps; ++timeStep)
    {
        if (timeStep%m_probePeriod==0)
        {
            output.writeMicrotubulePosition(timeStep*m_calcTimeStep, systemState);
        }

        moveMicrotubule(systemState, generator);





    }
    output.writeMicrotubulePosition(m_nTimeSteps*m_calcTimeStep, systemState); // Write the final state as well. The time it writes at is not equidistant compared to the previous writing times, when probePeriod does not divide nTimeSteps
}

void Propagator::moveMicrotubule(SystemState& systemState, RandomGenerator& generator)
{
    double change = generator.getGaussian(0.0, m_deviationMicrotubule);
    systemState.update(change);
}

void Propagator::performReaction(SystemState& systemState, RandomGenerator& generator)
{

}

double Propagator::getNewReactionRateThreshold(const double probability)
{
    return -std::log(probability)/m_calcTimeStep;
}
