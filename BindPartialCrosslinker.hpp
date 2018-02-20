#ifndef BINDPARTIALCROSSLINKER_HPP
#define BINDPARTIALCROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>
#include <vector>

class BindPartialCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToBind;

    SiteLocation whereToConnect(const SystemState& systemState, RandomGenerator& generator);

    std::vector<double> m_individualRates;

    const double m_springThermalRatio; // k/(4 k_B T)

public:
    BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind, const double springConstant, const double thermalEnergy);
    virtual ~BindPartialCrosslinker();

    virtual void setCurrentRate(const SystemState& systemState) override;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) override;

};

#endif // BINDPARTIALCROSSLINKER_HPP

