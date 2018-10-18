#ifndef COMMANDARGUMENTHANDLER_HPP
#define COMMANDARGUMENTHANDLER_HPP

#include <cstdint>
#include <string>
#include <sstream>

class CommandArgumentHandler
{
public:
    const int32_t MAXIMUM_NUMBER_OF_VARIABLES = 2;

private:
    bool m_mobileMicrotubuleLengthDefined;
    int32_t m_lengthMobile;

    bool m_numberOfPassiveCrosslinkersDefined;
    int32_t m_numberPassive;

    void readVariable(std::istringstream& CLA);

public:
    CommandArgumentHandler(int argc, char* argv[]);
    ~CommandArgumentHandler();

    bool mobileLengthDefined() const;
    int32_t getMobileLength() const;

    bool numberPassiveDefined() const;
    int32_t getNumberPassive() const;
};

#endif // COMMANDARGUMENTHANDLER_HPP
