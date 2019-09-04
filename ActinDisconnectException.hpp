#ifndef ACTINDISCONNECTEXCEPTION_HPP
#define ACTINDISCONNECTEXCEPTION_HPP

#include "GeneralException/GeneralException.hpp"
#include <string>

class ActinDisconnectException : public GeneralException
{
private:
    const double m_timeToDisconnect;
    const double m_positionOfDisconnect;
    const double m_timeLastTrackingCompletion;
    const double m_totalTimeBehindTip;
    const double m_totalTimeOnTip;
public:
    ActinDisconnectException(const double timeToDisconnect,
                             const double positionOfDisconnect,
                             const double timeLastTrackingCompletion,
                             const double totalTimeBehindTip,
                             const double totalTimeOnTip);

    double getTimeToDisconnect() const;
    double getPositionOfDisconnect() const;
    double getTimeLastTrackingCompletion() const;
    double getTotalTimeBehindTip() const;
    double getTotalTimeOnTip() const;
};

#endif // ACTINDISCONNECTEXCEPTION_HPP
