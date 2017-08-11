#ifndef EXTREMITY_HPP
#define EXTREMITY_HPP

#include <cstdint>

class Extremity
{
private:
    bool m_connected;

public:
    Extremity(const bool connected);
    ~Extremity();

    bool isConnected();
};

#endif // EXTREMITY_HPP
