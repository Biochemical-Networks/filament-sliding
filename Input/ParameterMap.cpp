/* FOR THE CONSTRUCTOR DEFINITION, LOOK IN "DefaultParameterMap.cpp" !!!
 */

#include "ParameterMap.hpp"
#include "GenericValue.hpp" // For overloaded IO operators
#include <iostream> // For overloading the IO operators
#include <iomanip> // For std::setw()
#include "InputException.hpp"
#include "InputFileGeometry.hpp"

std::ostream& operator<< (std::ostream& out, const ParameterMap &parameterMap)
{
    out << std::left; // Left justified
    // print the title line
    out << std::setw(InputFileGeometry::nameWidth) << "NAME"
        << std::setw(InputFileGeometry::valueWidth) << "VALUE"
        << std::setw(InputFileGeometry::unitWidth) << "UNIT"
        << std::setw(InputFileGeometry::typeWidth) << "TYPE" << '\n';

    for (auto name : parameterMap.m_parameterOrder) // Use parameterOrder to output with the proper order
    {
        out << std::setw(InputFileGeometry::nameWidth) << name; // Output the name left justified
        out << parameterMap.m_parameterMap.at(name) << '\n'; // Print the GenericValue that is stored with the name
    }
    return out;
}


std::istream& operator>> (std::istream& in, ParameterMap &parameterMap)
{
    std::string titleLine;
    std::getline(in, titleLine); // Throw this away

    for (auto parameterName : parameterMap.m_parameterOrder) // For input, the order is also fixed. This could be changed
    {
        std::string readName;
        in >> readName;

        if (readName != parameterName)
        {
            throw InputException("The parameter "+readName+" is not recognized as a valid parameter or the parameters are not ordered correctly");
        }
        in >> parameterMap.m_parameterMap.at(parameterName); // Use the overloaded input operator for GenericValue

    }
    return in;
}
