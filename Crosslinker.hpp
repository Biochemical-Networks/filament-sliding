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

private:
    const Type m_type;

    Extremity m_head;
    Extremity m_tail;

public:

    Crosslinker(const Type crosslinkerType, bool headConnected, bool tailConnected);
    ~Crosslinker();

    bool isConnected() const;

    int32_t getHeadPosition() const;
    int32_t getTailPosition() const;

    Extremity::MicrotubuleType getHeadMicrotubuleType() const;
    Extremity::MicrotubuleType getTailMicrotubuleType() const;

/*    void setHeadPosition(const int32_t sitePosition);
    void setTailPosition(const int32_t sitePosition);*/

};

#endif // CROSSLINKER_HPP
