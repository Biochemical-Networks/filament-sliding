#ifndef PROPAGATOR_HPP
#define PROPAGATOR_HPP

#include <random>
#include <cstdint>
#include <string>

class Propagator
{
private:
    std::mt19937_64 m_generator;
    const int32_t m_nTimeSteps;

public:
    Propagator(const std::string seedString, const int32_t nTimeSteps);
    ~Propagator();



};

#endif // PROPAGATOR_HPP
