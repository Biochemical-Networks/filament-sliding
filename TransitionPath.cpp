#include "TransitionPath.hpp"
#include "OutputParameters.hpp"
#include "GeneralException/GeneralException.hpp"

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

int32_t TransitionPath::getSize() const
{
    return m_pathVector.size();
}

double TransitionPath::getMobilePosition(const int32_t label) const
{
    try
    {
        return m_pathVector.at(label).m_mobilePosition;
    }
    catch(const std::out_of_range& outOfRange)
    {
        throw GeneralException(std::string("TransitionPath::getMobilePosition() label went out of range: ")+std::string(outOfRange.what()));
    }
}

int32_t TransitionPath::getNRightPullingLinkers(const int32_t label) const
{
    try
    {
        return m_pathVector.at(label).m_nRightPullingCrosslinkers;
    }
    catch(const std::out_of_range& outOfRange)
    {
        throw GeneralException(std::string("TransitionPath::getNRightPullingLinkers() label went out of range: ")+std::string(outOfRange.what()));
    }
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
