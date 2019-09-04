#include "GeneralException.hpp"
#include <string>
#include <iostream> // std::cerr


GeneralException::GeneralException(const std::string &error) : m_error(error)
{
    // Output the error message upon construction
    #pragma omp critical
    std::cerr << m_error << '\n';
}

const std::string& GeneralException::getError() const
{
    return m_error;
}
