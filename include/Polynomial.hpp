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
/*  Polynomial.hpp                                                  */
/*  Created: 11.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef Polynomial_hpp
#define Polynomial_hpp

namespace NReedSolomon
{
class Polynomial
{
private:
    const GaloisField*  m_GaloisField = nullptr;
    
    uint64_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
public:
    explicit Polynomial(const GaloisField* const galoisField);
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
    const std::vector<RSWord>* GetCoefficients() const noexcept { return &m_Coefficients; }
    
    RSWord operator[] (uint64_t index) const { return m_Coefficients[index]; }
    RSWord& operator[] (uint64_t index) { return m_Coefficients[index]; }
};
};

#endif /* Polynomial_hpp */
