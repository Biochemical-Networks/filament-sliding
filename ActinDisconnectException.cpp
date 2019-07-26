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
                         + "."
    )
{
}
