#ifndef ACTINDISCONNECTEXCEPTION_HPP
#define ACTINDISCONNECTEXCEPTION_HPP

#include "GeneralException/GeneralException.hpp"
#include <string>

class ActinDisconnectException : public GeneralException
{
private:
    double m_timeToDisconnect;
    double m_positionOfDisconnect;
    double m_timeLastTrackingCompletion;
    double m_totalTimeBehindTip;
    double m_totalTimeOnTip;
public:
    ActinDisconnectException(const double timeToDisconnect,
                             const double positionOfDisconnect,
                             const double timeLastTrackingCompletion,
                             const double totalTimeBehindTip,
                             const double totalTimeOnTip);
    ActinDisconnectException() = default; // Make it possible to default construct one, such that a vector containing them can contain dummy variables
    ~ActinDisconnectException() = default;
    ActinDisconnectException(const ActinDisconnectException&) = default;
    ActinDisconnectException& operator=(const ActinDisconnectException&) = default;
    ActinDisconnectException(ActinDisconnectException&&) = default;
    ActinDisconnectException& operator=(ActinDisconnectException&&) = default;

    double getTimeToDisconnect() const;
    double getPositionOfDisconnect() const;
    double getTimeLastTrackingCompletion() const;
    double getTotalTimeBehindTip() const;
    double getTotalTimeOnTip() const;
};

#endif // ACTINDISCONNECTEXCEPTION_HPP
