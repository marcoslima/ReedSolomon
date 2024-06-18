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
    const uint64_t          m_BitsPerWord = 0;
    const uint64_t          m_NumOfErrorCorrectingSymbols = 0;
    
    const GaloisField*      m_GaloisField = nullptr;
    Polynomial*             m_GeneratorPolynomial = nullptr;
        
    // Methods
    void        CreateGeneratorPolynomial();
    
    // Syndromes
    Polynomial  CalculateSyndromes(const Polynomial& message) const;
    Polynomial  CalculateForneySyndromes(const Polynomial& syndromes, const std::vector<uint64_t>* const erasurePositions, const uint64_t n) const;
    bool        CheckSyndromes(const Polynomial& syndromes) const;
    
    // Erasure
    Polynomial  CalculateErasureLocatorPolynomial(const std::vector<uint64_t>& erasurePositions) const;
    Polynomial  CalculateErrorEvaluatorPolynomial(const Polynomial& syndromes, const Polynomial& erasureLocatorPolynomial, const uint64_t n) const;
    Polynomial  CorrectErasures(const Polynomial& message, const Polynomial& syndromes, const std::vector<uint64_t>& erasurePositions) const;
    
    // Error
    Polynomial  CalculateErrorLocatorPolynomial(const Polynomial& syndromes, const int64_t n, const Polynomial* const erasureLocatorPolynomial, const int64_t erasureCount) const;
    const std::vector<uint64_t> FindErrors(const Polynomial& errorLocatorPolynomial, const uint64_t messageLength) const;
    
public:
    ReedSolomon(const uint64_t bitsPerWord, const uint64_t numOfErrorCorrectingSymbols);
    ReedSolomon(const ReedSolomon& other);
    ReedSolomon& operator=(const ReedSolomon& other) = delete;
    const ReedSolomon& operator=(const ReedSolomon& other) const = delete;
    ~ReedSolomon();
    
    std::vector<RSWord> Encode(const std::vector<RSWord>& message) const;
    std::vector<RSWord> Decode(const std::vector<RSWord>& data, const std::vector<uint64_t>* const erasurePositions = nullptr, uint64_t* const numOfErrorsFound = nullptr) const;
    
    bool IsMessageCorrupted(const std::vector<RSWord>& message) const;
};
};

#endif /* ReedSolomon_hpp */
