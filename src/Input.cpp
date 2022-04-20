#include <fstream>
#include <iostream> // For user interaction
#include <limits> // For std::numeric_limits<std::streamsize>::max()
#include <string>
#include <sys/stat.h> // Only works on POSIX systems (probably not windows)

#include "filament-sliding/CommandArgumentHandler.hpp"
#include "filament-sliding/GenericValue.hpp" // Defines the overloaded operator >>
#include "filament-sliding/Input.hpp"
#include "filament-sliding/InputException.hpp"

// Handle all input exceptions in the constructor of Input, because here is
// should be decided whether a default input file needs to be created. When the
// program does not have a
Input::Input(const CommandArgumentHandler& cmd, const std::string fileName):
        m_fileName(fileName) {
    try {
        read();
    } catch (InputException&
                     inputException) // Catch problems caused by an non-existing
                                     // or wrongly formatted input file
    {
        try {
            produceDefault();
            read(); // Read again, otherwise data that was already read in
                    // before encountering a mistake will remain in
                    // m_parameterMap
        } catch (InputException&
                         inputException) // Catch problems caused by the user
                                         // not wanting to override the input
                                         // file, or file permission issues
        {
            exit(1); // Terminate the program if no input file is present
        }
    }

    // Set the name of the current run, first from the input file, then from the
    // command line arguments, adding labels if the name was previously used
    if (cmd.runNameDefined()) {
        m_parameterMap.overrideParameter("runName", cmd.getRunName());
    }
    setRunName();
    m_parameterMap.overrideParameter("runName", m_runName);

    if (cmd.mobileLengthDefined()) {
        m_parameterMap.overrideParameter(
                "lengthMobileMicrotubule", cmd.getMobileLength());
    }

    if (cmd.numberPassiveDefined()) {
        m_parameterMap.overrideParameter(
                "numberPassiveCrosslinkers", cmd.getNumberPassive());
    }

    // Write the used input data to an output file, such that the data of the
    // run is stored together with the other information about the run. Do this
    // early (not in the destructor), since a second run can start without the
    // first being finished

    std::ofstream storeInputInformation {
            m_runName + "." +
            m_fileName}; // Prepend the name of the run to the inputFile name
    storeInputInformation << m_parameterMap; // Write the previously read
                                             // parameterMap to an output file
    // storeInputInformation closes because it goes out of scope
}

Input::~Input() {}

void Input::read() {
    std::ifstream inputFile(
            m_fileName
                    .c_str()); // Open and close here and don't make it a member
                               // variable, because then the stream would remain
                               // open after failing half way through, and
                               // you'll read from a certain position

    if (!inputFile) {
        throw InputException(std::string(
                "The input file could not be opened")); // Handling the
                                                        // exception can include
                                                        // asking for a default
    }
    else {
        inputFile >>
                m_parameterMap; // Read the file into memory (parameterMap).
                                // The operator is overloaded
    }
}

void Input::produceDefault() {
    if (askMakeDefault()) // returns true if the user wants a default file to be
                          // created
    {
        std::ofstream inputFile(m_fileName.c_str());

        if (!inputFile) {
            throw InputException(std::string(
                    "Program stopped because no input file could be created."));
        }
        else {
            inputFile << ParameterMap(); // Create an anonymous object that
                                         // contains the default values for the
                                         // parameters, and print it to the file
        }
    }
    else {
        throw InputException(std::string(
                "User has stopped the creation of a default input file"));
    }
}

// The only purpose of the function is to ask the user to produce a default
// input file
bool Input::askMakeDefault() {
    bool redo = true; // The parameter is set to false when a right input is
                      // given, and the loop should be terminated
    bool answerBool = false; // The variable to return
    constexpr int32_t maximumNumberOfTriesForAnswer = 5;
    int32_t numberOfTriesForAnswer = 0;
    while (redo) {
        ++numberOfTriesForAnswer;
        if (numberOfTriesForAnswer > maximumNumberOfTriesForAnswer) {
            throw InputException(
                    "User failed to answer question on creating a "
                    "default input file too often.");
        }

        std::cout << "Would you like the program to produce a default input "
                     "file, "
                     "and override the current file? (Y/n)\n";
        char answer = '\0';
        std::cin >> answer;

        std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'); // Delete all remaining characters in the input
                       // stream, without an assumption on the input size

        if (std::cin.fail()) // Check if cin was able to extract a character
        {
            std::cin.clear(); // Reset error flags
            redo = true;
        }
        else {
            if (answer == 'Y') {
                answerBool = true;
                redo = false;
            }
            else if (answer == 'n') {
                answerBool = false;
                redo = false;
            }
            else {
                std::cout << "Please enter yes (Y) or no (n). Be aware of "
                             "capitalisation.\n";
                redo = true;
            }
        }
    }
    return answerBool;
}

bool Input::fileExists(const std::string& fileName) {
    /* Taken from
     * "https://stackoverflow.com/questions/4316442/stdofstream-check-if-file-exists-before-writing"
     * Uses stat, which is defined on POSIX systems
     */

    struct stat buf; // Contains information about files (?)
    return (stat(fileName.c_str(), &buf) !=
            -1); // stat returns -1 if a file doesn't exist (0 if it does)
}

void Input::setRunName() {
    std::string runName;
    try {
        copyParameter("runName", runName);
    } catch (InputException runNameNonExistent) {
        std::cout
                << "\"runName\" was not one of the parameters, so the default "
                   "run name \"run\" will be used.\n";
        runName = "run";
    }
    int label = 0;
    m_runName = runName; // The name of the run is set from the input file

    // If a run with the same name was done before, then there is a copy of the
    // log file with the name m_runName+".log.txt" Use the log file, and not the
    // copied parameter file, since the latter depends on m_fileName, which can
    // vary and which means that other files that do not depend on m_fileName
    // (such as the log file) are overridden
    while (fileExists(m_runName + ".log.txt")) {
        ++label;
        m_runName = runName + "." + std::to_string(label);
    }
}

std::string Input::getRunName() { return m_runName; }
