#ifndef CROSSLINKER_HPP
#define CROSSLINKER_HPP

#include "Extremity.hpp"

class Crosslinker
{
private:
    Extremity m_head;
    Extremity m_tail;

public:

    Crosslinker(bool headConnected, bool tailConnected);
    ~Crosslinker();

    bool isConnected();
    /*bool isConnected(const Extremity& side);*/



};

#endif // CROSSLINKER_HPP
