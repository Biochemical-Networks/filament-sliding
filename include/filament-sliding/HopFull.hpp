#ifndef HOPFULL_HPP
#define HOPFULL_HPP

#include "filament-sliding/Reaction.hpp"

/* The Reaction that hops one of the connected termini of a fully connected
 * linker to either side. The head and tail can have a bias to hop in a specific
 * direction on the polarised microtubule. This class needs to decide which of
 * all possible hops is performed, and only reports the total hopping rate of
 * all full linkers of this type to the public.
 */

class HopFull : public Reaction {
private:
  const Crosslinker::Type m_typeToHop;

  const double m_springConstant; // k/(k_B T)

  // For vanishing spring constant, the rates of hopping towards the plus or
  // minus tip obey r+/r- = exp(hopToPlusBiasEnergy), with the choice r+ =
  // elementaryRate*exp(hopToPlusBias/2) and r- =
  // elementaryRate*exp(-hopToPlusBias/2)
  const double m_headHopToPlusBaseRate;
  const double m_headHopToMinusBaseRate;
  const double m_tailHopToPlusBaseRate;
  const double m_tailHopToMinusBaseRate;

  const double m_cooperativeRateFactorBias;

  std::vector<double>
      m_individualRates; // store one rate for each member of the
                         // CrosslinkerContainer.m_possibleFullHops

  double getBaseRateToHop(const Crosslinker::Terminus terminusToHop,
                          const HopDirection directionToHop,
                          const bool awayFromNeighbour) const;

  const PossibleFullHop &whichHop(const SystemState &systemState,
                                  RandomGenerator &generator) const;

public:
  HopFull(const double baseRateHead, const double baseRateTail,
          const Crosslinker::Type typeToHop, const double springConstant,
          const double headHopToPlusBiasEnergy,
          const double tailHopToPlusBiasEnergy,
          const double cooperativeBiasEnergy);
  ~HopFull() override;

  void setCurrentRate(const SystemState &systemState) override;

  void performReaction(SystemState &systemState,
                       RandomGenerator &generator) override;
};

#endif // HOPFULL_HPP
