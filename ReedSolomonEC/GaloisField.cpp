//
//  GaloisField.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#include <cstdint>
#include <vector>

#include "Utils.hpp"
#include "GaloisField.hpp"

using namespace RS;

GaloisField::GaloisField(const uint32_t exponent)
    : m_PrimitivePolynomial(285)
    , m_Exponent(exponent)
    , m_Cardinality(1 << exponent) // 2^exponent
{
    if(m_Exponent < 1)
        throw std::invalid_argument("Exponent must be greater than zero.");
    if(m_Exponent > 32)
        throw std::invalid_argument("Exponent must be smaller than 32.");
    
    PrecomputeTables();
}

void GaloisField::PrecomputeTables()
{
    m_ExponentialTable.resize((m_Cardinality - 1) * 2);
    m_LogarithmicTable.resize(m_Cardinality);
    
    m_ExponentialTable[0] = 1;
    m_LogarithmicTable[0] = 0;
    m_LogarithmicTable[1] = 0;
    
    // Precompute
    uint32_t x = 1;
    for(uint32_t i = 1; i < (m_Cardinality - 1); i++)
    {
        x *= 2;
        
        if(x >= m_Cardinality)
            x = Subtract(x, m_PrimitivePolynomial);
        
        m_ExponentialTable[i] = static_cast<RSWord>(x);
        m_LogarithmicTable[static_cast<RSWord>(x)] = i;
    }
    
    // Extend exponential table to double the size for optimization (don't need modulo later)
    for(uint32_t i = (m_Cardinality - 1); i < (m_Cardinality - 1) * 2; i++)
        m_ExponentialTable[i] = m_ExponentialTable[i - (m_Cardinality - 1)];
    
    // Debug print
    /*for(uint32_t i = 0; i < (m_Cardinality - 1) * 2; i++)
        std::cout << "2^" << i << " = " << (int)m_ExponentialTable[i] << std::endl;
    
    for(uint32_t i = 0; i < m_Cardinality; i++)
        std::cout << "log " << i << " = " << (int)m_LogarithmicTable[i] << std::endl;*/
}

RSWord GaloisField::Add(const RSWord x, const RSWord y) const noexcept
{
    return x ^ y;
}

RSWord GaloisField::Subtract(const RSWord x, const RSWord y) const noexcept
{
    return x ^ y; // Same as addition for binary Galois field (mod 2)
}

//Russian Peasant Multiplication
/*RSWord GaloisField::MultiplyWithoutLookupTable(RSWord x, RSWord y) const
{
    RSWord result = 0;
    
    while(y > 0)
    {
        if(!Utils::IsEven<RSWord>(y))
            result = Add(result, x);
        
        y >>= 1; // y / 2
        x <<= 1; // x * 2
        
        if(x >= m_Cardinality)
            x = Subtract(x, m_PrimitivePolynomial);
    }
    
    return result;
}*/

RSWord GaloisField::Multiply(const RSWord x, const RSWord y) const
{
    if(x == 0 || y == 0)
        return 0;
    
    const uint32_t index = m_LogarithmicTable[x] + m_LogarithmicTable[y];
    
    return m_ExponentialTable[index];
}

RSWord GaloisField::Divide(const RSWord x, const RSWord y) const
{
    if(y == 0)
        throw std::invalid_argument("Division by zero.");
    if(x == 0)
        return 0;
        
    //const uint32_t index = (m_LogarithmicTable[x] + (m_Cardinality - 1) - m_LogarithmicTable[y]) % (m_Cardinality - 1);
    const uint32_t index = (m_LogarithmicTable[x] - m_LogarithmicTable[y] + (m_Cardinality - 1));
    
    return m_ExponentialTable[index];
}

RSWord GaloisField::Pow(const RSWord x, const RSWord power) const
{
    const uint32_t index = (m_LogarithmicTable[x] * power) % (m_Cardinality - 1);
    
    return m_ExponentialTable[index];
}

RSWord GaloisField::Inverse(const RSWord x) const
{
    const uint32_t index = (m_Cardinality - 1) - m_LogarithmicTable[x];
    
    return m_ExponentialTable[index];
}
