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
/*  ReedSolomon.hpp                                                 */
/*  Created: 11.06.2024                                             */
/*------------------------------------------------------------------*/


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
