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
    
    const uint32_t bits = 8;
    const uint32_t numOfErrorCorrectionSymbols = 5;
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");
    const uint64_t messageSize = message.size();

    // Create Reed-Solomon object and encode message
    ReedSolomon rs(bits, numOfErrorCorrectionSymbols);
    std::vector<RSWord> encoded = rs.Encode(message);
    
    // ************************************************
    // Print message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Message (n=" << messageSize << "):" << std::endl;
    for(auto i : message)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Print encoded message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Encoded (n=" << std::dec << encoded.size() << "):" << std::endl;
    for(auto i : encoded)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Corrupt message
    encoded[0] = 0;
    encoded[1] = 0;
    
    // ************************************************
    // Print corrupted message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Corrupted (n=" << std::dec << encoded.size() << "):" << std::endl;
    for(auto i : encoded)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Check for corruption
    std::cout << "----------------------------------------------" << std::endl;
    const bool isCorrupted = rs.IsMessageCorrupted(encoded);
    std::cout << "Is corrupted: " << (isCorrupted ? "Yes" : "No") << std::endl << std::endl;
    
    // ************************************************
    // Fix erasure
    const Polynomial msgPoly(encoded, rs.m_GaloisField);
    const Polynomial syndromes = rs.CalculateSyndromes(msgPoly);
    const Polynomial corrected = rs.CorrectErasures(msgPoly, syndromes, {0, 1});
    
    // ************************************************
    // Print fixed message
    std::vector<RSWord> fixedMessage = *corrected.GetCoefficients();
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Fixed (n=" << fixedMessage.size() << "):" << std::endl;
    for(auto i : fixedMessage)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    std::cout << std::endl << std::endl;
    
    std::cout << std::endl << std::endl;
    
    return 0;
}
