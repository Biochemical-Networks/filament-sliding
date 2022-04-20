#include <cmath>   // exp
#include <cstddef> // size_t
#include <vector>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/HopPartial.hpp"
#include "filament-sliding/PossibleHop.hpp"
#include "filament-sliding/Reaction.hpp"

HopPartial::HopPartial(const double baseRateHead, const double baseRateTail,
                       const Crosslinker::Type typeToHop,
                       const double headHopToPlusBiasEnergy,
                       const double tailHopToPlusBiasEnergy,
                       const double cooperativeBiasEnergy)
    : Reaction(), m_typeToHop(typeToHop),
      m_headHopToPlusRate(baseRateHead *
                          std::exp(headHopToPlusBiasEnergy * 0.5)),
      m_headHopToMinusRate(baseRateHead *
                           std::exp(-headHopToPlusBiasEnergy * 0.5)),
      m_tailHopToPlusRate(baseRateTail *
                          std::exp(tailHopToPlusBiasEnergy * 0.5)),
      m_tailHopToMinusRate(baseRateTail *
                           std::exp(-tailHopToPlusBiasEnergy * 0.5)),
      m_cooperativeRateFactorBias(std::exp(-cooperativeBiasEnergy)) {}

HopPartial::~HopPartial() {}

void HopPartial::setCurrentRate(const SystemState &systemState) {
  const std::vector<PossiblePartialHop> &possiblePartialHops =
      systemState.getPossiblePartialHops(m_typeToHop);

  m_individualRates.clear();
  m_individualRates.reserve(possiblePartialHops.size());

  double sum = 0;
  for (const PossiblePartialHop &possiblePartialHop : possiblePartialHops) {
    const double rate = getRateToHop(possiblePartialHop.terminusToHop,
                                     possiblePartialHop.direction,
                                     possiblePartialHop.awayFromNeighbour);
    m_individualRates.push_back(rate);
    sum += rate;
  }
  m_currentRate = sum;
}

double HopPartial::getRateToHop(const Crosslinker::Terminus terminusToHop,
                                const HopDirection directionToHop,
                                bool awayFromNeighbour) const {
  const double biasFactor = awayFromNeighbour ? m_cooperativeRateFactorBias : 1;
  if (terminusToHop == Crosslinker::Terminus::HEAD &&
      directionToHop == HopDirection::FORWARD) {
    return m_headHopToPlusRate * biasFactor;
  } else if (terminusToHop == Crosslinker::Terminus::HEAD &&
             directionToHop == HopDirection::BACKWARD) {
    return m_headHopToMinusRate * biasFactor;
  } else if (terminusToHop == Crosslinker::Terminus::TAIL &&
             directionToHop == HopDirection::FORWARD) {
    return m_tailHopToPlusRate * biasFactor;
  } else if (terminusToHop == Crosslinker::Terminus::TAIL &&
             directionToHop == HopDirection::BACKWARD) {
    return m_tailHopToMinusRate * biasFactor;
  } else {
    throw GeneralException(
        "HopPartial::getRateToHop() was called with improper parameters.");
  }
}

PossiblePartialHop HopPartial::whichHop(const SystemState &systemState,
                                        RandomGenerator &generator) const {
  const std::vector<PossiblePartialHop> &possiblePartialHops =
      systemState.getPossiblePartialHops(m_typeToHop);

#ifdef MYDEBUG
  if (possiblePartialHops.empty()) {
    throw GeneralException("HopPartial::whichHop() did not have possibilities");
  }
  if (possiblePartialHops.size() != m_individualRates.size()) {
    throw GeneralException(
        "HopPartial::whichHop() was called with an outdated vector");
  }
#endif // MYDEBUG

  // Choose the connection with a probability proportional to its rate
  const double eventIdentifyingRate = generator.getUniform(0, m_currentRate);
  double sum = 0;
  for (std::size_t label = 0; label < m_individualRates.size();
       ++label) // We need to find a location, so don't use a range based loop
  {
    sum += m_individualRates.at(label);
    if (sum > eventIdentifyingRate) {
      return possiblePartialHops.at(label);
    }
  }
  throw GeneralException("The end of HopPartial::whichHop() was reached");
}

void HopPartial::performReaction(SystemState &systemState,
                                 RandomGenerator &generator) {
  PossiblePartialHop hopToMake = whichHop(systemState, generator);

  // Implement the hop as a combination of binding and unbinding
  systemState.disconnectPartiallyConnectedCrosslinker(
      *hopToMake.p_partialLinker);
  systemState.connectFreeCrosslinker(m_typeToHop, hopToMake.terminusToHop,
                                     hopToMake.locationToHopTo);
}
