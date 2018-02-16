#ifndef CROSSLINKER_HPP
#define CROSSLINKER_HPP

#include <cstdint>
#include "Extremity.hpp"
#include "MicrotubuleType.hpp"


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

    bool isFree() const;

    bool isPartial() const;

    bool isFull() const;

    Type getType() const;

/*    int32_t getHeadPosition() const;
    int32_t getTailPosition() const;

    MicrotubuleType getHeadMicrotubuleType() const;
    MicrotubuleType getTailMicrotubuleType() const;*/

    SiteLocation getSiteLocationOf(const Terminus terminus) const;

    void connectFromFree(const Terminus terminusToConnect, const SiteLocation connectAt);

    void disconnectFromPartialConnection();

    void fullyConnectFromPartialConnection(const SiteLocation connectAt);

    void disconnectFromFullConnection(const Terminus terminusToDisconnect);

    Terminus getFreeTerminusWhenPartiallyConnected() const;

    SiteLocation getBoundLocationWhenPartiallyConnected() const;

    SiteLocation getOneBoundLocationWhenFullyConnected(const Crosslinker::Terminus terminus) const;

/*    void setHeadPosition(const int32_t sitePosition);
    void setTailPosition(const int32_t sitePosition);*/

};

#endif // CROSSLINKER_HPP
