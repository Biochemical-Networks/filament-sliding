#include "Propagator.hpp"

#include "RandomGenerator.hpp"
#include "SystemState.hpp"
#include "Output.hpp"
#include "Reaction.hpp"
#include "BindFreeCrosslinker.hpp"
#include "BindPartialCrosslinker.hpp"
#include "UnbindPartialCrosslinker.hpp"
#include "UnbindFullCrosslinker.hpp"
#include "GrowFixedMicrotubule.hpp"
#include "RemoveSite.hpp"
#include "Crosslinker.hpp"
#include "MathematicalFunctions.hpp"
#include "ActinDisconnectException.hpp"
#include "ConnectedTipGrowth.hpp"

#include <cstdint>
#include <string>
#include <random>
#include <cmath>
#include <memory>
#include <utility> // pair



Propagator::Propagator(const int32_t numberEquilibrationBlocks,
                       const int32_t numberRunBlocks,
                       const int32_t nTimeSteps,
                       const double calcTimeStep,
                       const int32_t positionProbePeriod,
                       const double diffusionConstantMicrotubule,
                       const double actinDisconnectTime,
                       const double springConstant,
                       const double latticeSpacing,
                       const MicrotubuleDynamics microtubuleDynamics,
                       const double baseRateZeroToOneExtremitiesConnected,
                       const double baseRateOneToZeroExtremitiesConnected,
                       const double baseRateOneToTwoExtremitiesConnected,
                       const double baseRateTwoToOneExtremitiesConnected,
                       const double baseRateZeroToOneExtremitiesConnectedBlocked,
                       const double baseRateOneToZeroExtremitiesConnectedBlocked,
                       const double rateFixedMicrotubuleGrowth,
                       const double rateBlockBoundSites,
                       const double rateBlockUnboundSites,
                       const bool unbindUponBlock,
                       RandomGenerator& generator,
                       const bool samplePositionalDistribution,
                       const bool addExternalForce,
                       const bool actinInitiallyOnTip,
                       Log& log)
    :   m_nEquilibrationBlocks(numberEquilibrationBlocks),
        m_nRunBlocks(numberRunBlocks),
        m_nTimeSteps(nTimeSteps),
        m_calcTimeStep(calcTimeStep),
        m_positionProbePeriod(positionProbePeriod),
        m_diffusionConstantMicrotubule(diffusionConstantMicrotubule),
        m_actinDisconnectTime(actinDisconnectTime),
        m_springConstant(springConstant),
        m_latticeSpacing(latticeSpacing),
        m_microtubuleDynamics(microtubuleDynamics),
        m_deviationMicrotubule(std::sqrt(2*m_diffusionConstantMicrotubule*m_calcTimeStep)),
        m_currentTime(-m_nEquilibrationBlocks*m_nTimeSteps*m_calcTimeStep), // time 0 is the start of the run blocks
        m_samplePositionalDistribution(samplePositionalDistribution),
        m_addExternalForce(addExternalForce),
        m_nDeterministicBoundaryCrossings(0), // Counts the number of times a force has tried to push the mobile microtubule across a maximum stretch barrier
        m_nStochasticBoundaryCrossings(0), // Counts the number of times diffusion of the mobile microtubule was reflected at a maximum stretch barrier
        m_log(log),
        m_actinIsFree(false), // start with connected filaments
        m_timeFreeActin(0.0),
        m_actinWasOnTip(actinInitiallyOnTip),
        m_timeLastTrackingCompletion(0.0),
        m_totalTimeBehindTip(0.0),
        m_totalTimeOnTip(0.0)
{
    // objects in std::initializer_list are inherently const, so std::unique_ptr's copy constructor cannot be used there, and we cannot use this method of initialising m_reactions.
    // See https://stackoverflow.com/questions/38213088/initialize-static-stdmap-with-unique-ptr-as-value
    // Use pointers, because m_reactions contains Reactions, while we want to store objects of a class inherited from Reaction
    // Make sure that the specific reaction has been included in this file
    m_reactions["bindingFreePassiveCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, baseRateZeroToOneExtremitiesConnectedBlocked, Crosslinker::Type::PASSIVE));
    m_reactions["bindingFreeDualCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, baseRateZeroToOneExtremitiesConnectedBlocked, Crosslinker::Type::DUAL));
    m_reactions["bindingFreeActiveCrosslinker"] = std::unique_ptr<Reaction>(new BindFreeCrosslinker(baseRateZeroToOneExtremitiesConnected, baseRateZeroToOneExtremitiesConnectedBlocked, Crosslinker::Type::ACTIVE));
    m_reactions["bindingPartialPassiveCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::PASSIVE, m_springConstant));
    m_reactions["bindingPartialDualCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::DUAL, m_springConstant));
    m_reactions["bindingPartialActiveCrosslinker"] = std::unique_ptr<Reaction>(new BindPartialCrosslinker(baseRateOneToTwoExtremitiesConnected, Crosslinker::Type::ACTIVE, m_springConstant));
    m_reactions["unbindingPartialPassiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, baseRateOneToZeroExtremitiesConnectedBlocked, Crosslinker::Type::PASSIVE));
    m_reactions["unbindingPartialDualCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, baseRateOneToZeroExtremitiesConnectedBlocked, Crosslinker::Type::DUAL));
    m_reactions["unbindingPartialActiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindPartialCrosslinker(baseRateOneToZeroExtremitiesConnected, baseRateOneToZeroExtremitiesConnectedBlocked, Crosslinker::Type::ACTIVE));
    m_reactions["unbindingFullPassiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::PASSIVE, m_springConstant));
    m_reactions["unbindingFullDualCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::DUAL, m_springConstant));
    m_reactions["unbindingFullActiveCrosslinker"] = std::unique_ptr<Reaction>(new UnbindFullCrosslinker(baseRateTwoToOneExtremitiesConnected, Crosslinker::Type::ACTIVE, m_springConstant));

    switch(m_microtubuleDynamics)
    {
    case MicrotubuleDynamics::STOCHASTIC:
        m_reactions["fixedMicrotubuleGrowth"] = std::unique_ptr<Reaction>(new GrowFixedMicrotubule(rateFixedMicrotubuleGrowth));
        m_reactions["fixedMicrotubuleDecay"] = std::unique_ptr<Reaction>(new RemoveSite(rateBlockBoundSites, rateBlockUnboundSites, unbindUponBlock));
        break;
    case MicrotubuleDynamics::DETERMINISTIC:
        m_reactions["connectedTipGrowth"] = std::unique_ptr<Reaction>(new ConnectedTipGrowth(rateFixedMicrotubuleGrowth, unbindUponBlock));
        break;
    default:
        throw GeneralException("Propagator::Propagator() encountered a wrong MicrotubuleDynamics");
    }

    // The standard deviation of the average microtubule position update should be much smaller (orders of magnitude smaller) than the lattice spacing,
    // since that sets a scale over which force differences definitely emerge. The choice 0.1 is pretty large, but this is a hard maximum limit.
    if(m_deviationMicrotubule>(0.1*m_latticeSpacing))
    {
        throw GeneralException("The time step is too large to allow for the approximate microtubule movement to be trustworthy. See Propagator constructor");
    }

    // Initialise the threshold, which is used to decide when a reaction will fire
    // The reaction rate is integrated discretely over the time steps, and leads to a reaction when this accumulated reaction rate (or action) comes above a certain threshold.
    // The threshold is set by inverting the survival probability: map probabilities to survival times. Then, draw a random probability to get the survival time.
    setNewReactionRateThreshold(generator.getProbability());
}

Propagator::~Propagator()
{
    // Since log has to be declared before propagator (the latter uses the former), log will be destroyed after propagator. So the following code is valid:
    m_log.writeBoundaryProtocolAppearance(m_nDeterministicBoundaryCrossings, m_nStochasticBoundaryCrossings);
}

void Propagator::propagateBlock(SystemState& systemState, RandomGenerator& generator, Output& output, const bool writeOutput, const int32_t nTimeSteps)
{
    for (int32_t timeStep = 0; timeStep < nTimeSteps; ++timeStep)
    {
        if(writeOutput)
        {
            if(timeStep%m_positionProbePeriod==0)
            {
                output.writePositionsAndCrosslinkerNumbers(m_currentTime, systemState);
            }
            // Add the microtubule positions more often than the m_positionProbePeriod, since it is not directly written to a file (not slow), and it requires much data.
            if(m_samplePositionalDistribution)
            {
                output.addPosition(MathematicalFunctions::mod(systemState.getMicrotubulePosition(), m_latticeSpacing));
            }
        }

        advanceTimeStep(systemState, generator);
    }
}

void Propagator::equilibrate(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    constexpr bool writeOutput = false;
    for(int32_t blockNumber = 0; blockNumber < m_nEquilibrationBlocks; ++blockNumber)
    {
        propagateBlock(systemState, generator, output, writeOutput, m_nTimeSteps);
    }
}

void Propagator::run(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    constexpr bool writeOutput = true;
    for(int32_t blockNumber = 0; blockNumber < m_nRunBlocks; ++blockNumber)
    {
        /*output.newBlock(blockNumber+1);*/
        propagateBlock(systemState, generator, output, writeOutput, m_nTimeSteps);
    }
}

void Propagator::propagateGraphicsInterval(SystemState& systemState, RandomGenerator& generator, Output& output, const int32_t nTimeStepsInterval)
{
    constexpr bool writeOutput = true;
    static int32_t intervalNumber = 0;
    ++intervalNumber;
    /*output.newBlock(intervalNumber);*/
    propagateBlock(systemState, generator, output, writeOutput, nTimeStepsInterval);
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
    m_currentTime+=m_calcTimeStep;

    const bool isFreeNow = (systemState.getNFullCrosslinkers()==0);
    if(isFreeNow && m_actinIsFree)
    {
        m_timeFreeActin+=m_calcTimeStep;
    }
    else if(isFreeNow && !m_actinIsFree)
    {
        m_timeFreeActin = 0.0;
    }
    m_actinIsFree = isFreeNow; // m_actinIsFree holds the value of isFreeNow of the previous time step

    if(m_actinIsFree && m_timeFreeActin > m_actinDisconnectTime)
    {
        // Actin has fallen off
        throw ActinDisconnectException(m_currentTime,
                                       systemState.getMicrotubulePosition(),
                                       m_timeLastTrackingCompletion,
                                       m_totalTimeBehindTip,
                                       m_totalTimeOnTip);
    }

    const bool actinIsOnTip = systemState.actinOnTip();
    if(actinIsOnTip && m_actinWasOnTip)
    {
        m_timeLastTrackingCompletion+=m_calcTimeStep;
        m_totalTimeOnTip+=m_calcTimeStep;
    }
    else if(actinIsOnTip && !m_actinWasOnTip)
    {
        m_timeLastTrackingCompletion = m_currentTime;
        m_totalTimeOnTip+=m_calcTimeStep;
    }
    else if(!actinIsOnTip)
    {
        m_totalTimeBehindTip+=m_calcTimeStep;
    }
    m_actinWasOnTip = actinIsOnTip;
}

void Propagator::moveMicrotubule(SystemState& systemState, RandomGenerator& generator)
{
    std::pair<double,double> exclusiveMovementBorders = systemState.movementBordersSetByFullLinkers();

    // The mean should be proportional to the force (overdamped system)
    // mean change = mobility*force*timeStep
    // units:   [timeStep] = s
    //          [force] = (kT)*micron^(-1)
    //          [mobility] = micron^(2)*(kT)^(-1)*s^(-1)
    // In these units, mobility has the same value as the diffusion constant, which has units micron^(2)*s^(-1), and mobility = D/(kT)

    // First, propose a change due to the deterministic force.
    // To this end, integrate the force over the positions.

    const double totalExtension = systemState.getTotalExtensionLinkers();
    const int32_t numberFullLinkers = systemState.getNFullCrosslinkers();
    // The integrated change is given by the following (exponential minus one) function (see notes).
    // The change c follows the formula dc/dt = -k D (E0 + N c).
    // Here, k is the spring constant (in units of k_B T), D is the diffusion constant (and thus the mobility through the Einstein relation),
    // E0 is the totalExtension at time 0, and N is the number of crosslinkers. The change in extension upon a change c is N*c.
    // which for small time steps reduces to:
    // m_diffusionConstantMicrotubule*systemState.getForce()*m_calcTimeStep
    // use the expm1 function to prevent catastrophic cancellation for very small numbers.
    // Check for the case of no crosslinkers. The numberFullLinkers is implicitly converted to double.
    double deterministicChange = (numberFullLinkers!=0)?
                                 (totalExtension/numberFullLinkers*std::expm1(-numberFullLinkers*m_springConstant*m_diffusionConstantMicrotubule*m_calcTimeStep)):
                                 (0.0);

    if(m_addExternalForce) // This if is not necessary, but it does prevent the multiplications below to be performed when not necessary
    {
        deterministicChange += m_diffusionConstantMicrotubule*systemState.findExternalForce()*m_calcTimeStep;
    }

    // Check if the deterministic change is breaking the boundaries; if so: place the particle just on the proper side of the boundary
    if (deterministicChange<=exclusiveMovementBorders.first)
    {
        deterministicChange = std::nextafter(exclusiveMovementBorders.first, exclusiveMovementBorders.second);
        ++m_nDeterministicBoundaryCrossings;
    }
    else if (deterministicChange>=exclusiveMovementBorders.second)
    {
        deterministicChange = std::nextafter(exclusiveMovementBorders.second, exclusiveMovementBorders.first);
        ++m_nDeterministicBoundaryCrossings;
    }

    #ifdef MYDEBUG
    if (deterministicChange<=exclusiveMovementBorders.first || deterministicChange>=exclusiveMovementBorders.second)
    {
        throw GeneralException("In Propagator::moveMicrotubule(), the deterministic change was outside of the allowed zone.");
    }
    #endif // MYDEBUG

    // update the exclusiveMovementBorders, such that they are now relative to the new position after the deterministic change
    exclusiveMovementBorders.first -= deterministicChange;
    exclusiveMovementBorders.second -= deterministicChange;

    // Use a reflecting boundary condition for the random change. First, calculate the Gaussian change.
    double randomChange = generator.getGaussian(0.0, m_deviationMicrotubule);

    // If the change breaks a barrier, reflect it around that barrier.
    // The while loop is there to check if a double reflection is necessary (this should really never happen, so while acts as an if here)
    while (randomChange<=exclusiveMovementBorders.first || randomChange>=exclusiveMovementBorders.second)
    {
        if(randomChange<=exclusiveMovementBorders.first)
        {
            randomChange = 2*exclusiveMovementBorders.first-randomChange;
        }
        if(randomChange>=exclusiveMovementBorders.second)
        {
            randomChange = 2*exclusiveMovementBorders.second-randomChange;
        }

        ++m_nStochasticBoundaryCrossings;
    }

    systemState.updateMobilePosition(deterministicChange+randomChange);
    systemState.updateForceAndEnergy();
}

void Propagator::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    getReactionToHappen(generator).performReaction(systemState, generator);
    resetAction();
    setNewReactionRateThreshold(generator.getProbability());
    systemState.updateForceAndEnergy();
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



