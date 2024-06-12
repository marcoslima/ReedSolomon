//
//  ReedSolomon.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"
#include "ReedSolomon.hpp"

using namespace RS;

ReedSolomon::ReedSolomon(const uint32_t exponent, const uint32_t numOfErrorCorrectingSymbols)
    : m_NumOfErrorCorrectingSymbols(numOfErrorCorrectingSymbols)
{
    if(numOfErrorCorrectingSymbols < 1)
        throw std::invalid_argument("Number of error correction symbols must be greater than zero.");
    
    m_GaloisField = new GaloisField(exponent);
    m_GeneratorPolynomial = new Polynomial({1}, m_GaloisField);
    
    CreateGeneratorPolynomial(m_NumOfErrorCorrectingSymbols);
}

ReedSolomon::~ReedSolomon()
{
    delete m_GaloisField;
}

// Create irreducible generator polynomial
void ReedSolomon::CreateGeneratorPolynomial(const uint8_t numOfECSymbols)
{
    Polynomial factor({1, 0}, m_GaloisField);
    
    for (uint16_t i = 0; i < numOfECSymbols; i++)
    {
        //factor[1] = m_GaloisField->GetExponentialTable()->at(i);
        //m_GeneratorPolynomial->Multiply(&factor);
        
        factor[1] = m_GaloisField->Pow(2, i);
        m_GeneratorPolynomial->Multiply(&factor);
    }
}

std::vector<RSWord> ReedSolomon::Encode(const std::vector<RSWord>& message) const
{
    Polynomial messagePolynomial(message, m_GaloisField);
    Polynomial remainder(m_GaloisField);
    
    messagePolynomial.Enlarge(m_NumOfErrorCorrectingSymbols);
    messagePolynomial.Divide(m_GeneratorPolynomial, nullptr, &remainder);
    
    std::vector<RSWord> result = message;
    result.insert(result.end(), remainder.GetCoefficients()->begin(), remainder.GetCoefficients()->end());
        
    return result;
}

Polynomial ReedSolomon::CalculateSyndromes(const Polynomial& message) const
{
    std::vector<RSWord> tmp(m_NumOfErrorCorrectingSymbols + 1);
    tmp[m_NumOfErrorCorrectingSymbols] = 0; // Padding
    
    for(uint8_t i = 0; i < m_NumOfErrorCorrectingSymbols; i++)
    {
        tmp[m_NumOfErrorCorrectingSymbols - i - 1] = message.Evaluate(m_GaloisField->GetExponentialTable()[i]);
        
        //tmp[i] = message.Evaluate(m_GaloisField->Pow(2, i));
    }
    
    return Polynomial(tmp, m_GaloisField);
}

bool ReedSolomon::CheckSyndromes(const Polynomial& syndromes) const
{
    for(uint8_t i = 0; i < syndromes.GetNumberOfCoefficients(); i++)
    {
        if(syndromes[i] != 0)
            return false;
    }
    
    return true;
}

bool ReedSolomon::IsMessageCorrupted(const std::vector<RSWord>& message)
{
    const Polynomial msg(message, m_GaloisField);
    const Polynomial syndromes = CalculateSyndromes(msg);
    
    return !CheckSyndromes(syndromes);
}
