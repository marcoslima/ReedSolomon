/*
    The zlib License

    Copyright (C) 2024 Marc Schöndorf
 
This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1.  The origin of this software must not be misrepresented; you must not claim
    that you wrote the original software. If you use this software in a product,
    an acknowledgment in the product documentation would be appreciated but is
    not required.

2.  Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

3.  This notice may not be removed or altered from any source distribution.
*/

/*------------------------------------------------------------------*/
/*                                                                  */
/*                      (C) 2024 Marc Schöndorf                     */
/*                            See license                           */
/*                                                                  */
/*  Polynomial.cpp                                                  */
/*  Created: 11.06.2024                                             */
/*------------------------------------------------------------------*/

#include "ReedSolomon.hpp"

using namespace NReedSolomon;

Polynomial::Polynomial(const GaloisField* const galoisField)
    : m_GaloisField(galoisField)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
}

Polynomial::Polynomial(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField)
    : m_GaloisField(galoisField)
    , m_NumOfCoefficients(coefficients.size())
    , m_Coefficients(coefficients)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
}

Polynomial::Polynomial(const RSWord* const coefficients, const uint64_t numOfCoefficients, const GaloisField* const galoisField)
    : m_GaloisField(galoisField)
    , m_NumOfCoefficients(numOfCoefficients)
{
    if(!galoisField)
        throw std::invalid_argument("GaloisField cannot be nullptr.");
     
    m_Coefficients.resize(m_NumOfCoefficients); // Allocate memory
    
    if(coefficients && m_NumOfCoefficients > 0)
    {
        std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
    }
}

void Polynomial::SetNew(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField)
{
    if(galoisField)
        m_GaloisField = galoisField;
    
    m_Coefficients = coefficients;
    m_NumOfCoefficients = m_Coefficients.size();
}

void Polynomial::SetNew(const RSWord* const coefficients, const uint64_t numOfCoefficients, const GaloisField* const galoisField)
{
    if(!coefficients)
        throw std::invalid_argument("Coefficients cannot be nullptr.");
    
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    if(galoisField)
        m_GaloisField = galoisField;
    
    m_Coefficients.clear();
    
    m_NumOfCoefficients = numOfCoefficients;
    m_Coefficients.resize(m_NumOfCoefficients);
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
}

void Polynomial::Clear()
{
    m_Coefficients.clear();
    m_NumOfCoefficients = 0;
}

void Polynomial::Add(const Polynomial* const polynomial)
{
    const uint64_t numCoefficients = std::max(m_NumOfCoefficients, polynomial->m_NumOfCoefficients);
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint64_t i = 0; i < m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - m_NumOfCoefficients] = m_Coefficients[i];
    
    for(uint64_t i = 0; i < polynomial->m_NumOfCoefficients; i++)
        coefficients[i + numCoefficients - polynomial->m_NumOfCoefficients] ^= polynomial->m_Coefficients[i];
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

void Polynomial::Scale(const RSWord scalar)
{
    for(uint64_t i = 0; i < m_NumOfCoefficients; i++)
        m_Coefficients[i] = m_GaloisField->Multiply(m_Coefficients[i], scalar);
}

Polynomial Polynomial::operator* (const RSWord scalar) const
{
    Polynomial result = *this;
    
    for(uint64_t i = 0; i < m_NumOfCoefficients; i++)
        result[i] = m_GaloisField->Multiply(result[i], scalar);
    
    return result;
}

void Polynomial::Multiply(const Polynomial* const polynomial)
{
    const uint64_t numCoefficients = m_NumOfCoefficients + polynomial->m_NumOfCoefficients - 1;
    std::vector<RSWord> coefficients(numCoefficients, 0);
    
    for(uint64_t i = 0; i < m_NumOfCoefficients; i++)
    {
        for(uint64_t j = 0; j < polynomial->m_NumOfCoefficients; j++)
            coefficients[i + j] ^= m_GaloisField->Multiply(m_Coefficients[i], polynomial->m_Coefficients[j]);
    }
    
    m_NumOfCoefficients = numCoefficients;
    m_Coefficients = coefficients;
}

