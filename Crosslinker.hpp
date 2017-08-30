#ifndef CROSSLINKER_HPP
#define CROSSLINKER_HPP

#include <cstdint>
#include "Extremity.hpp"


class Crosslinker
{
public:
    enum class Type
    {
        PASSIVE,
        DUAL,
        ACTIVE
    };

    enum class Terminus // Such that other classes can name the extremities
    {
        HEAD,
        TAIL
    };

private:
    const Type m_type;

    Extremity m_head;
    Extremity m_tail;

public:

    Crosslinker(const Type crosslinkerType);
    ~Crosslinker();

    bool isConnected() const;

    int32_t getHeadPosition() const;
    int32_t getTailPosition() const;

    Type getType() const;

    Extremity::MicrotubuleType getHeadMicrotubuleType() const;
    Extremity::MicrotubuleType getTailMicrotubuleType() const;

    void connectFromFree(const Extremity::MicrotubuleType microtubuleToConnectTo, const Terminus terminusToConnect, const int32_t position);

    void disconnectFromPartialConnection();

    void fullyConnectFromPartialConnection(const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position);

    void disconnectFromFullConnection(const Terminus terminusToDisconnect);

    Terminus getFreeTerminusWhenPartiallyConnected() const;

    void getBindingPositionWhenPartiallyConnected(Extremity::MicrotubuleType& microtubuleToDisconnect, int32_t& positionToDisconnectFrom) const;

    void getOneBindingPositionWhenFullyConnected(const Crosslinker::Terminus terminus, Extremity::MicrotubuleType& microtubuleToDisconnect, int32_t& positionToDisconnectFrom) const;

/*    void setHeadPosition(const int32_t sitePosition);
    void setTailPosition(const int32_t sitePosition);*/

};

#endif // CROSSLINKER_HPP
