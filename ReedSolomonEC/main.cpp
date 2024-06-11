//
//  main.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#include <iostream>

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"
#include "ReedSolomon.hpp"

using namespace RS;

int main()
{
    std::cout << "Reed-Solomon error correction" << std::endl << std::endl;
    
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");
    
    ReedSolomon rs(8);
    const std::vector<RSWord> encoded = rs.Encode(message, 5);
    
    std::cout << "Message: ";
    for(auto i : message)
        std::cout << i << " ";
    
    std::cout << std::endl;
    
    std::cout << "Encoded: ";
    for(auto i : encoded)
        std::cout << i << " ";
    
    return 0;
}
