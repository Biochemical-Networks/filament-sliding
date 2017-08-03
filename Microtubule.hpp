#ifndef MICROTUBULE_HPP
#define MICROTUBULE_HPP
#include <cstdint>
#include "Input/Input.hpp"

class Microtubule
{
private:
    const double m_length;
    const double m_latticeSpacing;
    const int32_t m_nSites;

public:
    Microtubule(const double length, const double latticeSpacing);
    virtual ~Microtubule();

};

#endif // MICROTUBULE_HPP
