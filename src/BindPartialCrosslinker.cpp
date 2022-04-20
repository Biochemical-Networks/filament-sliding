#include <cmath>   // exp
#include <cstddef> // size_t
#include <cstdint>
#include <vector>

#include "filament-sliding/BindPartialCrosslinker.hpp"
#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/CrosslinkerContainer.hpp"
#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/PossibleFullConnection.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/SystemState.hpp"

BindPartialCrosslinker::BindPartialCrosslinker(
    const double rateOneTerminusToOneSite, const Crosslinker::Type typeToBind,
    const double headBiasEnergy, const double springConstant)
    : Reaction(), m_rateOneTerminusToOneSite(rateOneTerminusToOneSite),
      m_typeToBind(typeToBind),
      m_headBindingFactor(2 / (1 + std::exp(-headBiasEnergy))),
      m_tailBindingFactor(2 - m_headBindingFactor),
      m_springConstant(springConstant) {}

BindPartialCrosslinker::~BindPartialCrosslinker() {}

// Energy dependent rate
void BindPartialCrosslinker::setCurrentRate(const SystemState &systemState) {
  const std::vector<PossibleFullConnection> &possibleConnections =
      systemState.getPossibleConnections(m_typeToBind);
  m_individualRates.clear();
  m_individualRates.reserve(possibleConnections.size());

  double sum = 0;
  for (const PossibleFullConnection &possibleConnection : possibleConnections) {
    // spread the effect of extension evenly over connecting and disconnecting:
    // rate scales with exp(-k x^2 / (4 k_B T))
    double rate = m_rateOneTerminusToOneSite *
                  std::exp(-m_springConstant * possibleConnection.extension *
                           possibleConnection.extension * 0.25);
    rate *= (possibleConnection.p_partialLinker
                 ->getFreeTerminusWhenPartiallyConnected() ==
             Crosslinker::Terminus::HEAD)
                ? m_headBindingFactor
                : m_tailBindingFactor;
    m_individualRates.push_back(rate);
    sum += rate;
  }
  m_currentRate = sum;
}

// This function uses the current (individual) rates, make sure they are
// updated!
PossibleFullConnection
BindPartialCrosslinker::whichToConnect(const SystemState &systemState,
                                       RandomGenerator &generator) const {
  const std::vector<PossibleFullConnection> &possibleConnections =
      systemState.getPossibleConnections(m_typeToBind);

#ifdef MYDEBUG
  if (possibleConnections.empty()) {
    throw GeneralException("BindPartialCrosslinker::whichToConnect() was not "
                           "able to disconnect a linker");
  }
  if (possibleConnections.size() != m_individualRates.size()) {
    throw GeneralException("BindPartialCrosslinker::whichToConnect() was "
                           "called with an outdated vector");
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
      return possibleConnections.at(label);
    }
  }
  throw GeneralException(
      "The end of BindPartialCrosslinker::whichToConnect() was reached");
}

void BindPartialCrosslinker::performReaction(SystemState &systemState,
                                             RandomGenerator &generator) {
  PossibleFullConnection connectionToMake =
      whichToConnect(systemState, generator);

  systemState.connectPartiallyConnectedCrosslinker(
      *(connectionToMake.p_partialLinker), connectionToMake.location);
}
