//
//  Polynomial.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include <cstdint>
#include <vector>

#include "Utils.hpp"
#include "Polynomial.hpp"
//#include "GaloisField.hpp"

using namespace ReedSolomon;

/*Polynomial::Polynomial()
{
}*/

Polynomial::Polynomial(const std::vector<RSWord>& coefficients)
{
    m_Coefficients = coefficients;
    m_NumOfCoefficients = static_cast<uint16_t>(m_Coefficients.size());
}

Polynomial::Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients)
{
    if(numOfCoefficients < 1)
        throw std::invalid_argument("Number of coefficients must be greater than zero.");
    
    m_NumOfCoefficients = numOfCoefficients;
    std::memcpy(m_Coefficients.data(), coefficients, sizeof(RSWord) * m_NumOfCoefficients);
}
