#ifndef CLOCK_HPP
#define CLOCK_HPP
#include <chrono>

class Clock
{
private:
    // steady_clock (not system_clock), because this cannot be affected by changes of the system time
    std::chrono::steady_clock::time_point m_constructionTime;
public:
    Clock();
    ~Clock();
    double now() const;
};

#endif // CLOCK_HPP
