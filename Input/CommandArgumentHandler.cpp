#include "CommandArgumentHandler.hpp"
#include "InputException.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdint>

CommandArgumentHandler::CommandArgumentHandler(int argc, char* argv[])
    :   m_runNameDefined(false),
        m_runName(""),
        m_mobileMicrotubuleLengthDefined(false),
        m_lengthMobile(0.0),
        m_numberOfPassiveCrosslinkersDefined(false),
        m_numberPassive(0),
        m_springConstantDefined(false),
        m_springConstant(0.0),
        m_growthVelocityDefined(false),
        m_growthVelocity(0.0)
{
    constexpr int32_t maximumNumberOfArguments = 1+2*static_cast<int32_t>(VariableName::INVALID);
    if(argc>1)
    {
        try
        {
            // expect an argument declaration and value, so an even number of elements should be in argv[].
            if(argc%2 != 1 || argc > maximumNumberOfArguments) // the name of the program is also counted by argc
            {
                std::cerr << "The structure of the command line arguments is not recognised, so these will be ignored and the program proceeds with the input file arguments.\n";
            }
            else
            {
                // The number of arguments is correct

                const int32_t numberSetArguments = (argc-1)/2;
                for (int32_t i=0; i!=numberSetArguments; ++i)
                {
                    // read the variable name and value
                    readVariable(std::istringstream{argv[2*i+1]}, std::istringstream{argv[2*i+2]});
                }
            }
        }
        catch(GeneralException except)
        {
            std::cerr << "Continue with the input file values of all variables.\n";
            // reset if something was set
            m_runNameDefined = false;
            m_mobileMicrotubuleLengthDefined = false;
            m_numberOfPassiveCrosslinkersDefined = false;
            m_springConstantDefined = false;
            m_growthVelocityDefined = false;
        }
    }
}

CommandArgumentHandler::~CommandArgumentHandler()
{
}

void CommandArgumentHandler::readVariable(std::istringstream&& streamName, std::istringstream&& streamValue)
{
    std::string variableType;
    VariableName newVariable=VariableName::INVALID;
    if(!(streamName >> variableType))
    {
        throw InputException("CommandArgumentHandler::readVariable() expected a variableType, but found no valid one.");
    }
    if(variableType == "-N" || variableType == "-n")
    {
        newVariable=VariableName::RUNNAME;
    }
    else if(variableType == "-NP" || variableType == "-np" || variableType == "-nP" || variableType == "-Np" ||
       variableType == "-PN" || variableType == "-pn" || variableType == "-pN" || variableType == "-Pn")
    {
        newVariable=VariableName::NUMBERPASSIVE;
    }
    else if(variableType == "-LM" || variableType == "-lm" || variableType == "-lM" || variableType == "-Lm" ||
            variableType == "-ML" || variableType == "-ml" || variableType == "-mL" || variableType == "-Ml")
    {
        newVariable=VariableName::MOBILELENGTH;
    }
    else if(variableType == "-K" || variableType == "-k")
    {
        newVariable=VariableName::SPRINGCONSTANT;
    }
    else if(variableType == "-GV" || variableType == "-gv" || variableType == "-gV" || variableType == "-Gv" ||
            variableType == "-VG" || variableType == "-vg" || variableType == "-vG" || variableType == "-Vg")
    {
        newVariable=VariableName::GROWTHVELOCITY;
    }
    else
    {
        newVariable=VariableName::INVALID;
        throw InputException("CommandArgumentHandler::readVariable() could not match the input variable "+variableType+" to any known variable.");
    }

    switch(newVariable)
    {
        case VariableName::RUNNAME:
            if(m_runNameDefined)
            {
                throw InputException("CommandArgumentHandler::readVariable() tried to set the run name more than once.");
            }
            if(!(streamValue >> m_runName))
            {
                throw InputException("CommandArgumentHandler::readVariable() did not encounter a proper run name.");
            }
            m_runNameDefined = true;
            break;
        case VariableName::NUMBERPASSIVE:
            if(m_numberOfPassiveCrosslinkersDefined)
            {
                throw InputException("CommandArgumentHandler::readVariable() tried to set the number of passive linkers more than once.");
            }
            if(!(streamValue >> m_numberPassive))
            {
                throw InputException("CommandArgumentHandler::readVariable() did not encounter a proper number of passive linkers.");
            }
            m_numberOfPassiveCrosslinkersDefined = true;
            break;
        case VariableName::MOBILELENGTH:
            if(m_mobileMicrotubuleLengthDefined)
            {
                throw InputException("CommandArgumentHandler::readVariable() tried to set the mobile microtubule length more than once.");
            }
            if(!(streamValue >> m_lengthMobile))
            {
                throw InputException("CommandArgumentHandler::readVariable() did not encounter a proper length of the mobile microtubule.");
            }
            m_mobileMicrotubuleLengthDefined = true;
            break;
        case VariableName::SPRINGCONSTANT:
            if(m_springConstantDefined)
            {
                throw InputException("CommandArgumentHandler::readVariable() tried to set the springconstant more than once.");
            }
            if(!(streamValue >> m_springConstant))
            {
                throw InputException("CommandArgumentHandler::readVariable() did not encounter a proper springconstant.");
            }
            m_springConstantDefined = true;
            break;
        case VariableName::GROWTHVELOCITY:
            if(m_growthVelocityDefined)
            {
                throw InputException("CommandArgumentHandler::readVariable() tried to set the growth velocity more than once.");
            }
            if(!(streamValue >> m_growthVelocity))
            {
                throw InputException("CommandArgumentHandler::readVariable() did not encounter a proper growth velocity.");
            }
            m_growthVelocityDefined = true;
            break;
        case VariableName::INVALID:
        default:
            throw InputException("CommandArgumentHandler::readVariable() set newVariable to an invalid value.");
    }
}

