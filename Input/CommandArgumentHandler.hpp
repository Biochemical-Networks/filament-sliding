#ifndef COMMANDARGUMENTHANDLER_HPP
#define COMMANDARGUMENTHANDLER_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <string>

class CommandArgumentHandler
{
private:
    enum class VariableName
    {
        RUNNAME,
        MOBILELENGTH,
        NUMBERPASSIVE,
        SPRINGCONSTANT,
        GROWTHVELOCITY,
        INVALID // MUST BE LAST ELEMENT DEFINED. Used to cast to the number of elements. Treat this together with default.
    };

    bool m_runNameDefined;
    std::string m_runName;

    bool m_mobileMicrotubuleLengthDefined;
    double m_lengthMobile;

    bool m_numberOfPassiveCrosslinkersDefined;
    int32_t m_numberPassive;

    bool m_springConstantDefined;
    double m_springConstant;

    bool m_growthVelocityDefined;
    double m_growthVelocity;

    void readVariable(std::istringstream&& streamName, std::istringstream&& streamValue);

public:
    CommandArgumentHandler(int argc, char* argv[]);
    ~CommandArgumentHandler();

    bool runNameDefined() const;
    std::string getRunName() const;

    bool mobileLengthDefined() const;
    double getMobileLength() const;

    bool numberPassiveDefined() const;
    int32_t getNumberPassive() const;

    bool springConstantDefined() const;
    double getSpringConstant() const;

    bool growthVelocityDefined() const;
    double getGrowthVelocity() const;
};

#endif // COMMANDARGUMENTHANDLER_HPP
