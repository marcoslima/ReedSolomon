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

using namespace ReedSolomon;

GaloisField::GaloisField(uint8_t exponent)
    : m_Exponent(exponent)
    , m_PrimitivePolynomial(0x11d)
{
    if(m_Exponent < 1)
        throw std::invalid_argument("Exponent must be greater than zero.");
    
    m_Cardinality = std::pow(m_Characteristic, m_Exponent);
    
    PrecomputeTables();
}

void GaloisField::PrecomputeTables()
{
    m_ExponentialTable.resize(m_Cardinality * 2);
    m_LogarithmicTable.resize(m_Cardinality);
    
    m_ExponentialTable[0] = 1;
    m_LogarithmicTable[0] = 0;
    m_LogarithmicTable[1] = 0;
    
    // Precompute
    uint32_t x = 1;
    for(uint16_t i = 1; i < m_Cardinality; i++)
    {
        x *= 2;
        
        if(x >= m_Cardinality)
            x = Subtract(x, m_PrimitivePolynomial);
        
        m_ExponentialTable[i] = static_cast<RSWord>(x);
        m_LogarithmicTable[static_cast<RSWord>(x)] = i;
    }
    
    // Extend exponential table to double the size for optimization (don't need modulo later)
    for(uint16_t i = m_Cardinality - 1; i < (m_Cardinality * 2); i++)
        m_ExponentialTable[i] = m_ExponentialTable[i - m_Cardinality - 1];
    
    // Debug print
    /*for(uint16_t i = 0; i < m_Cardinality * 2; i++)
        std::cout << "2^" << i << " = " << (int)m_ExponentialTable[i] << std::endl;
    
    for(uint16_t i = 0; i < m_Cardinality; i++)
        std::cout << "log " << i << " = " << (int)m_LogarithmicTable[i] << std::endl;*/
}

inline RSWord GaloisField::Add(RSWord x, RSWord y) const
{
    return x ^ y;
}

inline RSWord GaloisField::Subtract(RSWord x, RSWord y) const
{
    return x ^ y; // Same as addition for binary Galois field (mod 2)
}

//Russian Peasant Multiplication
/*inline RSWord GaloisField::MultiplyWithoutLookupTable(RSWord x, RSWord y) const
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

inline RSWord GaloisField::Multiply(RSWord x, RSWord y) const
{
    if(x == 0 || y == 0)
        return 0;
    
    return m_ExponentialTable[m_LogarithmicTable[x] + m_LogarithmicTable[y]];
}

inline RSWord GaloisField::Divide(RSWord x, RSWord y) const
{
    if(y == 0)
        throw std::invalid_argument("Division by zero.");
    if(x == 0)
        return 0;
        
    const uint16_t index = (m_LogarithmicTable[x] + (m_Cardinality - 1) - m_LogarithmicTable[y]) % (m_Cardinality - 1);
    //const uint16_t index = (m_LogarithmicTable[x] - m_LogarithmicTable[y] + m_Cardinality - 1);
    
    return m_ExponentialTable[index];
}

inline RSWord GaloisField::Pow(RSWord x, RSWord power) const
{
    const uint16_t index = (m_LogarithmicTable[x] * power) % (m_Cardinality - 1);
    
    return m_ExponentialTable[index];
}

inline RSWord GaloisField::Inverse(RSWord x) const
{
    const uint16_t index = (m_Cardinality - 1) - m_LogarithmicTable[x];
    
    return m_ExponentialTable[index];
}
