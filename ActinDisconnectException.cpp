#include "ActinDisconnectException.hpp"

ActinDisconnectException::ActinDisconnectException(const double timeToDisconnect)
    :   GeneralException("The actin disconnected./nOccured at time " + std::to_string(timeToDisconnect) + ".")
{
}
