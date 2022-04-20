#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "filament-sliding/Statistics.hpp"

/* Histogram bins an interval on the real line into bins of size binSize, and
 * can count how often a value was passed to it that fell in a certain bin. The
 * final bin always contains highestValue. The bins are intervals of the type
 * [a,b), such that, when highestValue = lowestValue + k*binWidth, where k is an
 * integer, then there is a separate bin for highestValue. All values lower than
 * lowestValue and higher than the bin containing highestValue are stored in
 * separate bins on the edges. Always keep track of the statistics (mean and
 * variance) of the data, since these contain information about the distribution
 * as well, but should not be quantified from the histogram.
 */

class Histogram: public Statistics {
  private:
    const double m_binSize;
    const double m_lowestValue;
    const int32_t
            m_numberOfBins; // excludes the two outermost bins, which represent
                            // values that lie outside of the bins representing
                            // values from lowestValue to highestValue
    const double m_highestValue; // holds the true highest value, usually not
                                 // the one that was passed to the constructor
    std::vector<int64_t> m_bins; // The number of samples can be large,
                                 // therefore use 64 bit integers

    std::pair<double, double> calculateBinBounds(const int32_t binNumber) const;

  public:
    Histogram(
            const double binSize,
            const double lowestValue,
            const double highestValue);
    ~Histogram() override;

    void addValue(
            const double value); // redefine behaviour of Statistics::addValue

    friend std::ostream& operator<<(
            std::ostream& out,
            const Histogram& histogram);
};

std::ostream& operator<<(std::ostream& out, const Histogram& histogram);

#endif // HISTOGRAM_HPP
