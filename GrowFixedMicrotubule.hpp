#ifndef GROWFIXEDMICROTUBULE_HPP
#define GROWFIXEDMICROTUBULE_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <cstdint>

class GrowFixedMicrotubule : public Reaction
{
private:
    const double m_growthRate;
public:
    GrowFixedMicrotubule(const double growthRate);
    ~GrowFixedMicrotubule() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;

    std::string identity() const override;
};

#endif // GROWFIXEDMICROTUBULE_HPP
