#ifndef POSSIBLEFULLCONNECTION_HPP
#define POSSIBLEFULLCONNECTION_HPP

// Defined struct to group data about possible connections for partial linkers
struct PossibleFullConnection
{
    Crosslinker* p_partialLinker;
    SiteLocation location;
    double extension;
};



#endif // POSSIBLEFULLCONNECTION_HPP
