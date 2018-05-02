#include "MultiVector.hpp"

template <typename T, uint32_t dimension>
MultiVector<T,dimension>::MultiVector(const std::array<uint32_t, dimension> sizes, const T zero)
    :   m_sizes(sizes),
        m_trueSize(calculateTrueSize()),
        m_trueVector(m_trueSize, zero)
{
    // count upwards, since an unsigned int does not work well when you subtract (i>=0 will always be true)
    for (uint32_t i = 0; i<dimension; ++i)
    {
        if(i == 0)
        {
            m_factors[dimension-1-i] = 1;
        }
        else
        {
            m_factors[dimension-1-i] = m_factors[dimension-i]*m_sizes[dimension-i];
        }
    }
}

template <typename T, uint32_t dimension>
MultiVector<T,dimension>::~MultiVector()
{
}

template <typename T, uint32_t dimension>
uint32_t MultiVector<T,dimension>::calculateTrueSize() const
{
    uint32_t trueSize = 1;
    for(uint32_t i : m_sizes)
    {
        trueSize*= i;
    }
    return trueSize;
}

template <typename T, uint32_t dimension>
T& MultiVector<T,dimension>::at(const std::array<uint32_t, dimension> position)
{
    uint32_t truePosition = 0;
    for(uint32_t i=0; i<dimension; ++i)
    {
        truePosition += position[i]*m_factors[i];
    }
    return m_trueVector.at(truePosition);
}

template <typename T, uint32_t dimension>
T& MultiVector<T,dimension>::operator[](const std::array<uint32_t, dimension> position)
{
    return this->at(position);
}

template <typename T, uint32_t dimension>
std::array<uint32_t, dimension> MultiVector<T,dimension>::getSizes() const
{
    return m_sizes;
}

template <typename T, uint32_t dimension>
typename std::vector<T>::iterator MultiVector<T,dimension>::begin()
{
    return m_trueVector.begin();
}

template <typename T, uint32_t dimension>
typename std::vector<T>::const_iterator MultiVector<T,dimension>::begin() const
{
    return m_trueVector.begin();
}

template <typename T, uint32_t dimension>
typename std::vector<T>::iterator MultiVector<T,dimension>::end()
{
    return m_trueVector.end();
}

template <typename T, uint32_t dimension>
typename std::vector<T>::const_iterator MultiVector<T,dimension>::end() const
{
    return m_trueVector.end();
}

