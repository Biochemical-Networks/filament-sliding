#include "RandomGenerator.hpp"

#include <random>

RandomGenerator::RandomGenerator(const std::string seedString)
{
    std::seed_seq seed(seedString.begin(), seedString.end());
    m_generator.seed(seed);
}

RandomGenerator::~RandomGenerator()
{
}

const double RandomGenerator::getGaussian(const double mean, const double deviation)
{
    std::normal_distribution<double> distribution(mean, deviation);
    return distribution(m_generator);
}
