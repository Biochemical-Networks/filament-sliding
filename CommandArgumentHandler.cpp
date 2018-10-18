#include "CommandArgumentHandler.hpp"
#include "GeneralException/GeneralException.hpp"

#include <string>
#include <sstream>
#include <iostream>

CommandArgumentHandler::CommandArgumentHandler(int argc, char* argv[])
    :   m_mobileMicrotubuleLengthDefined(false),
        m_lengthMobile(0),
        m_numberOfPassiveCrosslinkersDefined(false),
        m_numberPassive(0)
{
    const int maximumNumberOfArguments = 1+2*MAXIMUM_NUMBER_OF_VARIABLES;
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
                std::istringstream commandLineArgument(argv[1]);
                const int32_t numberSetArguments = (argc-1)/2;
                for (int32_t i=0; i!=numberSetArguments; ++i)
                {
                    readVariable(commandLineArgument);
                }
            }
        }
        catch(GeneralException except)
        {
            m_mobileMicrotubuleLengthDefined = m_numberOfPassiveCrosslinkersDefined = false; // reset if something was set
        }
    }
}

CommandArgumentHandler::~CommandArgumentHandler()
{
}

void CommandArgumentHandler::readVariable(std::istringstream& CLA)
{
    std::string variableType;
    if(!(CLA >> variableType))
    {
        throw GeneralException("CommandArgumentHandler::readVariable() encountered a something wrong where a variableType was expected.");
    }

    int32_t variableValue;
    if(!(CLA >> variableValue))
    {
        throw GeneralException("CommandArgumentHandler::readVariable() encountered a something wrong where a variableValue was expected.");
    }

    if(variableType == "-NP" || variableType == "-np" || variableType == "-nP" || variableType == "-Np")
    {
        if(m_numberOfPassiveCrosslinkersDefined)
        {
            throw GeneralException("CommandArgumentHandler::readVariable() tried to set m_numberPassive which was already set.");
        }
        m_numberPassive = variableValue;
        m_numberOfPassiveCrosslinkersDefined = true;
    }
    else if(variableType == "-LM" || variableType == "-lm" || variableType == "-lM" || variableType == "-Lm")
    {
        if(m_mobileMicrotubuleLengthDefined)
        {
            throw GeneralException("CommandArgumentHandler::readVariable() tried to set m_lengthMobile which was already set.");
        }
        m_lengthMobile = variableValue;
        m_mobileMicrotubuleLengthDefined = true;
    }
}

bool CommandArgumentHandler::mobileLengthDefined() const
{
    return m_mobileMicrotubuleLengthDefined;
}
int32_t CommandArgumentHandler::getLength() const
{
    #ifdef MYDEBUG
    if(!m_mobileMicrotubuleLengthDefined)
    {
        throw GeneralException("CommandArgumentHandler::getLength() was called when the command line did not set the length");
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
        throw GeneralException("CommandArgumentHandler::getNumberPassive() was called when the command line did not set the number of passive linkers.");
    }
    #endif // MYDEBUG

    return m_numberPassive;
}
