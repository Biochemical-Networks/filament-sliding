#ifndef UNBINDPARTIALCROSSLINKER_HPP
#define UNBINDPARTIALCROSSLINKER_HPP

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"


class UnbindPartialCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToUnbind;

    Crosslinker& whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const;
public:
    UnbindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToUnbind);
    ~UnbindPartialCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;

};

#endif // UNBINDPARTIALCROSSLINKER_HPP
