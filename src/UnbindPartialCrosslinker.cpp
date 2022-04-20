#include <utility>
#include <vector>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/Reaction.hpp"
#include "filament-sliding/SystemState.hpp"
#include "filament-sliding/UnbindPartialCrosslinker.hpp"

UnbindPartialCrosslinker::UnbindPartialCrosslinker(
    const double rateOneTerminusDisconnects,
    const Crosslinker::Type typeToUnbind, const double headBiasEnergy)
    : Reaction(), m_rateOneTerminusDisconnects(rateOneTerminusDisconnects),
      m_typeToUnbind(typeToUnbind),
      m_headUnbindingFactor(2 / (1 + std::exp(headBiasEnergy))),
      m_tailUnbindingFactor(2 - m_headUnbindingFactor) {}

UnbindPartialCrosslinker::~UnbindPartialCrosslinker() {}

void UnbindPartialCrosslinker::setCurrentRate(const SystemState &systemState) {
  const std::pair<int32_t, int32_t> nPartialsBoundWithHeadAndTail =
      systemState.getNPartialCrosslinkersBoundWithHeadAndTailOfType(
          m_typeToUnbind);

#ifdef MYDEBUG
  int32_t nCrosslinkersOfThisType =
      systemState.getNPartialCrosslinkersOfType(m_typeToUnbind);
  if (nCrosslinkersOfThisType != nPartialsBoundWithHeadAndTail.first +
                                     nPartialsBoundWithHeadAndTail.second) {
    throw GeneralException("UnbindPartialCrosslinker::setCurrentRate() found "
                           "two different numbers of partial linkers");
  }
#endif // MYDEBUG

  m_currentRate =
      m_rateOneTerminusDisconnects *
      (nPartialsBoundWithHeadAndTail.first * m_headUnbindingFactor +
       nPartialsBoundWithHeadAndTail.second * m_tailUnbindingFactor);
}

void UnbindPartialCrosslinker::performReaction(SystemState &systemState,
                                               RandomGenerator &generator) {
  Crosslinker &linkerToDisconnect = whichToDisconnect(systemState, generator);
  systemState.disconnectPartiallyConnectedCrosslinker(linkerToDisconnect);
}

Crosslinker &
UnbindPartialCrosslinker::whichToDisconnect(SystemState &systemState,
                                            RandomGenerator &generator) const {
  const std::vector<Crosslinker *> &partialsBoundWithHead =
      systemState.getPartialLinkersBoundWithHead(m_typeToUnbind);
  const std::vector<Crosslinker *> &partialsBoundWithTail =
      systemState.getPartialLinkersBoundWithTail(m_typeToUnbind);

#ifdef MYDEBUG
  if (partialsBoundWithHead.empty() && partialsBoundWithTail.empty()) {
    throw GeneralException("No partial linker is available to be disconnected "
                           "in UnbindPartialCrosslinker::whichToDisconnect()");
  }
#endif // MYDEBUG

  const double probHeadUnbinds = m_rateOneTerminusDisconnects *
                                 partialsBoundWithHead.size() *
                                 m_headUnbindingFactor / m_currentRate;

  // Choose from which of the two sets (bound with head or tail) the crosslinker
  // needs to be picked.
  const std::vector<Crosslinker *> &whichSet =
      (generator.getBernoulli(probHeadUnbinds)) ? partialsBoundWithHead
                                                : partialsBoundWithTail;

  // Then, generate the position in the set of those linkers uniformly. This
  // way, two random numbers are used, but it does prevent a loop through all
  // separate possible unbinding events.
  const int32_t label = generator.getUniformInteger(0, whichSet.size() - 1);
  return *whichSet.at(label);
}
