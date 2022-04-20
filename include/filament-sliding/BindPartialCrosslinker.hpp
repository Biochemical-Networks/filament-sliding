#ifndef BINDPARTIALCROSSLINKER_HPP
#define BINDPARTIALCROSSLINKER_HPP

#include <cstdint>
#include <vector>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/PossibleFullConnection.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/Reaction.hpp"
#include "filament-sliding/SystemState.hpp"

/* The Reaction that binds a partial linker such that it becomes fully
 * connected. This class needs to decide where the free terminus connects, and
 * only reports the total binding rate to the public.
 */

class BindPartialCrosslinker : public Reaction {
private:
  const double m_rateOneTerminusToOneSite;
  const Crosslinker::Type m_typeToBind;
  // The following gives the factor with which the rate needs to be multiplied
  // when it involves binding the head (tail).
  const double m_headBindingFactor; // Equals 2/(1+exp(-headBindingBiasEnergy))
                                    // = 2*prob_head_binds. m_tailBindingFactor
                                    // = 2-m_headBindingFactor
  const double m_tailBindingFactor;

  PossibleFullConnection whichToConnect(const SystemState &systemState,
                                        RandomGenerator &generator) const;

  std::vector<double>
      m_individualRates; // store one rate for each member of the
                         // CrosslinkerContainer.m_possibleConnections

  const double m_springConstant; // k/(k_B T)

public:
  BindPartialCrosslinker(const double rateOneTerminusToOneSite,
                         const Crosslinker::Type typeToBind,
                         const double headBiasEnergy,
                         const double springConstant);
  ~BindPartialCrosslinker() override;

  void setCurrentRate(const SystemState &systemState) override;

  void performReaction(SystemState &systemState,
                       RandomGenerator &generator) override;
};

#endif // BINDPARTIALCROSSLINKER_HPP
