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
    uint32_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
    const GaloisField*  m_GaloisField = nullptr;
    
public:
    Polynomial(const GaloisField* const galoisField);
    Polynomial(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField);
    Polynomial(const RSWord* const coefficients, const uint32_t numOfCoefficients, const GaloisField* const galoisField);
    
    void Add(const Polynomial* const polynomial);
    void Scale(const RSWord scalar);
    void Multiply(const Polynomial* const polynomial);
    void Divide(const Polynomial* const divisor, Polynomial* const quotient = nullptr, Polynomial* const remainder = nullptr);
    RSWord Evaluate(const RSWord x) const;
    
    void Enlarge(const uint32_t add, const RSWord value = 0);
    
    uint32_t GetNumberOfCoefficients() const { return m_NumOfCoefficients; }
    const std::vector<RSWord>* const GetCoefficients() const noexcept { return &m_Coefficients; }
    
    RSWord operator[] (int index) const { return m_Coefficients[index]; }
    RSWord& operator[] (int index) { return m_Coefficients[index]; }
    
    void SetNew(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField = nullptr);
    void SetNew(const RSWord* const coefficients, const uint32_t numOfCoefficients, const GaloisField* const galoisField = nullptr);
    
    void Print() const;
};
};

#endif /* Polynomial_hpp */
