#include <cstdint>
#include <vector>
#include <iostream>

class TransitionPath
{
private:
    struct SystemCoordinate
    {
        const double m_time;
        const double m_mobilePosition;
        const int32_t m_nRightPullingCrosslinkers;
    };

    std::vector<SystemCoordinate> m_pathVector;

public:
    TransitionPath();
    ~TransitionPath();

    void addPoint(const double time, const double mobilePosition, const int32_t nRightPullingCrosslinkers);

    void clean();

    friend std::ostream& operator<< (std::ostream &out, const TransitionPath &transitionPath);
};

std::ostream& operator<< (std::ostream &out, const TransitionPath &transitionPath);
