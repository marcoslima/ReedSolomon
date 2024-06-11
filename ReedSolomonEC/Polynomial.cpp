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

using namespace RS;

/*Polynomial::Polynomial()
{
}*/

Polynomial::Polynomial(const std::vector<RSWord>& coefficients, GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint16_t>(m_Coefficients.size());
    m_GaloisField = galoisField;
}

Polynomial::Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    if(!coefficients)
        throw std::invalid_argument("Coefficients cannot be nullptr.");
    
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_NumOfCoefficients = numOfCoefficients;
    m_Coefficients.resize(m_NumOfCoefficients);
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    
    m_GaloisField = galoisField;
}

void Polynomial::Add(const Polynomial* const polynomial)
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

void Polynomial::Scale(const RSWord scalar)
{
    for(uint16_t i = 0; i < m_NumOfCoefficients; i++)
        m_Coefficients[i] = m_GaloisField->Multiply(m_Coefficients[i], scalar);
}

void Polynomial::Multiply(const Polynomial* const polynomial)
{
    const uint16_t numCoefficients = m_NumOfCoefficients + polynomial->m_NumOfCoefficients - 1;
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint16_t i = 0; i < m_NumOfCoefficients; i++)
    {
        for(uint16_t j = 0; j < polynomial->m_NumOfCoefficients; j++)
            coefficients[i + j] ^= m_GaloisField->Multiply(m_Coefficients[i], polynomial->m_Coefficients[j]);
    }
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

RSWord Polynomial::Evaluate(const RSWord x) const
{
    RSWord result = m_Coefficients[0];
    for(uint16_t i = 0; i < m_NumOfCoefficients; i++)
        result = m_GaloisField->Multiply(result, x) ^ m_Coefficients[i];
    
    return result;
}

void Polynomial::SetNew(const std::vector<RSWord>& coefficients, GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    m_Coefficients.clear();
    
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint16_t>(m_Coefficients.size());
    m_GaloisField = galoisField;
}

void Polynomial::SetNew(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    if(!coefficients)
        throw std::invalid_argument("Coefficients cannot be nullptr.");
    
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_Coefficients.clear();
    
    m_NumOfCoefficients = numOfCoefficients;
    m_Coefficients.resize(m_NumOfCoefficients);
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    
    m_GaloisField = galoisField;
}
