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
/*  ReedSolomonImpl.cpp                                             */
/*  Created: 11.06.2024                                             */
/*------------------------------------------------------------------*/

#include "ReedSolomon.hpp"
#include <ranges>
using namespace NReedSolomon;

ReedSolomon::ReedSolomon(const uint64_t bitsPerWord, const uint64_t numOfErrorCorrectingSymbols)
    : m_BitsPerWord(bitsPerWord)
    , m_NumOfErrorCorrectingSymbols(numOfErrorCorrectingSymbols)
{
    if(numOfErrorCorrectingSymbols < 1)
        throw std::invalid_argument("Number of error correction symbols must be greater than zero.");
    
    m_GaloisField = new GaloisField(m_BitsPerWord);
    m_GeneratorPolynomial = new Polynomial({1}, m_GaloisField);
    
    CreateGeneratorPolynomial();
}
 
ReedSolomon::ReedSolomon(const ReedSolomon& other)
    : m_BitsPerWord(other.m_BitsPerWord)
    , m_NumOfErrorCorrectingSymbols(other.m_NumOfErrorCorrectingSymbols)
{
    m_GaloisField = new GaloisField(m_BitsPerWord);
    m_GeneratorPolynomial = new Polynomial(m_GaloisField);
    if (other.m_GeneratorPolynomial == nullptr)
        throw std::invalid_argument("Generator polynomial cannot be nullptr.");
    *m_GeneratorPolynomial = *other.m_GeneratorPolynomial;
}

ReedSolomon::~ReedSolomon()
{
    delete m_GaloisField;
    delete m_GeneratorPolynomial;
}

// Create irreducible generator polynomial
void ReedSolomon::CreateGeneratorPolynomial() const
{
    Polynomial factor({1, 0}, m_GaloisField);
    
    for (uint64_t i = 0; i < m_NumOfErrorCorrectingSymbols; i++)
    {
        factor[1] = m_GaloisField->GetExponentialTable()[i];
        m_GeneratorPolynomial->Multiply(&factor);
    }
}

std::vector<RSWord> ReedSolomon::Encode(const std::vector<RSWord>& message) const
{
    if(message.empty())
        throw std::invalid_argument("Cannot encode empty message.");
    
    Polynomial messagePolynomial(message, m_GaloisField);
    Polynomial remainder(m_GaloisField);
    
    messagePolynomial.Enlarge(m_NumOfErrorCorrectingSymbols);
    messagePolynomial.Divide(m_GeneratorPolynomial, nullptr, &remainder);
    
    std::vector<RSWord> result(messagePolynomial.GetNumberOfCoefficients());
    std::ranges::copy(message, result.begin());
    
    // Append remainder to result (remainder are the error correction symbols)
    std::ranges::copy(*remainder.GetCoefficients(), result.begin() + static_cast<coef_diff_type>(message.size()));
    
    return result;
}

Polynomial ReedSolomon::CalculateSyndromes(const Polynomial& message) const
{
    std::vector<RSWord> tmp(m_NumOfErrorCorrectingSymbols + 1);
    tmp[m_NumOfErrorCorrectingSymbols] = 0; // Padding
    
    for(uint64_t i = 0; i < m_NumOfErrorCorrectingSymbols; i++)
        tmp[m_NumOfErrorCorrectingSymbols - i - 1] = message.Evaluate(m_GaloisField->GetExponentialTable()[i]);

    return {tmp, m_GaloisField};
}

