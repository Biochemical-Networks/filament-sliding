#ifndef GENERALEXCEPTION_HPP
#define GENERALEXCEPTION_HPP

#include <iostream> // std::cerr
#include <string>

// A class to be thrown upon exceptions relating the input file
class GeneralException {
  private:
    const std::string m_error;

  public:
    GeneralException(const std::string& error);
    GeneralException() = delete; // Do not allow an exception without a label

    const std::string& getError() const;
};

#endif // GENERALEXCEPTION_HPP
