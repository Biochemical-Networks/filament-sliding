#ifndef OUPUT_HPP
#define OUPUT_HPP

#include <fstream>

class Output
{
private:
    std::ofstream m_microtubulePositionFile;

    const int m_collumnWidth = 40;
public:
    Output(const std::string &runName);
    ~Output();

    void writeMicrotubulePosition(const double time, const double position);
};

#endif // OUPUT_HPP
