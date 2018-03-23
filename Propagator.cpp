#include "Propagator.hpp"

#include "RandomGenerator.hpp"
#include "SystemState.hpp"
#include "Output.hpp"
#include "Reaction.hpp"
#include "BindFreeCrosslinker.hpp"
#include "BindPartialCrosslinker.hpp"
#include "UnbindPartialCrosslinker.hpp"
#include "UnbindFullCrosslinker.hpp"
#include "HopPartial.hpp"
#include "HopFull.hpp"
#include "Crosslinker.hpp"

#include <cstdint>
#include <string>
#include <random>
#include <cmath>
#include <memory>
#include <utility> // pair



Propagator::Propagator(const int32_t nTimeSteps,
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
                       const double headBindingBiasEnergy)
    :   m_nTimeSteps(nTimeSteps),
        m_calcTimeStep(calcTimeStep),
        m_probePeriod(probePeriod),
        m_diffusionConstantMicrotubule(diffusionConstantMicrotubule),
        m_springConstant(springConstant),
        m_latticeSpacing(latticeSpacing),
        m_deviationMicrotubule(std::sqrt(2*m_diffusionConstantMicrotubule*m_calcTimeStep))
{
    // objects in std::initializer_list are inherently const, so std::unique_ptr's copy constructor cannot be used there, and we cannot use this method of initialising m_reactions.
    // See https://stackoverflow.com/questions/38213088/initialize-static-stdmap-with-unique-ptr-as-value
    // Use pointers, because m_reactions contains Reactions, while we want to store objects of a class inherited from Reaction
    // Make sure that the specific reaction has been included in this file
    m_reactions["bindingFreePassiveCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, Crosslinker::Type::PASSIVE, headBindingBiasEnergy));
    m_reactions["bindingFreeDualCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, Crosslinker::Type::DUAL, headBindingBiasEnergy));
    m_reactions["bindingFreeActiveCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, Crosslinker::Type::ACTIVE, headBindingBiasEnergy));
    m_reactions["bindingPartialPassiveCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::PASSIVE, headBindingBiasEnergy, m_springConstant));
    m_reactions["bindingPartialDualCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::DUAL, headBindingBiasEnergy, m_springConstant));
    m_reactions["bindingPartialActiveCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::ACTIVE, headBindingBiasEnergy, m_springConstant));
    m_reactions["unbindingPartialPassiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, Crosslinker::Type::PASSIVE, headBindingBiasEnergy));
    m_reactions["unbindingPartialDualCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, Crosslinker::Type::DUAL, headBindingBiasEnergy));
    m_reactions["unbindingPartialActiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, Crosslinker::Type::ACTIVE, headBindingBiasEnergy));
    m_reactions["unbindingFullPassiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::PASSIVE, headBindingBiasEnergy, m_springConstant));
    m_reactions["unbindingFullDualCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::DUAL, headBindingBiasEnergy, m_springConstant));
    m_reactions["unbindingFullActiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::ACTIVE, headBindingBiasEnergy, m_springConstant));
    m_reactions["hoppingPartialPassiveCrosslinker"] = std::unique_ptr<Reaction>(new HopPartial(ratePassivePartialHop, ratePassivePartialHop, Crosslinker::Type::PASSIVE, 0.0, 0.0)); // For a passive linker, the bias energy is zero
    m_reactions["hoppingPartialDualCrosslinker"] = std::unique_ptr<Reaction>(new HopPartial(baseRateActivePartialHop, ratePassivePartialHop, Crosslinker::Type::DUAL, activeHopToPlusBiasEnergy, 0.0));
    m_reactions["hoppingPartialActiveCrosslinker"] = std::unique_ptr<Reaction>(new HopPartial(baseRateActivePartialHop, baseRateActivePartialHop, Crosslinker::Type::ACTIVE, activeHopToPlusBiasEnergy, activeHopToPlusBiasEnergy));
    m_reactions["hoppingFullPassiveCrosslinker"] = std::unique_ptr<Reaction>(new HopFull(ratePassiveFullHop, ratePassiveFullHop, Crosslinker::Type::PASSIVE, m_springConstant, 0.0, 0.0));
    m_reactions["hoppingFullDualCrosslinker"] = std::unique_ptr<Reaction>(new HopFull(baseRateActiveFullHop, ratePassiveFullHop, Crosslinker::Type::DUAL, m_springConstant, activeHopToPlusBiasEnergy, 0.0));
    m_reactions["hoppingFullActiveCrosslinker"] = std::unique_ptr<Reaction>(new HopFull(baseRateActiveFullHop, baseRateActiveFullHop, Crosslinker::Type::ACTIVE, m_springConstant, activeHopToPlusBiasEnergy, activeHopToPlusBiasEnergy));

    // The standard deviation of the average microtubule position update should be much smaller (orders of magnitude smaller) than the lattice spacing,
    // since that sets a scale over which force differences definitely emerge. The choice 0.1 is pretty large, but this is a hard maximum limit.
    if(m_deviationMicrotubule>(0.1*m_latticeSpacing))
    {
        throw GeneralException("The time step is too large to allow for the approximate microtubule movement to be trustworthy. See Propagator constructor");
    }
}

Propagator::~Propagator()
{
}

// The reaction rate is integrated discretely over the time steps, and leads to a reaction when this accumulated reaction rate (or action) comes above a certain threshold.
// The threshold is set by inverting the survival probability: map probabilities to survival times. Then, draw a random probability to get the survival time.

void Propagator::run(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    // Initialise the threshold, which is used to decide when a reaction will fire
    setNewReactionRateThreshold(generator.getProbability());

    for (int32_t timeStep = 0; timeStep < m_nTimeSteps; ++timeStep)
    {
        if (timeStep%m_probePeriod==0)
        {
            output.writeMicrotubulePosition(timeStep*m_calcTimeStep, systemState);
        }

        advanceTimeStep(systemState, generator);

    }
    output.writeMicrotubulePosition(m_nTimeSteps*m_calcTimeStep, systemState); // Write the final state as well. The time it writes at is not equidistant compared to the previous writing times, when probePeriod does not divide nTimeSteps
}

void Propagator::advanceTimeStep(SystemState& systemState, RandomGenerator& generator)
{
    // First, update the reaction rates and actions, and perform a reaction when the total action surpasses the threshold.
    // Then, move the mobile microtubule at the end of the time step
    setRates(systemState);
    updateAction();
    if (getTotalAction() > m_currentReactionRateThreshold)
    {
        performReaction(systemState, generator); // also updates the force and action
    }
    moveMicrotubule(systemState, generator);
}

void Propagator::moveMicrotubule(SystemState& systemState, RandomGenerator& generator)
{
    std::pair<double,double> exclusiveMovementBorders = systemState.movementBordersSetByFullLinkers();
    double change;
    /* Ask the generator to propose a change to update the position of the microtubule.
     * Repeat until the proposed change is within the allowed range of movement set by the full linkers, which have a maximum stretch.
     * The repetition should not happen too often: this means that the time step cannot be too big.
     * For a maxStretch of 1.5 latticeSpacing, there is a guaranteed open interval of possible changes of the size of one lattice spacing.
     * For time steps small enough, such that the usually proposed changes are small, this function will not repeat often, even for very stretched conformations.
     * Hence, the repetition is a proper choice for the updating algorithm.
     */

    #ifdef MYDEBUG
    std::cout << exclusiveMovementBorders.first << " " << exclusiveMovementBorders.second << std::endl;
    int32_t trial = 0;
    #endif // MYDEBUG

    do
    {
        // The mean should be proportional to the force (overdamped system)
        // mean change = mobility*force*timeStep
        // units:   [timeStep] = s
        //          [force] = (kT)*micron^(-1)
        //          [mobility] = micron^(2)*(kT)^(-1)*s^(-1)
        // In these units, mobility has the same value as the diffusion constant, which has units micron^(2)*s^(-1), and mobility = D/(kT)
        change = generator.getGaussian(m_diffusionConstantMicrotubule*systemState.getForce()*m_calcTimeStep, m_deviationMicrotubule);

        #ifdef MYDEBUG
        ++trial;
        std::cout << "Trial number: "<< trial << '\n';
        std::cout << "Proposed change: " << change << '\n';
        #endif // MYDEBUG
    }
    while (change<=exclusiveMovementBorders.first || change>=exclusiveMovementBorders.second);

    systemState.updateMobilePosition(change);
    systemState.updateForceAndEnergy();
}

void Propagator::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    getReactionToHappen(generator).performReaction(systemState, generator);
    resetAction();
    setNewReactionRateThreshold(generator.getProbability());
    systemState.updateForceAndEnergy();
    #ifdef MYDEBUG
    static int32_t numberOfReactions = 1;
    ++numberOfReactions;
    std::cout << "A reaction happened, so far we've seen "<< numberOfReactions << " reactions.\n";
    #endif // MYDEBUG
}

// Invert the survival probability vs the integrated reaction rate
void Propagator::setNewReactionRateThreshold(const double probability)
{
    // The action threshold is in units of the time step
    // If probability is in the open interval (0,1), then the threshold is in the range (0, infty)
    m_currentReactionRateThreshold = -std::log(probability)/m_calcTimeStep;
}

double Propagator::getTotalAction() const
{
    double accumulatedAction = 0.0;

    // auto becomes a std::pair of the relevant types. Needs to be reference, because std::unique_ptr does not have a copy constructor
    for(const auto& reaction : m_reactions)
    {
        accumulatedAction += reaction.second->getAction();
    }

    return accumulatedAction; // The actions in the reactions contain the action in units of the time step
}

void Propagator::updateAction()
{
    for(auto& reaction : m_reactions)
    {
        reaction.second->updateAction();
    }
}

void Propagator::resetAction()
{
    for(auto& reaction : m_reactions)
    {
        reaction.second->resetAction();
    }
}

void Propagator::setRates(const SystemState& systemState)
{
    for(auto& reaction : m_reactions)
    {
        reaction.second->setCurrentRate(systemState);
    }
}

double Propagator::getTotalRate() const
{
    double accumulatedRate = 0.0;
    // auto becomes a std::pair of the relevant types. Needs to be reference, because std::unique_ptr does not have a copy constructor
    for(const auto& reaction : m_reactions)
    {
        accumulatedRate += reaction.second->getCurrentRate();
    }

    return accumulatedRate;
}

Reaction& Propagator::getReactionToHappen(RandomGenerator& generator) const
{
    double randomNumber = generator.getUniform(0.0, getTotalRate()); // uses interval [0,totalRate)

    for (const auto& reaction : m_reactions)
    {
        randomNumber -= reaction.second->getCurrentRate();
        // If the randomNumber falls exactly on the border between two reactions, the later one is chosen. Since 0 is an option, this does not bias any reaction
        // randomNumber is strictly smaller than the upper bound totalAccumulatedAction, so subtracting the total upper bound after the for loop will definitely make it lower than 0.0
        if (randomNumber < 0.0)
        {
            return *reaction.second;
        }
    }

    throw GeneralException("Something went wrong in Propagator::getReactionToHappen()");
}



