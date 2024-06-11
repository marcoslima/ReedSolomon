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
private:
    const GaloisField* m_GaloisField = nullptr;
    
    Polynomial CreateGeneratorPolynomial(const uint16_t numOfECSymbols);
    
public:
    ReedSolomon(const uint8_t exponent);
    ~ReedSolomon();
    
    std::vector<RSWord> Encode(const std::vector<RSWord>& message, const uint16_t numOfECSymbols);
};
};

#endif /* ReedSolomon_hpp */
