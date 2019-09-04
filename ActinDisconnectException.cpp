#include "ActinDisconnectException.hpp"

ActinDisconnectException::ActinDisconnectException(const double timeToDisconnect,
                                                   const double positionOfDisconnect,
                                                   const double timeLastTrackingCompletion,
                                                   const double totalTimeBehindTip,
                                                   const double totalTimeOnTip)
    :   GeneralException("The actin disconnected.\nOccured at time " + std::to_string(timeToDisconnect)
                         + ".\nOccured at position " + std::to_string(positionOfDisconnect)
                         + ".\nThe last tracking occured at time " + std::to_string(timeLastTrackingCompletion)
                         + ".\nThe total time spent on the lattice was " + std::to_string(totalTimeBehindTip)
                         + ".\nThe total time spent on the tip was " + std::to_string(totalTimeOnTip)
                         + "."),
        m_timeToDisconnect(timeToDisconnect),
        m_positionOfDisconnect(positionOfDisconnect),
        m_timeLastTrackingCompletion(timeLastTrackingCompletion),
        m_totalTimeBehindTip(totalTimeBehindTip),
        m_totalTimeOnTip(totalTimeOnTip)
{
}

double ActinDisconnectException::getTimeToDisconnect() const
{
    return m_timeToDisconnect;
}
double ActinDisconnectException::getPositionOfDisconnect() const
{
    return m_positionOfDisconnect;
}

double ActinDisconnectException::getTimeLastTrackingCompletion() const
{
    return m_timeLastTrackingCompletion;
}

double ActinDisconnectException::getTotalTimeBehindTip() const
{
    return m_totalTimeBehindTip;
}

double ActinDisconnectException::getTotalTimeOnTip() const
{
    return m_totalTimeOnTip;
}
