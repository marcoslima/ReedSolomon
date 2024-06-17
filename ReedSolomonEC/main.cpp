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
    
    const uint64_t bitsPerWord = 8;
    const uint64_t numOfErrorCorrectionSymbols = 5;
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");

    // Create Reed-Solomon object and encode message
    const ReedSolomon RS(bitsPerWord, numOfErrorCorrectionSymbols);
    std::vector<RSWord> encoded = RS.Encode(message);
    
    // ************************************************
    // Print message
    std::cout << "----------------------------------------------" << std::endl;
    Utils::PrintVector(message, "Message", true, true);
    Utils::PrintVectorAsASCIICharacters(message, "ASCII", false);
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Print encoded message
    std::cout << "----------------------------------------------" << std::endl;
    Utils::PrintVector(encoded, "Encoded", true, true);
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Corrupt message
    encoded[0] = 0xFF;
    encoded[1] = 0xFF;
    //encoded[3] = 0x67;
    encoded[11] = 0x00;
    
    // ************************************************
    // Print corrupted message
    std::cout << "----------------------------------------------" << std::endl;
    Utils::PrintVector(encoded, "Corrupted", true, true);
    Utils::PrintVectorAsASCIICharacters(encoded, "ASCII", false);
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Check for corruption
    /*std::cout << "----------------------------------------------" << std::endl;
    const bool isCorrupted = rs.IsMessageCorrupted(encoded);
    std::cout << "Is corrupted: " << (isCorrupted ? "Yes" : "No") << std::endl << std::endl;*/
    
    // ************************************************
    // Decode
    std::vector<uint64_t> erasurePositions = {11};
    std::vector<RSWord> fixedMessage;
    uint64_t numOfErrorsFound = 0;
    uint64_t numOfErrorsAndErasures = 0;
    
    try {
        fixedMessage = RS.Decode(encoded, &erasurePositions, &numOfErrorsFound, &numOfErrorsAndErasures); // With known erasure positions
        //fixedMessage = RS.Decode(encoded, nullptr, &numOfErrorsFound, &numOfErrorsAndErasures); // Unknown error positions
    }
    catch(const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    // ************************************************
    // Print fixed message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Decoding found " << numOfErrorsFound << " errors." << std::endl;
    std::cout << "In total " << numOfErrorsAndErasures << " errors and erasures." << std::endl << std::endl;
    Utils::PrintVector(fixedMessage, "Fixed", true, true);
    Utils::PrintVectorAsASCIICharacters(fixedMessage, "ASCII", false);
    std::cout << std::endl << std::endl;
        
    return 0;
}
