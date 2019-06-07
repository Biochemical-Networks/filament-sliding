#include "RandomGenerator.hpp"

#include <random>

RandomGenerator::RandomGenerator(const std::string seedString)
    :   m_distributionProbility(std::nextafter(0.0, std::numeric_limits<double>::max()), 1.0) // exclusive interval (0,1)
{
    std::seed_seq seed(seedString.begin(), seedString.end());
    m_generator.seed(seed);
}

RandomGenerator::~RandomGenerator()
{
}

std::mt19937_64& RandomGenerator::getBareGenerator()
{
    return m_generator;
}

double RandomGenerator::getGaussian(const double mean, const double deviation)
{
    std::normal_distribution<double> distribution(mean, deviation);
    return distribution(m_generator);
}

bool RandomGenerator::getBernoulli(const double probability)
{
    std::bernoulli_distribution distribution(probability);
    return distribution(m_generator);
}

double RandomGenerator::getProbability()
{
    return m_distributionProbility(m_generator);
}

double RandomGenerator::getUniform(const double lowerBound, const double upperBound)
{
    std::uniform_real_distribution<double> distribution(lowerBound, upperBound); // uses interval [a,b)
    return distribution(m_generator);
}

int32_t RandomGenerator::getUniformInteger(const int32_t inclusiveLowerBound, const int32_t inclusiveUpperBound)
{
    std::uniform_int_distribution<int32_t> distribution(inclusiveLowerBound, inclusiveUpperBound);
    return distribution(m_generator);
}

