//
//  GaloisField.hpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#ifndef GaloisField_hpp
#define GaloisField_hpp

namespace ReedSolomon
{
class GaloisField
{
private:
    const uint32_t          m_PrimitivePolynomial;
    
    const uint8_t           m_Characteristic = 2;
    uint8_t                 m_Exponent = 0;
    uint32_t                m_Cardinality = 0;
    
    std::vector<RSWord>     m_ExponentialTable;
    std::vector<RSWord>     m_LogarithmicTable;
    
    void PrecomputeTables();
    
public:
    GaloisField(uint8_t exponent);
    
    inline RSWord Add(RSWord x, RSWord y) const;
    inline RSWord Subtract(RSWord x, RSWord y) const;
    //inline RSWord MultiplyWithoutLookupTable(RSWord x, RSWord y) const;
    inline RSWord Multiply(RSWord x, RSWord y) const;
    inline RSWord Divide(RSWord x, RSWord y) const;
    inline RSWord Pow(RSWord x, RSWord power) const;
    inline RSWord Inverse(RSWord x) const;
};
};

#endif /* GaloisField_hpp */
