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
    
    GaloisField*        m_GaloisField;
    
public:
    //Polynomial();
    Polynomial(const std::vector<RSWord>& coefficients, GaloisField* galoisField);
    Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* galoisField);
    
    void Add(const Polynomial* const polynomial);
    void Scale(RSWord scalar);
    void Multiply(const Polynomial* const polynomial);
    RSWord Evaluate(const RSWord x) const;
};
};

#endif /* Polynomial_hpp */
