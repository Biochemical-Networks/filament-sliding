#ifndef MOBILEMICROTUBULE_HPP
#define MOBILEMICROTUBULE_HPP

#include "filament-sliding/Microtubule.hpp"

/* A derivative of Microtubule, this class adds extra functionality related to
 * movement of the mobile microtubule. This movement can be seen as relative to
 * the fixed microtubule.
 */

class MobileMicrotubule : public Microtubule {
private:
  double m_position;

  // An integer giving the position of the free energy valley where the mobile
  // microtubule is currently at, in units of the lattice spacing Typically, the
  // microtubule will fluctuate around a point where all springs (crosslinkers)
  // can be relaxed. This is where the two microtubules exactly lie on top of
  // each other. Say that the microtubule has crossed the free energy barrier
  // when the position is exactly one delta further than the old
  // m_currentAttractorPosition. In that case, the microtubule has passed the
  // barrier and even half of the valley:
  int32_t m_currentAttractorPosition;

public:
  MobileMicrotubule(const double length, const double latticeSpacing,
                    const double initialPosition = 0.);
  // Default value such that it is possible to create a MobileMicrotubule
  // without setting the initial position explicitly.
  virtual ~MobileMicrotubule();

  void setPosition(const double initialPosition);

  void updatePosition(const double change);

  double getPosition() const;

  int32_t
  barrierCrossed(); // also updates m_currentAttractorPosition if necessary.
                    // Returns the direction, 0 if no step is taken
};

#endif // MOBILEMICROTUBULE_HPP
