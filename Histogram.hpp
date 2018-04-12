#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <cstdint>
#include <vector>

/* Histogram bins an interval on the real line into bins of size binSize, and can count how often a value was passed to it that fell in a certain bin.
 * The final bin always contains highestValue.
 * The bins are intervals of the type [a,b), such that, when highestValue = lowestValue + k*binWidth, where k is an integer, then there is a separate bin for highestValue.
 * All values lower than lowestValue and higher than the bin containing highestValue are stored in separate bins on the edges.
 */

class Histogram
{
private:
    const double m_binSize;
    const double m_lowestValue;
    const double m_highestValue;

    const int32_t m_numberOfBins; // includes the two outermost bins, which represent values that lie outside of the bins representing values from lowestValue to highestValue
    std::vector<int64_t> m_bins;
public:
    Histogram(const double binSize, const double lowestValue, const double highestValue);
    ~Histogram();
};

#endif // HISTOGRAM_HPP
