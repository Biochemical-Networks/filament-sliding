#ifndef MOBILEMICROTUBULE_HPP
#define MOBILEMICROTUBULE_HPP

#include "Microtubule.hpp"


class MobileMicrotubule : public Microtubule
{
private:
    const double m_diffusionConstant;

    double m_position;
public:
    MobileMicrotubule(const double length, const double latticeSpacing, const double diffusionConstant, const double initialPosition = 0.);
    virtual ~MobileMicrotubule();

    void updatePosition(const double change);

    double getDiffusionConstant();

    double getPosition();
};

#endif // MOBILEMICROTUBULE_HPP
