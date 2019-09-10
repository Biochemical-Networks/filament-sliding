#ifndef GENERALEXCEPTION_HPP
#define GENERALEXCEPTION_HPP

#include <string>
#include <iostream> // std::cerr

// A class to be thrown upon exceptions relating the input file
class GeneralException
{
private:
    std::string m_error;

public:
    GeneralException(const std::string &error);
    GeneralException() = default; // Allow initiation of dummy variables

    const std::string& getError() const;
};


#endif // GENERALEXCEPTION_HPP

