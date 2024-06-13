//
//  Polynomial.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"

using namespace RS;

Polynomial::Polynomial(const GaloisField* const galoisField)
    : m_GaloisField(galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
}

Polynomial::Polynomial(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint32_t>(m_Coefficients.size());
    m_GaloisField = galoisField;
}

Polynomial::Polynomial(const RSWord* const coefficients, const uint32_t numOfCoefficients, const GaloisField* const galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
    
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_NumOfCoefficients = numOfCoefficients;
    m_Coefficients.resize(m_NumOfCoefficients);
    
    if(coefficients)
        std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    
    m_GaloisField = galoisField;
}

void Polynomial::Add(const Polynomial* const polynomial)
{
    const uint32_t numCoefficients = std::max(m_NumOfCoefficients, polynomial->m_NumOfCoefficients);
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint32_t i = 0; i < m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - m_NumOfCoefficients] = m_Coefficients[i];
    
    for(uint32_t i = 0; i < polynomial->m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - polynomial->m_NumOfCoefficients] ^= polynomial->m_Coefficients[i];
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

void Polynomial::Scale(const RSWord scalar)
{
    for(uint32_t i = 0; i < m_NumOfCoefficients; i++)
        m_Coefficients[i] = m_GaloisField->Multiply(m_Coefficients[i], scalar);
}

void Polynomial::Multiply(const Polynomial* const polynomial)
{
    const uint32_t numCoefficients = m_NumOfCoefficients + polynomial->m_NumOfCoefficients - 1;
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint32_t i = 0; i < m_NumOfCoefficients; i++)
    {
        for(uint32_t j = 0; j < polynomial->m_NumOfCoefficients; j++)
            coefficients[i + j] ^= m_GaloisField->Multiply(m_Coefficients[i], polynomial->m_Coefficients[j]);
    }
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

Polynomial Polynomial::operator* (RSWord scalar) const
{
    Polynomial result = *this;
    
    for(uint32_t i = 0; i < m_NumOfCoefficients; i++)
        result[i] = m_GaloisField->Multiply(result[i], scalar);
    
    return result;
}

// Extended synthetic division
// Function expects polynomials sorted from biggest to lowest degree
void Polynomial::Divide(const Polynomial* const divisor, Polynomial* const quotient, Polynomial* const remainder)
{
    std::vector<RSWord> tmp = m_Coefficients;
    const RSWord normalizer = divisor->m_Coefficients[0];
    
    const uint32_t upperLimit = m_NumOfCoefficients - divisor->m_NumOfCoefficients + 1;
    
    for(uint32_t i = 0; i < upperLimit; i++)
    {
        tmp[i] = m_GaloisField->Divide(tmp[i], normalizer); // Needed for non monic polynomials
        const RSWord coefficient = tmp[i];
        
        // Skip log(0)
        if(coefficient == 0)
            continue;
        
        for(uint32_t j = 1; j < divisor->m_NumOfCoefficients; j++)
        {
            if(divisor->m_Coefficients[j] != 0) // Skip log(0)
                tmp[i + j] ^= m_GaloisField->Multiply(divisor->m_Coefficients[j], coefficient);
        }
    }
    
    // Save result
    m_Coefficients = tmp;
    
    const uint32_t separator = m_NumOfCoefficients - divisor->m_NumOfCoefficients + 1;
    
    // Extract quotient
    if(quotient)
        quotient->SetNew(tmp.data(), separator);
    
    // Extract remainder
    if(remainder)
        remainder->SetNew(tmp.data() + separator, divisor->m_NumOfCoefficients - 1);
}

void Polynomial::Reverse()
{
    std::reverse(m_Coefficients.begin(), m_Coefficients.end());
}

RSWord Polynomial::Evaluate(const RSWord x) const
{
    RSWord result = m_Coefficients[0];
    for(uint32_t i = 1; i < m_NumOfCoefficients; i++)
        result = m_GaloisField->Multiply(result, x) ^ m_Coefficients[i];
    
    return result;
}

std::vector<uint32_t> Polynomial::ChienSearch(const uint32_t max) const
{
    std::vector<uint32_t> result;
    Polynomial tmp = *this;
    
    for(uint32_t i = 0; i < max; i++)
    {
        RSWord sum = 0;
        for(uint32_t j = 0; j < m_NumOfCoefficients; j++)
        {
            sum ^= tmp[j];
            
            const uint32_t index = m_NumOfCoefficients - j - 1;
            tmp[j] = m_GaloisField->Multiply(tmp[j], m_GaloisField->GetExponentialTable()[index]);
        }
        
        if(sum == 0)
            result.push_back(i);
    }
    
    return result;
}

void Polynomial::Enlarge(const uint32_t add, const RSWord value)
{
    if(add < 1)
        throw std::invalid_argument("Enlargement must be greater than zero.");
    
    m_NumOfCoefficients += add;
    m_Coefficients.resize(m_NumOfCoefficients, value);
}

// Cut n elements from the right
void Polynomial::TrimEnd(const uint32_t n)
{
    if(n > m_NumOfCoefficients)
        throw std::invalid_argument("Cannot erase more elements than the size of the polynomial.");
    
    m_NumOfCoefficients -= n;
    m_Coefficients.resize(m_NumOfCoefficients);
}

// Cut n elements from the left
void Polynomial::TrimBeginning(const uint32_t n)
{
    if(n > m_NumOfCoefficients)
        throw std::invalid_argument("Cannot erase more elements than the size of the polynomial.");
    
    m_NumOfCoefficients -= n;
    m_Coefficients.erase(m_Coefficients.begin(), m_Coefficients.begin() + n);
}

void Polynomial::SetNew(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField)
{
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint32_t>(m_Coefficients.size());
    
    if(galoisField)
        m_GaloisField = galoisField;
}

void Polynomial::SetNew(const RSWord* const coefficients, const uint32_t numOfCoefficients, const GaloisField* const galoisField)
{
    if(!coefficients)
        throw std::invalid_argument("Coefficients cannot be nullptr.");
    
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_Coefficients.clear();
    
    m_NumOfCoefficients = numOfCoefficients;
    m_Coefficients.resize(m_NumOfCoefficients);
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    
    if(galoisField)
        m_GaloisField = galoisField;
}

void Polynomial::Print() const
{
    auto printSeparated = [printComma = false](const RSWord& word) mutable
    {
        if(printComma)
            std::cout << ", ";
        
        std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(word) << std::dec;
        
        printComma = true;
    };
    
    std::cout << "Polynomial (n=" << m_NumOfCoefficients << "): [";
    
    for(RSWord i : m_Coefficients)
        printSeparated(i);
    
    std::cout << "]" << std::endl << std::endl;
}
