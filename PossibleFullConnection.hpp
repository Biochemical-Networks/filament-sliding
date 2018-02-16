#ifndef POSSIBLEFULLCONNECTION_HPP
#define POSSIBLEFULLCONNECTION_HPP

// Defined struct to group data about possible connections for partial linkers
struct PossibleFullConnection
{
    Crosslinker* p_partialLinker;
    SiteLocation location;
    double extension; // Is the position of the connection on the mobile microtubule minus the position on the fixed microtubule. Hence, is positive if fixed connection is closest to the origin
};



#endif // POSSIBLEFULLCONNECTION_HPP
