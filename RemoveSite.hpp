#ifndef REMOVESITE_HPP
#define REMOVESITE_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"

#include <cstdint>

// Reaction to block a site: label the site to be effectively non-existant.
// Used to make tip region disappear, and the reverse reaction, UnblockSite, is used to make the tip grow

class RemoveSite : public Reaction
{
private:
    const double m_rateRemoveOneSite;
public:
    RemoveSite(const double rateRemoveOneSite);
    ~RemoveSite() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
}

#endif // REMOVESITE_HPP
