//
//  ReedSolomon.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include <assert.h>

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
    
    // Append remainder to result (remainder are the error correction symbols)
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

Polynomial ReedSolomon::CalculateForneySyndromes(const Polynomial& syndromes, const std::vector<uint32_t>* const erasurePositions, const uint32_t n) const
{
    Polynomial forneySyndromes = syndromes;
    forneySyndromes.TrimEnd(1);
    
    if(erasurePositions)
    {
        for(uint32_t i : *erasurePositions)
        {
            RSWord reverse = static_cast<RSWord>(n) - i - 1;
            RSWord x = m_GaloisField->GetExponentialTable()[reverse];
            
            for(int32_t j = forneySyndromes.GetNumberOfCoefficients() - 2; j >= 0; j--)
            {
                RSWord tmp = m_GaloisField->Multiply(forneySyndromes[j + 1], x);
                forneySyndromes[j + 1] = tmp ^ forneySyndromes[j];
            }
        }
    }
    
    return forneySyndromes;
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
    result.TrimBeginning(result.GetNumberOfCoefficients() - n);
    
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

Polynomial ReedSolomon::CalculateErrorLocatorPolynomial(const Polynomial& syndromes, const int32_t n, const Polynomial* const erasureLocatorPolynomial, const int32_t erasureCount) const
{
    Polynomial errorLocations({1}, m_GaloisField);
    Polynomial oldLocations({1}, m_GaloisField);
    Polynomial tmp(m_GaloisField);
    
    if(erasureLocatorPolynomial)
    {
        errorLocations = *erasureLocatorPolynomial;
        oldLocations = *erasureLocatorPolynomial;
    }
    
    int32_t syndromeShift = 0;
    
    //if(syndromes.GetNumberOfCoefficients() > n)
    //    syndromeShift = syndromes.GetNumberOfCoefficients() - n;
            
    for(int32_t i = n - erasureCount - 1; i >= 0; i--)
    {
        const uint32_t k = i + syndromeShift + erasureCount;
        RSWord delta = syndromes[k];
        
        for(uint32_t j = 1; j < errorLocations.GetNumberOfCoefficients(); j++)
        {
            const RSWord x = errorLocations[errorLocations.GetNumberOfCoefficients() - j - 1];
            const RSWord y = syndromes[k + j];
            delta ^= m_GaloisField->Multiply(x, y);
        }
        
        oldLocations.Enlarge(1);
        
        if(delta != 0)
        {
            if(oldLocations.GetNumberOfCoefficients() > errorLocations.GetNumberOfCoefficients())
            {
                tmp = oldLocations * delta;
                oldLocations = errorLocations * m_GaloisField->Inverse(delta);
                errorLocations = tmp;
            }
            
            tmp = oldLocations * delta;
            errorLocations.Add(&tmp);
        }
    }
    
    // Count leading zeros and trim them
    uint32_t leadingZeros = 0;
    while(errorLocations[leadingZeros] == 0)
        leadingZeros++;
    
    errorLocations.TrimBeginning(leadingZeros);
    
    const int32_t numErrors = errorLocations.GetNumberOfCoefficients() - 1;
    if(numErrors * 2 - erasureCount > n)
        throw std::runtime_error("Too many errors to correct.");
    
    return errorLocations;
}

const std::vector<uint32_t> ReedSolomon::FindErrors(const Polynomial& errorLocatorPolynomial, const uint32_t messageLength) const
{
    std::vector<uint32_t> result;
    
    const uint32_t numErrors = errorLocatorPolynomial.GetNumberOfCoefficients() - 1;
    Polynomial reverseErrorLocator = errorLocatorPolynomial;
    reverseErrorLocator.Reverse();
    
    if(errorLocatorPolynomial.GetNumberOfCoefficients() == 1)
    {
        //assert(0);
        //throw std::logic_error("LOGIC ERROR: Unimplemented");
    }
    else if(errorLocatorPolynomial.GetNumberOfCoefficients() == 2)
    {
        const uint32_t index = m_GaloisField->Divide(errorLocatorPolynomial[0], errorLocatorPolynomial[1]);
        result.push_back(m_GaloisField->GetLogarithmicTable()[index]);
    }
    else
    {
        result = reverseErrorLocator.ChienSearch(messageLength);
    }
    
    if(result.size() != numErrors)
        throw std::runtime_error("Chien search found too many or to few errors for the erasure locator polynomial.");
    
    for(uint32_t i = 0; i < result.size(); i++)
    {
        if(result[i] >= messageLength)
            throw std::runtime_error("Unexpected error while searching errors in message.");
        
        result[i] = messageLength - result[i] - 1;
    }
    
    return result;
}

//const std::vector<uint32_t> ReedSolomon::FindErrors(const Polynomial* const errorLocatorPolynomial, const uint32_t messageLength)
//{
//    const uint32_t numErrors = errorLocatorPolynomial->GetNumberOfCoefficients() - 1;
//    std::vector<uint32_t> errorPositions;
//    
//    // Search roots
//    for(uint32_t i = 0; i < messageLength; i++)
//    {
//        if(errorLocatorPolynomial->Evaluate(m_GaloisField->Pow(2, i)) == 0)
//            errorPositions.push_back(messageLength - 1 - i);
//    }
//    
//    if(errorPositions.size() != numErrors)
//        throw std::runtime_error("Found to many or to few errors for the erasure locator polynomial.");
//    
//    return errorPositions;
//}

std::vector<RSWord> ReedSolomon::Decode(const std::vector<RSWord>& data, const uint32_t numOfErrorCorrectingSymbols, const std::vector<uint32_t>* const erasurePositions) const
{
    if(data.size() == 0)
        throw std::invalid_argument("Data to be decoded cannot have length zero.");
    if(numOfErrorCorrectingSymbols == 0)
        throw std::invalid_argument("Amount of error correcting symbols cannot be zero.");
    
    Polynomial messagePolynomial(data, m_GaloisField);
    
    // Do we know the erasure positions?
    if(erasurePositions)
    {
        if(erasurePositions->size() > numOfErrorCorrectingSymbols)
            throw std::runtime_error("Too many erasures to be corrected.");
        
        // Zero out erasure positions in message
        for(uint32_t i : *erasurePositions)
            messagePolynomial[i] = 0;
    }
    
    const Polynomial syndromes = CalculateSyndromes(messagePolynomial);
    
    // Is message corrupted?
    if(!CheckSyndromes(syndromes))
    {
        // Repair
        const Polynomial forneySyndromes = CalculateForneySyndromes(syndromes, erasurePositions, static_cast<uint32_t>(data.size()));
        const Polynomial errorLocator = CalculateErrorLocatorPolynomial(forneySyndromes, numOfErrorCorrectingSymbols, nullptr, erasurePositions ? static_cast<int32_t>(erasurePositions->size()) : 0);
        
        std::vector<uint32_t> errorPositions = FindErrors(errorLocator, static_cast<uint32_t>(data.size()));
        
        if(errorPositions.size() == 0 && (!erasurePositions || erasurePositions->size() == 0))
            throw std::runtime_error("Unable to locate errors.");
        
        // Append erasure positions to error positions vector
        if(erasurePositions)
        {
            errorPositions.insert(errorPositions.begin(), erasurePositions->begin(), erasurePositions->end());
        }
        
        // Correct errors
        messagePolynomial = CorrectErasures(messagePolynomial, syndromes, errorPositions);
    }
    
    // Cut error correcting symbols from message and return message
    std::vector<RSWord> result(data.size() - numOfErrorCorrectingSymbols);
    std::copy(messagePolynomial.GetCoefficients()->begin(), messagePolynomial.GetCoefficients()->end() - numOfErrorCorrectingSymbols, result.begin());
    
    return result;
}
