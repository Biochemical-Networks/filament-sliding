#ifndef MULTIVECTOR_HPP
#define MULTIVECTOR_HPP

#include <array>
#include <vector>
#include <cstdint>

template <typename T, uint32_t dimension>
class MultiVector
{
private:
    // only set size once, upon creation:
    const std::array<uint32_t, dimension> m_sizes;
    const uint32_t m_trueSize; // define after m_sizes

    std::vector<T> m_trueVector;

    std::array<uint32_t, dimension> m_factors; // the factors with which the labels need to multiplied to get the true position

    uint32_t calculateTrueSize() const;

public:
    MultiVector(const std::array<uint32_t, dimension> sizes, const T zero = 0);

    ~MultiVector();

    T& at(const std::array<uint32_t, dimension> position);

    T& operator[](const std::array<uint32_t, dimension> position);

    std::array<uint32_t, dimension> getSizes() const;

    typename std::vector<T>::iterator begin();
    typename std::vector<T>::const_iterator begin() const;
    typename std::vector<T>::iterator end();
    typename std::vector<T>::const_iterator end() const;
};


#endif // MULTIVECTOR_HPP
