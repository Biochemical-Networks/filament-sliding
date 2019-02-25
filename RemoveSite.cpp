#include "RemoveSite.hpp"

RemoveSite::RemoveSite(const double rateRemoveOneSite)
    :   Reaction(),
        m_rateRemoveOneSite(rateRemoveOneSite)
{
}

RemoveSite::~RemoveSite()
{
}

void RemoveSite::setCurrentRate(const SystemState& systemState)
{

}

void RemoveSite::performReaction(SystemState& systemState, RandomGenerator& generator)
{

}
