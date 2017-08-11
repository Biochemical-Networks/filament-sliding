#ifndef MICROTUBULE_HPP
#define MICROTUBULE_HPP
#include <cstdint>
#include <vector>
#include "Input/Input.hpp"
#include "Site.hpp"

class Microtubule
{
private:
    const double m_length;
    const double m_latticeSpacing;
    const int32_t m_nSites;

    // Choose to focus on free instead of occupied sites, since functions may ask if the site is free, not if it is occupied (otherwise completely equivalent of course)
    int32_t m_nFreeSites;
    std::vector<Site> m_sites;

public:

    Microtubule(const double length, const double latticeSpacing);
    virtual ~Microtubule();

};

#endif // MICROTUBULE_HPP
