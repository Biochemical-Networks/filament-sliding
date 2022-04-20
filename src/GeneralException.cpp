#include <iostream> // std::cerr
#include <string>

#include "filament-sliding/GeneralException.hpp"

GeneralException::GeneralException(const std::string& error): m_error(error) {
    // Output the error message upon construction
    std::cerr << m_error << '\n';
}

const std::string& GeneralException::getError() const { return m_error; }
