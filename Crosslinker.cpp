#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Extremity.hpp"

Crosslinker::Crosslinker(const Type type, bool headConnected, bool tailConnected)
    :   m_type(type),
        m_head(headConnected),
        m_tail(tailConnected)
{
}

Crosslinker::~Crosslinker()
{
}

bool Crosslinker::isConnected() const
{
    return (m_head.isConnected()||m_tail.isConnected());
}

int32_t Crosslinker::getHeadPosition() const
{
    return m_head.getPosition();
}

int32_t Crosslinker::getTailPosition() const
{
    return m_tail.getPosition();
}

Extremity::MicrotubuleType Crosslinker::getHeadMicrotubuleType() const
{
    return m_head.getMicrotubuleType();
}

Extremity::MicrotubuleType Crosslinker::getTailMicrotubuleType() const
{
    return m_tail.getMicrotubuleType();
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
