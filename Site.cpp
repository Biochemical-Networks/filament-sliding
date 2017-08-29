#include "Site.hpp"
#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>

Site::Site(const bool isFree)
    :   m_isFree(isFree),
        mp_connectedCrosslinker(nullptr)

    /*,
        m_leftFreeSite(nullptr),
        m_rightFreeSite(nullptr),
        m_leftOccupiedNeighbour(nullptr),
        m_rightOccupiedNeighbour(nullptr),
        m_leftFullyConnectedCrosslinker(nullptr),
        m_rightFullyConnectedCrosslinker(nullptr),
        m_leftOppositeSite(nullptr),
        m_rightOppositeSite(nullptr)*/
{
}

Site::~Site()
{
}

void Site::connectCrosslinker(Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect)
{
    if (!m_isFree)
    {
        throw GeneralException("Site::connectCrosslinker tried to connect a crosslinker that was already connected");
    }

    mp_connectedCrosslinker = &crosslinkerToConnect; // Store the address of the connected crosslinker
    m_connectedTerminus = terminusToConnect;

    m_isFree = false;
}

void Site::disconnectCrosslinker()
{
    if (m_isFree)
    {
        throw GeneralException("Site::disconnectCrosslinker tried to disconnect a crosslinker that was not connected");
    }

    mp_connectedCrosslinker = nullptr;

    m_isFree = true;
}

/*void Site::setLeftFreeSite(const Site* p_leftNeighbour)
{
    mp_leftFreeSite = p_leftNeighbour;
}
void setRightFreeSite(const Site* p_rightNeighbour)
{
    mp_rightFreeSite = p_rightNeighbour;
}*/
