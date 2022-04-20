#include <iostream> // std::cerr
#include <string>

#include "filament-sliding/GeneralException.hpp"
#include "filament-sliding/InputException.hpp"

InputException::InputException(const std::string& error):
        GeneralException(error) {}
