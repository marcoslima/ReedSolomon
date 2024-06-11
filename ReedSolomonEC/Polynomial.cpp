//
//  Polynomial.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include <cstdint>
#include <vector>

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"

using namespace ReedSolomon;

/*Polynomial::Polynomial()
{
}*/

Polynomial::Polynomial(const std::vector<RSWord>& coefficients, GaloisField* galoisField)
{
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint16_t>(m_Coefficients.size());
    m_GaloisField = galoisField;
}

Polynomial::Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* galoisField)
{
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_NumOfCoefficients = numOfCoefficients;
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    
    m_GaloisField = galoisField;
}

inline void Polynomial::Add(const Polynomial* const polynomial)
{
    const uint16_t numCoefficients = std::max(m_NumOfCoefficients, polynomial->m_NumOfCoefficients);
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint16_t i = 0; i < m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - m_NumOfCoefficients] = m_Coefficients[i];
    
    for(uint16_t i = 0; i < polynomial->m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - polynomial->m_NumOfCoefficients] ^= m_Coefficients[i];
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

inline void Polynomial::Multiply(const RSWord scalar)
{
    for(uint16_t i = 0; i < m_NumOfCoefficients; i++)
        m_Coefficients[i] = m_GaloisField->Multiply(m_Coefficients[i], scalar);
}

inline void Polynomial::Multiply(const Polynomial* const polynomial)
{
}
