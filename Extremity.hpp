#ifndef EXTREMITY_HPP
#define EXTREMITY_HPP

#include <cstdint>

class Extremity
{
// The following part needs to be defined earlier than the private m_connectedTo
public:
    enum class MicrotubuleType
    {
        FIXED,
        MOBILE
    };

private:
    bool m_connected;
    MicrotubuleType m_connectedTo;
    int32_t m_sitePosition;


public:

    Extremity(const bool connected);
    ~Extremity();

    bool isConnected() const;

    int32_t getPosition() const;

    MicrotubuleType getMicrotubuleType() const;

    void connect(const MicrotubuleType connectTo, const int32_t sitePosition);

    void disconnect();

    void changePosition(const MicrotubuleType connectedTo, const int32_t sitePosition);
};

#endif // EXTREMITY_HPP
