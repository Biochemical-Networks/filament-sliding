#include "ActinDisconnectException.hpp"

ActinDisconnectException::ActinDisconnectException(const double timeToDisconnect, const double positionOfDisconnect)
    :   GeneralException("The actin disconnected.\nOccured at time " + std::to_string(timeToDisconnect)
                         + ".\nOccured at position " + std::to_string(positionOfDisconnect) + "."
    )
{
}
