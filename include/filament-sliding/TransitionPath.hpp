#ifndef TRANSITIONPATH_HPP
#define TRANSITIONPATH_HPP

#include <cstdint>
#include <iostream>
#include <vector>

class TransitionPath {
  private:
    struct SystemCoordinate {
        const double m_time;
        const double m_mobilePosition;
        const int32_t m_nRightPullingCrosslinkers;
    };

    const int32_t m_writeFrequency;

    std::vector<SystemCoordinate> m_pathVector;

  public:
    TransitionPath(const int32_t writeFrequency);
    ~TransitionPath();

    void addPoint(
            const double time,
            const double mobilePosition,
            const int32_t nRightPullingCrosslinkers);

    void clean();

    int32_t getSize() const;

    double getMobilePosition(const int32_t label) const;

    int32_t getNRightPullingLinkers(const int32_t label) const;

    friend std::ostream& operator<<(
            std::ostream& out,
            const TransitionPath& transitionPath);
};

std::ostream& operator<<(
        std::ostream& out,
        const TransitionPath& transitionPath);

#endif // TRANSITIONPATH_HPP
