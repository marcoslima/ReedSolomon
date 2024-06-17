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
    
    const uint32_t bitsPerWord = 8;
    const uint32_t numOfErrorCorrectionSymbols = 5;
    
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");
    const uint64_t messageSize = message.size();

    // Create Reed-Solomon object and encode message
    const ReedSolomon RS(bitsPerWord, numOfErrorCorrectionSymbols);
    std::vector<RSWord> encoded = RS.Encode(message);
    
    Utils::PrintVector(message, "Message", true, false);
    Utils::PrintVectorAsASCIICharacters(message, "Message ASCII", true);
    
    Polynomial p(message, RS.m_GaloisField);
    p.Print("Poly", false);
    p.PrintAsASCIICharacters("PolyASCII", false);
    
    return 0;
    
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
    encoded[11] = 0xFF;
    //encoded[1] = 0x99;
    //encoded[2] = 0x55;
    //encoded[3] = 0x67;
    
    // ************************************************
    // Print corrupted message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Corrupted (n=" << std::dec << encoded.size() << "):" << std::endl;
    for(auto i : encoded)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Check for corruption
    /*std::cout << "----------------------------------------------" << std::endl;
    const bool isCorrupted = rs.IsMessageCorrupted(encoded);
    std::cout << "Is corrupted: " << (isCorrupted ? "Yes" : "No") << std::endl << std::endl;*/
    
    // ************************************************
    // Fix test
    //const Polynomial msgPoly(encoded, rs.m_GaloisField);
    //const Polynomial syndromes = rs.CalculateSyndromes(msgPoly);
    
    // Erasure
    //const Polynomial erasureLocatorPolynomial = rs.CalculateErasureLocatorPolynomial({0, 1});
    //const Polynomial corrected = rs.CorrectErasures(msgPoly, syndromes, {0, 1});
    
    // Error
    //const Polynomial errorLocatorPolynomial = rs.CalculateErrorLocatorPolynomial(syndromes, 5, nullptr, 0);
    
    //const std::vector<uint32_t> errorPositions = rs.FindErrors(&errorLocatorPolynomial, encoded.size());
    
    //std::vector<RSWord> fixedMessage = *corrected.GetCoefficients();
    
    // ************************************************
    // Decode
    std::vector<uint32_t> erasurePositions = {0, 1, 2};
    std::vector<RSWord> fixedMessage;
    
    try {
        //fixedMessage = RS.Decode(encoded, numOfErrorCorrectionSymbols, &erasurePositions);
        fixedMessage = RS.Decode(encoded, numOfErrorCorrectionSymbols, nullptr);
    }
    catch(const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    // ************************************************
    // Print fixed message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Fixed (n=" << fixedMessage.size() << "):" << std::endl;
    for(auto i : fixedMessage)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)i << std::dec << " ";
    std::cout << std::endl << std::endl;
        
    return 0;
}
