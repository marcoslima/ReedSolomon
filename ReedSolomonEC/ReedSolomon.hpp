//
//  ReedSolomon.hpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#ifndef ReedSolomon_hpp
#define ReedSolomon_hpp

namespace RS
{
class ReedSolomon
{
public:
    const uint32_t          m_NumOfErrorCorrectingSymbols = 0;
    
    const GaloisField*      m_GaloisField = nullptr;
    Polynomial*             m_GeneratorPolynomial = nullptr;
        
    // Methods
    void        CreateGeneratorPolynomial(const uint32_t numOfECSymbols);
    
    Polynomial  CalculateSyndromes(const Polynomial& message) const;
    Polynomial  CalculateForneySyndromes(const Polynomial& syndromes, const std::vector<uint32_t>& erasurePositions, const uint32_t n) const;
    bool        CheckSyndromes(const Polynomial& syndromes) const;
    
    // Erasure
    Polynomial  CalculateErasureLocatorPolynomial(const std::vector<uint32_t>& erasurePositions) const;
    Polynomial  CalculateErrorEvaluatorPolynomial(const Polynomial& syndromes, const Polynomial& erasureLocatorPolynomial, const uint32_t n) const;
    Polynomial  CorrectErasures(const Polynomial& message, const Polynomial& syndromes, const std::vector<uint32_t>& erasurePositions) const;
    
    // Error
    Polynomial  CalculateErrorLocatorPolynomial(const Polynomial& syndromes, const uint32_t n, const Polynomial* const erasureLocatorPolynomial, const uint32_t erasureCount) const;
    const std::vector<uint32_t> FindErrors(const Polynomial& errorLocatorPolynomial, const uint32_t messageLength) const;
    
public:
    ReedSolomon(const uint32_t exponent, const uint32_t numOfErrorCorrectingSymbols);
    ~ReedSolomon();
    
    std::vector<RSWord> Encode(const std::vector<RSWord>& message) const;
    std::vector<RSWord> Decode(const std::vector<RSWord>& data, const uint32_t numOfErrorCorrectingSymbols, const std::vector<uint32_t>* const erasurePositions = nullptr) const;
    
    bool IsMessageCorrupted(const std::vector<RSWord>& message);
};
};

#endif /* ReedSolomon_hpp */
