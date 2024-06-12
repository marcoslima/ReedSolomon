//
//  main.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#include <iostream>
#include <iomanip>

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"
#include "ReedSolomon.hpp"

using namespace RS;

int main()
{
    std::cout << "Reed-Solomon error correction" << std::endl << std::endl;
    
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");
    
    const uint8_t numOfErrorCorrectionSymbols = 5;
    ReedSolomon rs(8, numOfErrorCorrectionSymbols);
    const std::vector<RSWord> encoded = rs.Encode(message);
    
    std::cout << "Message: ";
    for(auto i : message)
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << (int)i << " ";
    
    std::cout << std::endl;
    
    std::cout << "Encoded: ";
    for(auto i : encoded)
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << (int)i << " ";
    
    std::cout << std::endl << std::endl;
    
    std::cout << "Is corrupted: " << (rs.IsMessageCorrupted(encoded) ? "Yes" : "No") << std::endl;
    
    return 0;
}
