#include <iomanip> // For std::setw()
#include <iostream> // For overloading the IO operators
#include <limits> // For std::numeric_limits

#include "filament-sliding/GenericValue.hpp" // For overloaded IO operators
#include "filament-sliding/InputException.hpp"
#include "filament-sliding/InputFileGeometry.hpp"
#include "filament-sliding/ParameterMap.hpp"

/* FOR THE CONSTRUCTOR DEFINITION, LOOK IN "DefaultParameterMap.cpp" !!!
 */

std::ostream& operator<<(std::ostream& out, const ParameterMap& parameterMap) {
    out << std::left; // Left justified
    // print the title line
    out << std::setw(InputFileGeometry::nameWidth) << "NAME"
        << std::setw(InputFileGeometry::valueWidth) << "VALUE"
        << std::setw(InputFileGeometry::unitWidth) << "UNIT"
        << std::setw(InputFileGeometry::typeWidth) << "TYPE"
        << std::setw(InputFileGeometry::possibleValuesWidth)
        << "POSSIBLE_VALUES" << '\n';

    for (auto name:
         parameterMap.m_parameterOrder) // Use parameterOrder to output with the
                                        // proper order
    {
        out << std::setw(InputFileGeometry::nameWidth)
            << name; // Output the name left justified
        out << parameterMap.m_parameterMap.at(
                name); // Print the GenericValue that is stored with the name
        out << std::setw(InputFileGeometry::possibleValuesWidth)
            << parameterMap.m_possibleValuesMap.at(name)
            << '\n'; // Print the possible values of the parameter
    }
    return out;
}

std::istream& operator>>(std::istream& in, ParameterMap& parameterMap) {
    std::string titleLine;
    std::getline(in, titleLine); // Throw this away

    for (auto parameterName:
         parameterMap.m_parameterOrder) // For input, the order is also fixed.
                                        // This could be changed
    {
        std::string readName;
        in >> readName;

        if (readName != parameterName) {
            throw InputException(
                    "The parameter " + readName +
                    " is not recognized as a valid parameter or the parameters "
                    "are not ordered correctly");
        }
        in >> parameterMap.m_parameterMap.at(
                      parameterName); // Use the overloaded input operator for
                                      // GenericValue

        std::string possibleValues;
        in >> possibleValues;

        if (possibleValues !=
            ParameterMap().m_possibleValuesMap.at(
                    parameterName)) // Check if the default ParameterMap
                                    // contains the same possibleValues as the
                                    // one read
        {
            throw InputException(
                    "The parameter " + readName +
                    " does not have correct allowed values, or the parameters "
                    "are not ordered correctly");
        }

        // Check if input failed
        if (in.fail()) {
            in.clear(); // Reset error flags
            throw InputException(
                    "At least one parameter is absent or has a wrong format");
        }
        else {
            // Remove junk from the input stream. Not sure if this is always
            // present.
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return in;
}
