#include "InputException.hpp"
#include "../GeneralException/GeneralException.hpp"
#include <string>
#include <iostream> // std::cerr


InputException::InputException(const std::string &error) : GeneralException(error)
{
}
