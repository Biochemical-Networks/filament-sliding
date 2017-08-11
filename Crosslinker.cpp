#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Extremity.hpp"

Crosslinker::Crosslinker(bool headConnected, bool tailConnected)
    :   m_head(headConnected),
        m_tail(tailConnected)
{
}

Crosslinker::~Crosslinker()
{
}

bool Crosslinker::isConnected()
{
    return (m_head.isConnected()||m_tail.isConnected());
}

/*
bool Crosslinker::isConnected(const Crosslinker::Extremity& side)
{
    switch (side)
    {
        case Crosslinker::Extremity::HEAD :
            return m_headConnected;
        case Crosslinker::Extremity::TAIL :
            return m_tailConnected;
        default:
            throw GeneralException("An extremity was passed to Crosslinker::isConnected() that does not hold an appropriate value");
    }
}*/
