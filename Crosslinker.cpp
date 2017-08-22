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

Crosslinker::Type Crosslinker::getType() const
{
    return m_type;
}

Extremity::MicrotubuleType Crosslinker::getHeadMicrotubuleType() const
{
    return m_head.getMicrotubuleType();
}

Extremity::MicrotubuleType Crosslinker::getTailMicrotubuleType() const
{
    return m_tail.getMicrotubuleType();
}

void Crosslinker::connectFromFree(const Extremity::MicrotubuleType microtubuleToConnectTo, const Terminus terminusToConnect, const int32_t position)
{
    switch(terminusToConnect)
    {
        case Crosslinker::Terminus::HEAD:
            m_head.connect(microtubuleToConnectTo, position);
            break;
        case Crosslinker::Terminus::TAIL:
            m_tail.connect(microtubuleToConnectTo, position);
            break;
        default:
            throw GeneralException("An incorrect crosslinker terminus was passed to Crosslinker::connectFromFree()");
            break;
    }
}



