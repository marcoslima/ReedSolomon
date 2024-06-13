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
void ReedSolomon::CreateGeneratorPolynomial(const uint32_t numOfECSymbols)
{
    Polynomial factor({1, 0}, m_GaloisField);
    
    for (uint32_t i = 0; i < numOfECSymbols; i++)
    {
        //factor[1] = m_GaloisField->Pow(2, i);
        factor[1] = m_GaloisField->GetExponentialTable()[i];
        m_GeneratorPolynomial->Multiply(&factor);
    }
}

std::vector<RSWord> ReedSolomon::Encode(const std::vector<RSWord>& message) const
{
    Polynomial messagePolynomial(message, m_GaloisField);
    Polynomial remainder(m_GaloisField);
    
    messagePolynomial.Enlarge(m_NumOfErrorCorrectingSymbols);
    messagePolynomial.Divide(m_GeneratorPolynomial, nullptr, &remainder);
    
    std::vector<RSWord> result(messagePolynomial.GetNumberOfCoefficients());
    std::copy(message.begin(), message.end(), result.begin());
    
    // Append remainder to result (remainder are the EC symbols)
    std::copy(remainder.GetCoefficients()->begin(), remainder.GetCoefficients()->end(), result.begin() + message.size());
    
    return result;
}

Polynomial ReedSolomon::CalculateSyndromes(const Polynomial& message) const
{
    std::vector<RSWord> tmp(m_NumOfErrorCorrectingSymbols + 1);
    tmp[m_NumOfErrorCorrectingSymbols] = 0; // Padding
    
    for(uint32_t i = 0; i < m_NumOfErrorCorrectingSymbols; i++)
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

Polynomial ReedSolomon::CalculateErasureLocatorPolynomial(const std::vector<uint32_t>& erasurePositions) const
{
    Polynomial erasureLocator({1}, m_GaloisField);
    Polynomial factor({0, 1}, m_GaloisField);

    for(uint32_t i : erasurePositions)
    {
        factor[0] = m_GaloisField->GetExponentialTable()[i];
        erasureLocator.Multiply(&factor);
    }
    
    return erasureLocator;
}

Polynomial ReedSolomon::CalculateErrorEvaluatorPolynomial(const Polynomial& syndromes, const Polynomial& erasureLocatorPolynomial, const uint32_t n) const
{
    Polynomial result = syndromes;
    result.Multiply(&erasureLocatorPolynomial);
    result.TrimLeft(result.GetNumberOfCoefficients() - n);
    
    return result;
}

Polynomial ReedSolomon::CorrectErasures(const Polynomial& message, const Polynomial& syndromes, const std::vector<uint32_t>& erasurePositions) const
{
    // Convert position to coefficient degree
    std::vector<uint32_t> coefficientPosition(erasurePositions.size());
    
    for(uint32_t i = 0; i < erasurePositions.size(); i++)
        coefficientPosition[i] = message.GetNumberOfCoefficients() - erasurePositions[i] - 1;
    
    const Polynomial erasureLocator = CalculateErasureLocatorPolynomial(coefficientPosition);
    const Polynomial errorEvaluator = CalculateErrorEvaluatorPolynomial(syndromes, erasureLocator, erasureLocator.GetNumberOfCoefficients());
        
    std::vector<RSWord> errorPositions(coefficientPosition.size());
    for(uint32_t i = 0; i < errorPositions.size(); i++)
        errorPositions[i] = m_GaloisField->GetExponentialTable()[coefficientPosition[i]];
    
    // Forney algorithm
    Polynomial errorMagnitude(nullptr, message.GetNumberOfCoefficients(), m_GaloisField);
    for(uint32_t i = 0; i < errorPositions.size(); i++)
    {
        const uint32_t index = m_GaloisField->GetCardinality() - 1 - coefficientPosition[i];
        RSWord Xi = m_GaloisField->GetExponentialTable()[index];
        RSWord errorLocatorPrime = 1;
        
        for(uint32_t j = 0; j < errorPositions.size(); j++)
        {
            if(j != i)
            {
                const RSWord tmp = 1 ^ m_GaloisField->Multiply(Xi, errorPositions[j]);
                errorLocatorPrime = m_GaloisField->Multiply(errorLocatorPrime, tmp);
            }
        }
        
        if(errorLocatorPrime == 0)
            throw std::runtime_error("Could not find error magnitude.");
        
        const RSWord tmp = errorEvaluator.Evaluate(Xi);
        const RSWord y = m_GaloisField->Multiply(errorPositions[i], tmp);
        
        errorMagnitude[erasurePositions[i]] = m_GaloisField->Divide(y, errorLocatorPrime);
    }
    
    Polynomial result = message;
    result.Add(&errorMagnitude);
    
    return result;
}
