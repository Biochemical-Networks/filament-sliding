#ifndef COMMANDARGUMENTHANDLER_HPP
#define COMMANDARGUMENTHANDLER_HPP

#include <cstdint>
#include <sstream>
#include <string>

class CommandArgumentHandler {
private:
  enum class VariableName {
    RUNNAME,
    MOBILELENGTH,
    NUMBERPASSIVE,
    INVALID // MUST BE LAST ELEMENT DEFINED. Used to cast to the number of
            // elements. Treat this together with default.
  };

  bool m_runNameDefined;
  std::string m_runName;

  bool m_mobileMicrotubuleLengthDefined;
  double m_lengthMobile;

  bool m_numberOfPassiveCrosslinkersDefined;
  int32_t m_numberPassive;

  void readVariable(std::istringstream &&streamName,
                    std::istringstream &&streamValue);

public:
  CommandArgumentHandler(int argc, char *argv[]);
  ~CommandArgumentHandler();

  bool runNameDefined() const;
  std::string getRunName() const;

  bool mobileLengthDefined() const;
  double getMobileLength() const;

  bool numberPassiveDefined() const;
  int32_t getNumberPassive() const;
};

#endif // COMMANDARGUMENTHANDLER_HPP
