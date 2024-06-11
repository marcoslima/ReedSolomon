//
//  Polynomial.hpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#ifndef Polynomial_hpp
#define Polynomial_hpp

namespace ReedSolomon
{
class Polynomial
{
private:
    uint16_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
public:
    //Polynomial();
    Polynomial(const std::vector<RSWord>& coefficients);
    Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients);
};
};

#endif /* Polynomial_hpp */