// Extended synthetic division
void Polynomial::Divide(const Polynomial* const divisor, Polynomial* const quotient_out, Polynomial* const remainder_out)
{
    if(divisor->m_NumOfCoefficients > m_NumOfCoefficients)
        throw std::runtime_error("Divisor has more coefficients than dividend.");
    
    std::vector<RSWord> tmp = m_Coefficients;
    const RSWord normalizer = divisor->m_Coefficients[0];
    const uint64_t upperLimit = m_NumOfCoefficients - divisor->m_NumOfCoefficients + 1;
    
    for(uint64_t i = 0; i < upperLimit; i++)
    {
        tmp[i] = m_GaloisField->Divide(tmp[i], normalizer); // Needed for non-monic polynomials
        const RSWord coefficient = tmp[i];
        
        // Skip log(0)
        if(coefficient == 0)
            continue;
        
        for(uint64_t j = 1; j < divisor->m_NumOfCoefficients; j++)
        {
            if(divisor->m_Coefficients[j] != 0) // Skip log(0)
                tmp[i + j] ^= m_GaloisField->Multiply(divisor->m_Coefficients[j], coefficient);
        }
    }
    
    // Save result
    m_Coefficients = tmp;
        
    // Extract quotient
    if(quotient_out)
        quotient_out->SetNew(tmp.data(), upperLimit);
    
    // Extract remainder
    if(remainder_out)
        remainder_out->SetNew(tmp.data() + upperLimit, divisor->m_NumOfCoefficients - 1);
}

void Polynomial::Reverse()
{
    std::ranges::reverse(m_Coefficients);
}

RSWord Polynomial::Evaluate(const RSWord x) const
{
    RSWord result = m_Coefficients[0];
    for(uint64_t i = 1; i < m_NumOfCoefficients; i++)
        result = m_GaloisField->Multiply(result, x) ^ m_Coefficients[i];
    
    return result;
}

std::vector<uint64_t> Polynomial::ChienSearch(const uint64_t max) const
{
    std::vector<uint64_t> result;
    Polynomial tmp = *this;
    
    for(uint64_t i = 0; i < max; i++)
    {
        RSWord sum = 0;
        for(uint64_t j = 0; j < m_NumOfCoefficients; j++)
        {
            sum ^= tmp[j];
            
            const uint64_t index = m_NumOfCoefficients - j - 1;
            tmp[j] = m_GaloisField->Multiply(tmp[j], m_GaloisField->GetExponentialTable()[index]);
        }
        
        if(sum == 0)
            result.push_back(i);
    }
    
    return result;
}

void Polynomial::Enlarge(const uint64_t numElementsToAdd, const RSWord value)
{
    if(numElementsToAdd < 1)
        throw std::invalid_argument("Enlargement must be greater than zero.");
    
    m_NumOfCoefficients += numElementsToAdd;
    m_Coefficients.resize(m_NumOfCoefficients, value);
}

// Cut n elements from the end
void Polynomial::TrimEnd(const uint64_t numElementsToTrim)
{
    if(numElementsToTrim > m_NumOfCoefficients)
        throw std::invalid_argument("Cannot trim more elements than the size of the polynomial.");
    
    m_NumOfCoefficients -= numElementsToTrim;
    m_Coefficients.resize(m_NumOfCoefficients);
}

// Cut n elements from the beginning
void Polynomial::TrimBeginning(const uint64_t numElementsToTrim)
{
    if(numElementsToTrim > m_NumOfCoefficients)
        throw std::invalid_argument("Cannot trim more elements than the size of the polynomial.");
    
    m_NumOfCoefficients -= numElementsToTrim;
    m_Coefficients.erase(m_Coefficients.begin(), m_Coefficients.begin() + static_cast<coef_diff_type>(numElementsToTrim));
}

void Polynomial::Print(const std::string& name, const bool printAsHexadecimal, const bool printSize) const
{
    // Lambda for comma separated printing
    auto printFormatted = [printComma = false](const RSWord& value, const bool printAsHex) mutable
    {
        if(printComma)
            std::cout << ", ";
        
        if(printAsHex)
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int64_t>(value) << std::dec;
        else
            std::cout << static_cast<int64_t>(value);
        
        printComma = true;
    };
    
    std::cout << name;
    
    if(printSize)
        std::cout << " (size=" << m_NumOfCoefficients << "): [ ";
    else
        std::cout << ": [ ";
    
    for(const RSWord i : m_Coefficients)
        printFormatted(i, printAsHexadecimal);
    
    std::cout << " ]" << std::endl;
}

void Polynomial::PrintAsASCIICharacters(const std::string& name, const bool printSize) const
{
    std::cout << name;
    
    if(printSize)
        std::cout << " (size=" << m_NumOfCoefficients << "): [ ";
    else
        std::cout << ": [ ";
    
    for(const RSWord i : m_Coefficients)
        std::cout << static_cast<char>(i);
    
    std::cout << " ]" << std::endl;
}
