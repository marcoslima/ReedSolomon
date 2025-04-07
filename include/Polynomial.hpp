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
    using coef_diff_type = std::vector<RSWord>::difference_type;
class Polynomial
{
    const GaloisField*  m_GaloisField = nullptr;
    
    uint64_t            m_NumOfCoefficients = 0;
    std::vector<RSWord> m_Coefficients;
    
public:
    explicit Polynomial(const GaloisField*galoisField);
    Polynomial(const std::vector<RSWord>& coefficients, const GaloisField*galoisField);
    Polynomial(const RSWord*coefficients, uint64_t numOfCoefficients, const GaloisField*galoisField);
    
    void SetNew(const std::vector<RSWord>& coefficients, const GaloisField*galoisField = nullptr);
    void SetNew(const RSWord*coefficients, uint64_t numOfCoefficients, const GaloisField*galoisField = nullptr);
    
    void Clear();
    void Print(const std::string& name = "Polynomial", bool printAsHexadecimal = true, bool printSize = true) const;
    void PrintAsASCIICharacters(const std::string& name = "Polynomial", bool printSize = true) const;
    
    // Operations changing object state
    void Add(const Polynomial*polynomial);
    void Scale(RSWord scalar);
    void Multiply(const Polynomial*polynomial);
    void Divide(const Polynomial*divisor, Polynomial*quotient_out = nullptr, Polynomial*remainder_out = nullptr);
    void Reverse();
    
    // Operations not changing object state
    Polynomial operator* (RSWord scalar) const;
    [[nodiscard]] RSWord Evaluate(RSWord x) const;
    [[nodiscard]] std::vector<uint64_t> ChienSearch(uint64_t max) const;
    
    // Change size
    void Enlarge(uint64_t numElementsToAdd, RSWord value = 0);
    void TrimEnd(uint64_t numElementsToTrim);
    void TrimBeginning(uint64_t numElementsToTrim);
    
    // Getter
    [[nodiscard]] uint64_t GetNumberOfCoefficients() const { return m_NumOfCoefficients; }
    [[nodiscard]] const std::vector<RSWord>* GetCoefficients() const noexcept { return &m_Coefficients; }
    
    RSWord operator[] (const uint64_t index) const { return m_Coefficients[index]; }
    RSWord& operator[] (const uint64_t index) { return m_Coefficients[index]; }
};
}

#endif /* Polynomial_hpp */
