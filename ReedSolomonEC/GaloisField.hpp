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
        const uint8_t m_Characteristic = 2;
        uint8_t m_Exponent = 0;
        uint32_t m_Cardinality = 0;
        
    public:
        GaloisField();
        
        uint8_t Add(uint8_t x, uint8_t y) const;
        uint8_t Subtract(uint8_t x, uint8_t y) const;
    };
};

#endif /* GaloisField_hpp */
