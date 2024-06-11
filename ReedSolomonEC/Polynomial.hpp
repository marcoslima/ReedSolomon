//
//  Polynomial.hpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#ifndef Polynomial_hpp
#define Polynomial_hpp

namespace RS
{
class Polynomial
{
private:
    uint16_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
    GaloisField*        m_GaloisField;
    
public:
    //Polynomial();
    Polynomial(const std::vector<RSWord>& coefficients, GaloisField* const galoisField);
    Polynomial(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* const galoisField);
    
    void Add(const Polynomial* const polynomial);
    void Scale(const RSWord scalar);
    void Multiply(const Polynomial* const polynomial);
    RSWord Evaluate(const RSWord x) const;
    
    uint16_t GetNumberOfCoefficients() const { return m_NumOfCoefficients; }
    const std::vector<RSWord>* const GetCoefficients() const { return &m_Coefficients; }
    
    void SetNew(const std::vector<RSWord>& coefficients, GaloisField* const galoisField);
    void SetNew(const RSWord* const coefficients, const uint16_t& numOfCoefficients, GaloisField* const galoisField);
};
};

#endif /* Polynomial_hpp */
