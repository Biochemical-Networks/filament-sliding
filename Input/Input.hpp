#ifndef INPUT_HPP
#define INPUT_HPP

#include <fstream>
#include <string>
#include "GenericValue.hpp"
#include "ParameterMap.hpp"

// A class that handles the input file. It opens and reads it. If the file does not exist, or it has a wrong format, then the user should be asked if a default file should be produced.
class Input
{
private:
    const std::string m_fileName; // The name of the input file, possibly including the extension
    std::string m_runName; // The name of the current run. If the run given was already used, the name will be incremented
    // ParameterMap lets us pick elements by name. The order of printing is fixed, not alphabetical
    ParameterMap m_parameterMap;

    void read();

    void produceDefault();

    bool askMakeDefault();

    bool fileExists(const std::string& fileName);

    void setRunName();

public:
    Input(const std::string fileName = "parameters.txt");

    ~Input();

    std::string getRunName();

    // Carry the copyParameter function from ParameterMap to the user interface of Input.
    template <typename T>
    void copyParameter(const std::string &name, T &variable)
    {
        m_parameterMap.copyParameter(name, variable);
    }

};


#endif // INPUT_HPP

