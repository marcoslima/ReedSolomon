//
//  main.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#include <iostream>

#include "Utils.hpp"
#include "GaloisField.hpp"

using namespace ReedSolomon;

int main()
{
    std::cout << "Reed-Solomon error correction" << std::endl;
    
    GaloisField(8); // GF(2^8)
    
    return 0;
}
