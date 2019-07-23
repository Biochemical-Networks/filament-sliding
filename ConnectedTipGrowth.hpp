#ifndef CONNECTEDTIPGROWTH_HPP
#define CONNECTEDTIPGROWTH_HPP

#include "Reaction.hpp"

#include <cstdint>

class ConnectedTipGrowth : public Reaction
{
private:
    const double m_growthRate;
    const bool m_unbindUponBlock;
public:
    ConnectedTipGrowth(const double growthRate, const bool unbindUponBlock);
    ~ConnectedTipGrowth() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // CONNECTEDTIPGROWTH_HPP
