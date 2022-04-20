#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <cstdint>
#include <random>

// The class is made, such that the seeding, implementation etc. is shielded
// from the user, and random numbers from certain distributions can be asked via
// its methods
class RandomGenerator {
private:
  std::mt19937_64 m_generator;

  std::uniform_real_distribution<double>
      m_distributionProbility; // Create as a member variable, such that it is
                               // only created once

public:
  RandomGenerator(const std::string seedString);
  ~RandomGenerator();

  std::mt19937_64 &
  getBareGenerator(); // Return by reference, otherwise a copy is made, and the
                      // generator is not updated upon use

  // Functions that change the generator state should not be const
  double getGaussian(const double mean, const double deviation);
  bool getBernoulli(const double probability);
  double getProbability();
  double getUniform(const double lowerBound, const double upperBound);
  int32_t getUniformInteger(const int32_t inclusiveLowerBound,
                            const int32_t inclusiveUpperBound);
};

#endif // RANDOMGENERATOR_HPP
