#ifndef ACTINDISCONNECTEXCEPTION_HPP
#define ACTINDISCONNECTEXCEPTION_HPP

#include "GeneralException/GeneralException.hpp"
#include <string>

class ActinDisconnectException : public GeneralException
{
public:
    ActinDisconnectException(const double timeToDisconnect);
};

#endif // ACTINDISCONNECTEXCEPTION_HPP
