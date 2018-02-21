#ifndef FULLCONNECTION_HPP
#define FULLCONNECTION_HPP

#include "Crosslinker.hpp"

// Defined struct to group data about a fully connected crosslinker
struct FullConnection
{
    Crosslinker* p_fullLinker;
    // Extension is the position of the connection on the mobile microtubule minus the position on the fixed microtubule. Hence, is positive if fixed connection is closest to the origin
    double extension;
};



#endif // FULLCONNECTION_HPP