Polynomial ReedSolomon::CalculateForneySyndromes(const Polynomial& syndromes, const std::vector<uint64_t>* const erasurePositions, const uint64_t n) const
{
    Polynomial forneySyndromes = syndromes;
    forneySyndromes.TrimEnd(1);
    
    if(erasurePositions)
    {
        for(const uint64_t i : *erasurePositions)
        {
            const RSWord reverse = static_cast<RSWord>(n) - i - 1;
            const RSWord x = m_GaloisField->GetExponentialTable()[reverse];
            
            for(int64_t j = static_cast<int64_t>(forneySyndromes.GetNumberOfCoefficients()) - 2; j >= 0; j--)
            {
                const RSWord tmp = m_GaloisField->Multiply(forneySyndromes[j + 1], x);
                forneySyndromes[j + 1] = tmp ^ forneySyndromes[j];
            }
        }
    }
    
    return forneySyndromes;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool ReedSolomon::CheckSyndromes(const Polynomial& syndromes) const // NOLINT(*-convert-member-functions-to-static)
{
    for(uint64_t i = 0; i < syndromes.GetNumberOfCoefficients(); i++)
    {
        if(syndromes[i] != 0)
            return false;
    }
    
    return true;
}

bool ReedSolomon::IsMessageCorrupted(const std::vector<RSWord>& message) const
{
    const Polynomial msg(message, m_GaloisField);
    const Polynomial syndromes = CalculateSyndromes(msg);
    
    return !CheckSyndromes(syndromes);
}

Polynomial ReedSolomon::CalculateErasureLocatorPolynomial(const std::vector<uint64_t>& erasurePositions) const
{
    Polynomial erasureLocator({1}, m_GaloisField);
    Polynomial factor({0, 1}, m_GaloisField);

    for(const uint64_t i : erasurePositions)
    {
        factor[0] = m_GaloisField->GetExponentialTable()[i];
        erasureLocator.Multiply(&factor);
    }
    
    return erasureLocator;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
Polynomial ReedSolomon::CalculateErrorEvaluatorPolynomial(const Polynomial& syndromes, const Polynomial& erasureLocatorPolynomial, const uint64_t n) const // NOLINT(*-convert-member-functions-to-static)
{
    Polynomial result = syndromes;
    result.Multiply(&erasureLocatorPolynomial);
    result.TrimBeginning(result.GetNumberOfCoefficients() - n);
    
    return result;
}

Polynomial ReedSolomon::CorrectErasures(const Polynomial& message, const Polynomial& syndromes, const std::vector<uint64_t>& erasurePositions) const
{
    // Convert position to coefficient degree
    std::vector<uint64_t> coefficientPosition(erasurePositions.size());
    
    for(uint64_t i = 0; i < erasurePositions.size(); i++)
        coefficientPosition[i] = message.GetNumberOfCoefficients() - erasurePositions[i] - 1;
    
    const Polynomial erasureLocator = CalculateErasureLocatorPolynomial(coefficientPosition);
    const Polynomial errorEvaluator = CalculateErrorEvaluatorPolynomial(syndromes, erasureLocator, erasureLocator.GetNumberOfCoefficients());
        
    std::vector<RSWord> errorPositions(coefficientPosition.size());
    for(uint64_t i = 0; i < errorPositions.size(); i++)
        errorPositions[i] = m_GaloisField->GetExponentialTable()[coefficientPosition[i]];
    
    // Forney algorithm
    Polynomial errorMagnitude(nullptr, message.GetNumberOfCoefficients(), m_GaloisField);
    for(uint64_t i = 0; i < errorPositions.size(); i++)
    {
        const uint64_t index = m_GaloisField->GetCardinality() - 1 - coefficientPosition[i];
        const RSWord Xi = m_GaloisField->GetExponentialTable()[index];
        RSWord errorLocatorPrime = 1;
        
        for(uint64_t j = 0; j < errorPositions.size(); j++)
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

Polynomial ReedSolomon::CalculateErrorLocatorPolynomial(const Polynomial& syndromes, const uint64_t n, const Polynomial* const erasureLocatorPolynomial, const uint64_t erasureCount) const
{
    Polynomial errorLocations({1}, m_GaloisField);
    Polynomial oldLocations({1}, m_GaloisField);
    Polynomial tmp(m_GaloisField);
    
    if(erasureLocatorPolynomial)
    {
        errorLocations = *erasureLocatorPolynomial;
        oldLocations = *erasureLocatorPolynomial;
    }

    //if(syndromes.GetNumberOfCoefficients() > n)
    //    syndromeShift = syndromes.GetNumberOfCoefficients() - n;

    // for(int64_t i = n - (erasureCount - 1); i >= 0; i--)
    // "erasureCount - 1": "-1" removed since iota is already exclusive.
    for (const auto i : std::ranges::reverse_view(std::views::iota(0U, n - erasureCount)))
    {
        constexpr int64_t syndromeShift = 0;
        const uint64_t k = i + syndromeShift + erasureCount;
        RSWord delta = syndromes[k];
        
        for(uint64_t j = 1; j < errorLocations.GetNumberOfCoefficients(); j++)
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
    uint64_t leadingZeros = 0;
    while(errorLocations[leadingZeros] == 0)
        leadingZeros++;
    
    errorLocations.TrimBeginning(leadingZeros);

    const uint64_t numErrors = errorLocations.GetNumberOfCoefficients() - 1;
    if(numErrors * 2 - erasureCount > n)
        throw std::runtime_error("Too many errors to correct.");
    
    return errorLocations;
}

std::vector<uint64_t> ReedSolomon::FindErrors(const Polynomial &errorLocatorPolynomial,
                                              const uint64_t messageLength) const
{
    std::vector<uint64_t> result;
    
    const uint64_t numErrors = errorLocatorPolynomial.GetNumberOfCoefficients() - 1;
    Polynomial reverseErrorLocator = errorLocatorPolynomial;
    reverseErrorLocator.Reverse();
    
    if(errorLocatorPolynomial.GetNumberOfCoefficients() > 2)
    {
        result = reverseErrorLocator.ChienSearch(messageLength);
    }
    else if(errorLocatorPolynomial.GetNumberOfCoefficients() == 2)
    {
        const uint64_t index = m_GaloisField->Divide(errorLocatorPolynomial[0], errorLocatorPolynomial[1]);
        result.push_back(m_GaloisField->GetLogarithmicTable()[index]);
    }
    /*else if(errorLocatorPolynomial.GetNumberOfCoefficients() == 1)
    {
        assert(0);
        throw std::logic_error("LOGIC ERROR: Unimplemented");
    }*/
    
    if(result.size() != numErrors)
        throw std::runtime_error("Chien search found too many or to few errors for the erasure locator polynomial.");
    
    for(unsigned long & i : result)
    {
        if(i >= messageLength)
            throw std::runtime_error("Unexpected error while searching errors in message.");
        
        i = messageLength - i - 1;
    }
    
    return result;
}

std::vector<RSWord> ReedSolomon::Decode(const std::vector<RSWord>& data, const std::vector<uint64_t>* const erasurePositions, uint64_t* const numOfErrorsFound) const
{
    if(numOfErrorsFound)
        *numOfErrorsFound = 0;
    
    if(data.empty())
        throw std::invalid_argument("Data to be decoded cannot have length zero.");
    
    Polynomial messagePolynomial(data, m_GaloisField);
    
    // Do we know the erasure positions?
    if(erasurePositions)
    {
        if(erasurePositions->size() > m_NumOfErrorCorrectingSymbols)
            throw std::runtime_error("Too many erasures to be corrected.");
        
        // Zero out erasure positions in message
        for(const uint64_t i : *erasurePositions)
            messagePolynomial[i] = 0;
    }
    
    const Polynomial syndromes = CalculateSyndromes(messagePolynomial);
    
    // Is message corrupted?
    if(!CheckSyndromes(syndromes))
    {
        // Repair
        const Polynomial forneySyndromes = CalculateForneySyndromes(syndromes, erasurePositions, data.size());
        const Polynomial errorLocator = CalculateErrorLocatorPolynomial(forneySyndromes, m_NumOfErrorCorrectingSymbols, nullptr, erasurePositions ? erasurePositions->size() : 0);
        
        std::vector<uint64_t> errorPositions = FindErrors(errorLocator, data.size());
        
        if(numOfErrorsFound)
            *numOfErrorsFound = errorPositions.size();
        
        if(errorPositions.empty() && (!erasurePositions || erasurePositions->empty()))
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
    std::vector<RSWord> result(data.size() - m_NumOfErrorCorrectingSymbols);
    std::copy(messagePolynomial.GetCoefficients()->begin(), messagePolynomial.GetCoefficients()->end() - static_cast<coef_diff_type>(m_NumOfErrorCorrectingSymbols), result.begin());
    
    return result;
}
