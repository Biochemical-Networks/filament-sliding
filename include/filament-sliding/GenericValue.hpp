#ifndef GENERICVALUE_HPP
#define GENERICVALUE_HPP

#include <cstdint>
#include <iostream>
#include <string>

/* A class that can hold a single value of different types. The same can be
 * achieved more efficiently using boost::variant (or std::variant, C++17)
 * However, the choice is made to only use the standard library, disallowing
 * boost::variant, and C++17 is not available yet This class will allow for
 * storing different types in one container (e.g. std::map). The class cannot be
 * a template class with the type of the parameter as the template parameter,
 * because this would not allow for storing different versions in one container
 *
 */

enum class AllowedTypes { TEXT, INTEGER, REAL, NONE };

class GenericValue {
  private:
    // The currentType must be set upon construction, therefore this is const.
    const AllowedTypes m_currentType = AllowedTypes::NONE;

    // Set the default values to null. Only one of the variables is used, the
    // others are garbage.
    std::string m_stringValue = "0";
    int32_t m_integerValue = 0;
    double m_realValue = 0.0;

    // The unit is kept with the value, because a dangling value is often
    // confusing / worthless
    std::string m_unit = "unitless";

  public:
    GenericValue() = delete; // no typeless constructed GenericValue

    /* Use specialized constructors and an overloaded assignment operator to
     * assign values. Are not "explicit", so arguments of functions can
     * implicitly converted to GenericValue using these constructors. If no unit
     * is given, it is assumed the variable is unitless.
     */
    GenericValue(const GenericValue& genericValue); // Copy constructor
    GenericValue(
            const std::string& stringValue,
            const std::string& unit = "unitless");
    GenericValue(int32_t integerValue, const std::string& unit = "unitless");
    GenericValue(double realValue, const std::string& unit = "unitless");

    GenericValue& operator=(
            const GenericValue& genericValue); // Standard assignment

    /* No assignment operators for the allowed types,
     * because a unit always needs to be present, and assignment takes one
     * parameter.
     */

    /* Overloaded function to copy the value to the parameter given.
     * copyValue uses a reference and no return value, such that the type can be
     * deduced by the compiler, and no type needs to be given when calling the
     * function.
     */
    void copyValue(std::string& stringValue);
    void copyValue(int32_t& integerValue);
    void copyValue(double& realValue);

    /* The output operator can be implemented straightforwardly.
     * The input operator has to assume that the type of the right-hand-side is
     * correct. This can be done, because the type cannot change after
     * construction. Additionally, the type has to be given as a string upon
     * input.
     */
    friend std::ostream& operator<<(
            std::ostream& out,
            const GenericValue& genericValue);
    friend std::istream& operator>>(
            std::istream& in,
            GenericValue& genericValue); // not const, needs to change it
};

std::ostream& operator<<(std::ostream& out, const GenericValue& genericValue);
std::istream& operator>>(std::istream& in, GenericValue& genericValue);

AllowedTypes convertToType(
        const std::string& type); // Function to convert some allowed string
                                  // values to AllowedTypes

#endif // GENERICVALUE_HPP
