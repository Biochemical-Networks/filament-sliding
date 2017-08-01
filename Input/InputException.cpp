#include "InputException.hpp"
#include <string>
#include <iostream> // std::cerr


InputException::InputException(const std::string &error) : m_error(error)
{
    // Output the error message upon construction
    std::cerr << m_error << '\n';
}

const std::string& InputException::getError() const
{
    return m_error;
}
