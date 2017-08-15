#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <random>

// The class is made, such that the seeding, implementation etc. is shielded from the user, and random numbers from certain distributions can be asked via its methods
class RandomGenerator
{
private:
    std::mt19937_64 m_generator;


public:
    RandomGenerator(const std::string seedString);
    ~RandomGenerator();

    // Functions that change the generator state should not be const
    const double getGaussian(const double mean, const double deviation);
};

#endif // RANDOMGENERATOR_HPP
