#ifndef MOBILEMICROTUBULE_HPP
#define MOBILEMICROTUBULE_HPP

#include "Microtubule.hpp"


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
