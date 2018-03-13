#ifndef MOBILEMICROTUBULE_HPP
#define MOBILEMICROTUBULE_HPP

#include "Microtubule.hpp"

/* A derivative of Microtubule, this class adds extra functionality related to movement of the mobile microtubule.
 * This movement can be seen as relative to the fixed microtubule.
 */

class MobileMicrotubule : public Microtubule
{
private:
    double m_position;
public:
    MobileMicrotubule(const double length, const double latticeSpacing, const double initialPosition = 0.);
    virtual ~MobileMicrotubule();

    void setPosition(const double initialPosition);

    void updatePosition(const double change);

    double getPosition() const;
};

#endif // MOBILEMICROTUBULE_HPP
