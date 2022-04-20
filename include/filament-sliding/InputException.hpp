#ifndef INPUTEXCEPTION_HPP
#define INPUTEXCEPTION_HPP

#include <string>

#include "filament-sliding/GeneralException.hpp"

// A class to be thrown upon exceptions relating the input file. Define it to
// create more general exceptions
class InputException : public GeneralException {
public:
  InputException(const std::string &error);
  InputException() = delete; // Do not allow an exception without a label
};

#endif // INPUTEXCEPTION_HPP
