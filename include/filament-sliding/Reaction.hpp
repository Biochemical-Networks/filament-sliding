#ifndef REACTION_HPP
#define REACTION_HPP

#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/SystemState.hpp"

/* Reactions set the rules by which the SystemState is changed.
 * It is a virtual class that contains the basic methods and members required
 * for a reaction, and other reactions are inherited from it. This is done such
 * that all reactions can be treated on equal footing when choosing which
 * reaction needs to happen. To change the SystemState, a Reaction uses methods
 * of SystemState, and Reactions are called by the Propagator, such that direct
 * manipulation of the SystemState is shielded away from the Propagator.
 */

class Reaction {
protected: // Protected, since the inherited classes need access to these.
  // Do not include an elementary rate, since there may be several (this is the
  // case for HopFull)
  double m_currentRate;

  double
      m_action; // The summation of the rates at each time step, used for
                // integrating the rate over time (time steps are assumed fixed)
public:
  Reaction();
  virtual ~Reaction(); // Don't allow Reaction pointers to destroy derived
                       // objects only using the Reaction destructor

  void resetAction();

  double getAction() const; // does not have time step duration included

  double getCurrentRate() const;

  void updateAction();
  // Action is updated by adding the current rate times the time step size to
  // it. Once the total action of all reactions reaches a (randomly set) plateau
  // value, one fires

  // Pure virtual functions, these have to be present in a reaction, but need to
  // be implemented in the derived classes
  virtual void setCurrentRate(const SystemState &systemState) = 0;

  virtual void performReaction(SystemState &systemState,
                               RandomGenerator &generator) = 0;
};

#endif // REACTION_HPP
