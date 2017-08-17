#include "GenericValue.hpp"
#include "InputException.hpp"
#include "InputFileGeometry.hpp" // For the sizes of columns in the input file
#include <string>
#include <iostream> // For overloading the IO operators
#include <iomanip> // For std::setw()
#include <cstdint> // For int32_t

// Default copy constructor: copy all members
GenericValue::GenericValue (const GenericValue &genericValue) : m_currentType(genericValue.m_currentType) // const member
{
    if (&genericValue == this)
    {
        throw InputException{"Do not self initialize a GenericValue"};
    }
    *this = genericValue; // Use overloaded operator= for GenericValue to prevent duplicate code.
}

GenericValue::GenericValue(const std::string &stringValue, const std::string &unit)
    : m_currentType(AllowedTypes::TEXT), m_stringValue(stringValue), m_unit(unit)
{
}

GenericValue::GenericValue(int32_t integerValue, const std::string &unit)
    : m_currentType(AllowedTypes::INTEGER), m_integerValue(integerValue), m_unit(unit)
{
}

GenericValue::GenericValue(double realValue, const std::string &unit)
    : m_currentType(AllowedTypes::REAL), m_realValue(realValue), m_unit(unit)
{
}

// Assignment operator
GenericValue& GenericValue::operator= (const GenericValue &genericValue)
{
    if (&genericValue!=this) // Nothing needs to be done at self assignment
    {
        if(!(m_currentType == genericValue.m_currentType))
        {
            throw InputException{"A GenericType with non-matching type was tried to be assigned"};
        }
        // Do all assignments, the type is already shown to be correct
        m_stringValue = genericValue.m_stringValue;
        m_integerValue = genericValue.m_integerValue;
        m_realValue = genericValue.m_realValue;

        // Assign the unit too
        m_unit = genericValue.m_unit;
    }

    return *this;
}
// No assignment operators for the allowed types, because a unit always needs to be present, and assignment takes one parameter

// Overloaded function to copy variables into the given value:

void GenericValue::copyValue(std::string &stringValue)
{
    if (m_currentType != AllowedTypes::TEXT)
    {
        throw InputException{"GenericValue tried to copy a non-string to a string value."};
    }
    stringValue = m_stringValue;
}

void GenericValue::copyValue(int32_t &integerValue)
{
    if (m_currentType != AllowedTypes::INTEGER)
    {
        throw InputException{"GenericValue tried to copy a non-integer to an integer value."};
    }
    integerValue = m_integerValue;
}

void GenericValue::copyValue(double &realValue)
{
    if (m_currentType != AllowedTypes::REAL)
    {
        throw InputException{"GenericValue tried to copy a non-real to a real value."};
    }
    realValue = m_realValue;
}



// IO operators

std::ostream& operator<< (std::ostream& out, const GenericValue &genericValue)
{
    out << std::left;
    out << std::setw(InputFileGeometry::valueWidth); // Column size for value
    switch (genericValue.m_currentType)
    {
        case AllowedTypes::TEXT:
            out << genericValue.m_stringValue;
            break;
        case AllowedTypes::INTEGER:
            out << genericValue.m_integerValue;
            break;
        case AllowedTypes::REAL:
            out << genericValue.m_realValue;
            break;
        case AllowedTypes::NONE:
            throw InputException{"A GenericValue with no type was tried to be output."};
            break;
    }
    out << std::setw(InputFileGeometry::unitWidth) << genericValue.m_unit;
    out << std::setw(InputFileGeometry::typeWidth);
    switch (genericValue.m_currentType)
    {
        case AllowedTypes::TEXT:
            out << "text";
            break;
        case AllowedTypes::INTEGER:
            out << "integer";
            break;
        case AllowedTypes::REAL:
            out << "real";
            break;
        case AllowedTypes::NONE:
            throw InputException{"A GenericValue with no type was tried to be output."}; // Already covered above, but doesn't hurt
            break;
    }
    return out; // For chaining the operator
}


std::istream& operator>> (std::istream& in, GenericValue &genericValue)
{
    /* Read the Parameter, and delete all junk from the input stream after.
     * Assume the type in the genericValue is correct. If not, then the input will be downcast or fail.
     */

    // Read the value, assuming the type
    switch (genericValue.m_currentType)
    {
        case AllowedTypes::TEXT:
            in >> genericValue.m_stringValue;
            break;
        case AllowedTypes::INTEGER:
            in >> genericValue.m_integerValue;
            break;
        case AllowedTypes::REAL:
            in >> genericValue.m_realValue;
            break;
        default:
            throw InputException{"Input was tried to be put into a GenericValue with no type."};
            break;
    }

    // Read the unit
    in >> genericValue.m_unit;

    /* For the type, read in the string and convert it to a AllowedTypes.
     * If no string is recognized or the type does not correspond to what was expected, throw an exception.
     */

    // Read the type
    std::string type;
    in >> type;

    // Check if input failed
    if (in.fail())
    {
        in.clear(); // Reset error flags
        throw InputException("At least one parameter is absent or has a wrong format");
    }

    // Convert type to AllowedTypes
    AllowedTypes givenType = convertToType(type);

    // The givenType should correspond to the type of the GenericValue that this variable will be stored in
    if (givenType != genericValue.m_currentType)
    {
        throw InputException{"An input file parameter has a type that does not correspond to the GenericValue it is stored in"};
    }

    return in;
}


// Function to convert some allowed string values to AllowedTypes
const AllowedTypes convertToType(const std::string &type)
{
    AllowedTypes givenType;
    if (type == "string" || type == "STRING" || type == "String"
            || type == "text" || type == "Text" || type == "TEXT")
    {
        givenType = AllowedTypes::TEXT;
    }
    else if (type == "integer" || type == "Integer" || type == "INTEGER"
            || type == "Int" || type == "int" || type == "INT")
    {
        givenType = AllowedTypes::INTEGER;
    }
    else if (type == "real" || type == "Real" || type == "REAL"
            || type == "double" || type == "Double" || type == "DOUBLE"
            || type == "float" || type == "Float" || type == "FLOAT")
    {
        givenType = AllowedTypes::REAL;
    }
    else
    {
        throw InputException{"An input file parameter has a type that is not recognized."};
    }

    return givenType;
}
