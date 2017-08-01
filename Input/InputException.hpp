#ifndef INPUTEXCEPTION_HPP
#define INPUTEXCEPTION_HPP

#include <string>
#include <iostream> // std::cerr

// A class to be thrown upon exceptions relating the input file
class InputException
{
private:
    const std::string m_error;

public:
    InputException(const std::string &error);
    InputException() = delete; // Do not allow an exception without a label

    const std::string& getError() const;
};


#endif // INPUTEXCEPTION_HPP
