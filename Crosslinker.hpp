#ifndef CROSSLINKER_HPP
#define CROSSLINKER_HPP

#include <cstdint>
#include "Extremity.hpp"

class Crosslinker
{
private:
    Extremity m_head;
    Extremity m_tail;

public:

    Crosslinker(bool headConnected, bool tailConnected);
    ~Crosslinker();

    bool isConnected() const;

    int32_t getHeadPosition() const;
    int32_t getTailPosition() const;

/*    void setHeadPosition(const int32_t sitePosition);
    void setTailPosition(const int32_t sitePosition);*/


    /*bool isConnected(const Extremity& side);*/



};

#endif // CROSSLINKER_HPP
