#ifndef PARAMETERMAP_HPP
#define PARAMETERMAP_HPP

#include <deque>
#include <stdexcept> // std::out_of_range
#include <string>
#include <unordered_map>

#include "filament-sliding/GenericValue.hpp"
#include "filament-sliding/InputException.hpp"

class ParameterMap {
  private:
    /* Store parameters as Key/Value (string/GenericValue) in m_parameterMap.
     * Store the parameter names (the keys) also in the deque, which is FIFO.
     * Together, it can be printed as a FIFO map.
     */
    std::unordered_map<std::string, GenericValue> m_parameterMap;
    std::deque<std::string> m_parameterOrder;
    std::unordered_map<std::string, std::string> m_possibleValuesMap;

    // This function makes the syntax for adding elements easier.
    template <typename T>
    void defineParameter(
            const std::string name,
            T value,
            std::string unit = "unitless",
            std::string possibleValues = "all") {
        m_parameterOrder.push_back(
                name); // Add the name to this FIFO container, such that it can
                       // be printed in the right order
        m_parameterMap.insert(
                {name,
                 GenericValue {
                         value, unit}}); // The map uses the name as a key and
                                         // the GenericValue as the value.
        m_possibleValuesMap.insert({name, possibleValues});
    }

  public:
    ParameterMap();

    /* The function gets the value of the parameter and copies it to variable.
     * By not using a return type but a reference to the variable it needs to be
     * copied into, the compiler can deduce the template type from the argument
     * type. Hence, no type has to be given calling this function
     */
    template <typename T>
    void copyParameter(const std::string& name, T& variable) {
        GenericValue variableType {
                variable, "unknown_unit"}; // Create a GenericValue with the
                                           // type of variable
        // Change a std::out_of_range to an InputException, such that the
        // exceptions can be handled uniformly.
        try {
            variableType = m_parameterMap.at(
                    name); // Call the assignment operator. This operation will
                           // throw if the types don't match
        } catch (const std::out_of_range& error) // Throw if m_parameterMap does
                                                 // not contain the key 'name'
        {
            throw InputException {
                    "Out of range error: the parameterMap does not "
                    "contain a variable with the name " +
                    name + ".\n"};
        }
        variableType.copyValue(variable); // Perform the copy operation
    }

    template <typename T>
    void overrideParameter(const std::string name, T value) {
        try {
            m_parameterMap.at(name) =
                    GenericValue {value, m_parameterMap.at(name).getUnit()};
        } catch (const std::out_of_range& error) // Throw if m_parameterMap does
                                                 // not contain the key 'name'
        {
            throw InputException {
                    "Out of range error: the parameterMap does not contain a "
                    "variable "
                    "with the name " +
                    name +
                    ", which overrideParameter() is trying to access.\n"};
        }
    }

    friend std::ostream& operator<<(
            std::ostream& out,
            const ParameterMap& parameterMap);
    friend std::istream& operator>>(
            std::istream& in,
            ParameterMap& parameterMap); // not const, needs to change it
};

std::ostream& operator<<(std::ostream& out, const ParameterMap& parameterMap);
std::istream& operator>>(std::istream& in, ParameterMap& parameterMap);

#endif // PARAMETERMAP_HPP
