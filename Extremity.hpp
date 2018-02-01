#ifndef EXTREMITY_HPP
#define EXTREMITY_HPP

#include <cstdint>
#include "MicrotubuleType.hpp"

class Extremity
{
private:
    bool m_connected;
    MicrotubuleType m_connectedTo;
    int32_t m_sitePosition;

public:
    Extremity(const bool connected);
    ~Extremity();

    bool isConnected() const;

    // CAN THE TWO FOLLOWING FUNCTIONS BE REPLACED WITH THE THIRD?
    /*int32_t getPosition() const;

    MicrotubuleType getMicrotubuleType() const;*/

    SiteLocation getSiteLocation() const;

    void connect(const SiteLocation siteToConnectTo);

    void disconnect();

    void changePosition(const SiteLocation siteToConnectTo);
};

#endif // EXTREMITY_HPP
