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
    uint64_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
    const GaloisField*  m_GaloisField = nullptr;
    
public:
    Polynomial(const GaloisField* const galoisField);
    Polynomial(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField);
    Polynomial(const RSWord* const coefficients, const uint64_t numOfCoefficients, const GaloisField* const galoisField);
    
    void SetNew(const std::vector<RSWord>& coefficients, const GaloisField* const galoisField = nullptr);
    void SetNew(const RSWord* const coefficients, const uint64_t numOfCoefficients, const GaloisField* const galoisField = nullptr);
    
    void Clear();
    void Print(const std::string& name = "Polynomial", const bool printAsHexadecimal = true, const bool printSize = true) const;
    void PrintAsASCIICharacters(const std::string& name = "Polynomial", const bool printSize = true) const;
    
    // Operations changing object state
    void Add(const Polynomial* const polynomial);
    void Scale(const RSWord scalar);
    void Multiply(const Polynomial* const polynomial);
    void Divide(const Polynomial* const divisor, Polynomial* const quotient_out = nullptr, Polynomial* const remainder_out = nullptr);
    void Reverse();
    
    // Operations not changing object state
    Polynomial operator* (const RSWord scalar) const;
    RSWord Evaluate(const RSWord x) const;
    std::vector<uint64_t> ChienSearch(const uint64_t max) const;
    
    // Change size
    void Enlarge(const uint64_t numElementsToAdd, const RSWord value = 0);
    void TrimEnd(const uint64_t numElementsToTrim);
    void TrimBeginning(const uint64_t numElementsToTrim);
    
    // Getter
    uint64_t GetNumberOfCoefficients() const { return m_NumOfCoefficients; }
    const std::vector<RSWord>* const GetCoefficients() const noexcept { return &m_Coefficients; }
    
    const RSWord operator[] (uint64_t index) const { return m_Coefficients[index]; }
    RSWord& operator[] (uint64_t index) { return m_Coefficients[index]; }
};
};

#endif /* Polynomial_hpp */
