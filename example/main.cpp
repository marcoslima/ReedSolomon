/*
    The zlib License

    Copyright (C) 2024 Marc Schöndorf
 
This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1.  The origin of this software must not be misrepresented; you must not claim
    that you wrote the original software. If you use this software in a product,
    an acknowledgment in the product documentation would be appreciated but is
    not required.

2.  Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

3.  This notice may not be removed or altered from any source distribution.
*/

/*------------------------------------------------------------------*/
/*                                                                  */
/*                      (C) 2024 Marc Schöndorf                     */
/*                            See license                           */
/*                                                                  */
/*  Main.cpp                                                        */
/*  Created: 09.06.2024                                             */
/*------------------------------------------------------------------*/

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
    encoded[11] = 0x00;
    
    // ************************************************
    // Print corrupted message
    std::cout << "----------------------------------------------" << std::endl;
    Utils::PrintVector(encoded, "Corrupted", true, true);
    Utils::PrintVectorAsASCIICharacters(encoded, "ASCII", false);
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Check for corruption
    std::cout << "----------------------------------------------" << std::endl;
    const bool isCorrupted = RS.IsMessageCorrupted(encoded);
    std::cout << "Is corrupted: " << (isCorrupted ? "Yes" : "No") << std::endl << std::endl;
    
    // *******************************                         *****************
    // Decode
    std::vector<uint64_t> erasurePositions = {11};
    std::vector<RSWord> fixedMessage;
    uint64_t numOfErrorsFound = 0;
    const uint64_t numOfErasures = erasurePositions.size();
    
    try {
        fixedMessage = RS.Decode(encoded, &erasurePositions, &numOfErrorsFound); // With known erasure positions
        //fixedMessage = RS.Decode(encoded, nullptr, &numOfErrorsFound); // Unknown error positions
    }
    catch(const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    // ************************************************
    // Print fixed message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Number of errors found by decoding: " << numOfErrorsFound << std::endl;
    std::cout << "Number of erasures known before decoding: " << numOfErasures << std::endl;
    
    Utils::PrintVector(fixedMessage, "Fixed", true, true);
    Utils::PrintVectorAsASCIICharacters(fixedMessage, "ASCII", false);
    
    std::cout << std::endl << std::endl;
        
    return 0;
}
