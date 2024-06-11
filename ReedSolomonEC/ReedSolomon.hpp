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
    
public:
    ReedSolomon(const uint8_t exponent);
    ~ReedSolomon();
};
};

#endif /* ReedSolomon_hpp */
