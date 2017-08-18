#include "Microtubule.hpp"
#include <cstdint>
#include <cmath>
#include <iostream>
#include <vector>
#include "Site.hpp"
#include "Crosslinker.hpp"

Microtubule::Microtubule(const double length, const double latticeSpacing)
    :   m_length(length),
        m_latticeSpacing(latticeSpacing),
        m_nSites(static_cast<int32_t>(std::floor(m_length/m_latticeSpacing))),
        m_nFreeSites(m_nSites),
        m_sites(m_nSites, Site(true))
{
}

Microtubule::~Microtubule()
{
}

void Microtubule::connectSite(const int32_t sitePosition, Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect)
{
    m_sites.at(sitePosition).connectCrosslinker(crosslinkerToConnect, terminusToConnect);
}
