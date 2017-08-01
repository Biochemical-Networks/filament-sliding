#include <iostream>
#include "Clock/Clock.hpp"
#include "Input/Input.hpp"

int main()
{
    Clock clock; // Counts time from creation to destruction
    Input input;

    int test1, test2;
    input.copyParameter("numberEquilibrationBlocks", test1);
    input.copyParameter("numberRunBlocks", test2);

    std::cout<<test1<<' '<<test2<<'\n';

    return 0;
}
