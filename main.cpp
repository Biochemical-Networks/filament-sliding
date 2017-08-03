#include <iostream>
#include "Clock/Clock.hpp"
#include "Input/Input.hpp"
#include "Microtubule.hpp"

int main()
{
    Clock clock; // Counts time from creation to destruction
    Input input;

    int test1, test2;
    input.copyParameter("numberEquilibrationBlocks", test1);
    input.copyParameter("numberRunBlocks", test2);

    std::cout<<test1<<' '<<test2<<'\n';

    double length;
    double latticeSpacing;
    input.copyParameter("lengthFixedMicrotubule", length);
    input.copyParameter("latticeSpacing", latticeSpacing);

    Microtubule microtubule(length, latticeSpacing);

    std::cout <<"Length is "<< length << " and lattice spacing is " << latticeSpacing << '\n';

    return 0;
}
