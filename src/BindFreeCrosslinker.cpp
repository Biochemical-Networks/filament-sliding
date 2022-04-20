#include <cmath>
#include <cstdint>

#include "filament-sliding/BindFreeCrosslinker.hpp"
#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/SystemState.hpp"

BindFreeCrosslinker::BindFreeCrosslinker(const double rateToOneSite,
                                         const Crosslinker::Type typeToBind,
                                         const double headBiasEnergy)
    : Reaction(), m_rateToOneSite(rateToOneSite), m_typeToBind(typeToBind),
      m_probHeadBinds(
          1 /
          (1 +
           std::exp(
               -headBiasEnergy))) // headBiasEnergy should have units of (k_B T)
{}

BindFreeCrosslinker::~BindFreeCrosslinker() {}

void BindFreeCrosslinker::setCurrentRate(const SystemState &systemState) {
  m_currentRate = m_rateToOneSite * systemState.getNFreeSites();
}

SiteLocation
BindFreeCrosslinker::whereToConnect(const SystemState &systemState,
                                    RandomGenerator &generator) const {
  int32_t nFreeSitesFixed = systemState.getNFreeSitesFixed();
  int32_t nFreeSitesMobile = systemState.getNFreeSitesMobile();
  int32_t nFreeSites = nFreeSitesFixed + nFreeSitesMobile;

  // Initialise these as if the fixed microtubule needs to be connected, and
  // change it after if the opposite needs to happen
  MicrotubuleType microtubuleToConnect = MicrotubuleType::FIXED;
  int32_t freeSiteLabelToConnect = generator.getUniformInteger(
      0, nFreeSites - 1); // Each site has equal probability of binding
  if (freeSiteLabelToConnect >= nFreeSitesFixed) {
    microtubuleToConnect = MicrotubuleType::MOBILE;
    freeSiteLabelToConnect -=
        nFreeSitesFixed; // Now it labels a site on the mobile microtubule.
  }

  // Now, we have picked a label to a free site uniformly. Then, map this using
  // a linear (bijective) function to the actual positions on the microtubule
  // where the free sites are
  int32_t positionToConnectAt = systemState.getFreeSitePosition(
      microtubuleToConnect, freeSiteLabelToConnect);

  return SiteLocation{microtubuleToConnect, positionToConnectAt};
}

void BindFreeCrosslinker::performReaction(SystemState &systemState,
                                          RandomGenerator &generator) {
  if (systemState.getNFreeCrosslinkersOfType(m_typeToBind) <= 0) {
    throw GeneralException("BindFreeCrosslinker::performReaction() was called, "
                           "but no cross-linkers are available any more");
  }

  SiteLocation connectLocation = whereToConnect(systemState, generator);

  // Binding can be biased towards the head or tail
  Crosslinker::Terminus terminusToConnect =
      ((generator.getBernoulli(m_probHeadBinds))
           ? (Crosslinker::Terminus::HEAD)
           : (Crosslinker::Terminus::TAIL));

  systemState.connectFreeCrosslinker(m_typeToBind, terminusToConnect,
                                     connectLocation);
}
