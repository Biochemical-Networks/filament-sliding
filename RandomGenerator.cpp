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
