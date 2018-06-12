#include "TransitionPath.hpp"
#include "OutputParameters.hpp"

#include <vector>
#include <cstdint>
#include <iomanip>

TransitionPath::TransitionPath()
{
}

TransitionPath::~TransitionPath()
{
}

void TransitionPath::addPoint(const double time, const double mobilePosition, const int32_t nRightPullingCrosslinkers)
{
    m_pathVector.push_back(SystemCoordinate{time, mobilePosition, nRightPullingCrosslinkers});
}

void TransitionPath::clean()
{
    m_pathVector.clear();
}

std::ostream& operator<< (std::ostream &out, const TransitionPath &transitionPath)
{
    for(const TransitionPath::SystemCoordinate& systemCoordinate : transitionPath.m_pathVector)
    {
        out << std::setw(OutputParameters::collumnWidth) << systemCoordinate.m_time
            << std::setw(OutputParameters::collumnWidth) << systemCoordinate.m_mobilePosition
            << std::setw(OutputParameters::collumnWidth) << systemCoordinate.m_nRightPullingCrosslinkers << '\n';
    }
    return out;
}
