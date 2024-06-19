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
/*  GaloisField.hpp                                                 */
/*  Created: 09.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef GaloisField_hpp
#define GaloisField_hpp

namespace RS
{
class GaloisField
{
private:
    const uint64_t          m_PrimitivePolynomial;
    
    const uint64_t          m_Characteristic = 2;
    const uint64_t          m_Exponent = 0;
    const uint64_t          m_Cardinality = 0;
    
    std::vector<RSWord>     m_ExponentialTable;
    std::vector<RSWord>     m_LogarithmicTable;
    
    void PrecomputeTables();
    
public:
    GaloisField(const uint64_t exponent);
    
    RSWord Add(const RSWord x, const RSWord y) const noexcept;
    RSWord Subtract(const RSWord x, const RSWord y) const noexcept;
    RSWord Multiply(const RSWord x, const RSWord y) const;
    RSWord Divide(const RSWord x, const RSWord y) const;
    RSWord Pow(const RSWord x, const RSWord power) const;
    RSWord Inverse(const RSWord x) const;
    
    const std::vector<RSWord>& GetExponentialTable() const noexcept { return m_ExponentialTable; }
    const std::vector<RSWord>& GetLogarithmicTable() const noexcept { return m_LogarithmicTable; }
    
    uint64_t    GetCharacteristic() const   { return m_Characteristic; }
    uint64_t    GetExponent() const         { return m_Exponent; }
    uint64_t    GetCardinality() const      { return m_Cardinality; }
};
};

#endif /* GaloisField_hpp */
