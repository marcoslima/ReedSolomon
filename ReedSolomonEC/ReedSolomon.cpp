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

ReedSolomon::ReedSolomon(const uint8_t exponent)
{
    m_GaloisField = new GaloisField(exponent);
}

ReedSolomon::~ReedSolomon()
{
    delete m_GaloisField;
}

// Create irreducible generator polynomial
Polynomial ReedSolomon::CreateGeneratorPolynomial(const uint16_t numOfECSymbols)
{
    Polynomial result({1}, m_GaloisField);
    Polynomial factor({1, 0}, m_GaloisField);
    
    for (uint16_t i = 0; i < numOfECSymbols; i++)
    {
        //factor[1] = m_GaloisField->GetExponentialTable()->at(i);
        //result.Multiply(&factor);
        
        factor[1] = m_GaloisField->Pow(2, i);
        result.Multiply(&factor);
    }
    
    return result;
}

std::vector<RSWord> ReedSolomon::Encode(const std::vector<RSWord>& message, const uint16_t numOfECSymbols)
{
    const Polynomial generator = CreateGeneratorPolynomial(numOfECSymbols);
    Polynomial messagePolynomial(message, m_GaloisField);
    Polynomial remainder(m_GaloisField);
    
    messagePolynomial.Divide(&generator, nullptr, &remainder);
    
    std::vector<RSWord> result = message;
    result.insert(result.end(), remainder.GetCoefficients()->begin(), remainder.GetCoefficients()->end());
        
    return result;
}