bool CommandArgumentHandler::runNameDefined() const
{
    return m_runNameDefined;
}

std::string CommandArgumentHandler::getRunName() const
{
    #ifdef MYDEBUG
    if(!m_runNameDefined)
    {
        throw InputException("CommandArgumentHandler::getRunName() was called when the command line did not set the run name");
    }
    #endif // MYDEBUG

    return m_runName;
}

bool CommandArgumentHandler::mobileLengthDefined() const
{
    return m_mobileMicrotubuleLengthDefined;
}
double CommandArgumentHandler::getMobileLength() const
{
    #ifdef MYDEBUG
    if(!m_mobileMicrotubuleLengthDefined)
    {
        throw InputException("CommandArgumentHandler::getLength() was called when the command line did not set the length");
    }
    #endif // MYDEBUG

    return m_lengthMobile;
}

bool CommandArgumentHandler::numberPassiveDefined() const
{
    return m_numberOfPassiveCrosslinkersDefined;
}

int32_t CommandArgumentHandler::getNumberPassive() const
{
    #ifdef MYDEBUG
    if(!m_numberOfPassiveCrosslinkersDefined)
    {
        throw InputException("CommandArgumentHandler::getNumberPassive() was called when the command line did not set the number of passive linkers.");
    }
    #endif // MYDEBUG

    return m_numberPassive;
}

bool CommandArgumentHandler::springConstantDefined() const
{
    return m_springConstantDefined;
}

double CommandArgumentHandler::getSpringConstant() const
{
    #ifdef MYDEBUG
    if(!m_springConstantDefined)
    {
        throw InputException("CommandArgumentHandler::getSpringConstant() was called when the command line did not set the spring constant.");
    }
    #endif // MYDEBUG

    return m_springConstant;
}

bool CommandArgumentHandler::growthVelocityDefined() const
{
    return m_growthVelocityDefined;
}

double CommandArgumentHandler::getGrowthVelocity() const
{
    #ifdef MYDEBUG
    if(!m_growthVelocityDefined)
    {
        throw InputException("CommandArgumentHandler::getGrowthVelocity() was called when the command line did not set the growth velocity.");
    }
    #endif // MYDEBUG

    return m_growthVelocity;
}
