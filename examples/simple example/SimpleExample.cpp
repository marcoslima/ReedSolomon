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
/*  Example.cpp                                                     */
/*  Created: 09.06.2024                                             */
/*------------------------------------------------------------------*/

#include "ReedSolomon.hpp"

// Using namespace for ReedSolomon lib
using namespace RS;

// Constants for setting up ReedSolomon
constexpr uint64_t bitsPerWord = 8 * sizeof(RSWord);
constexpr uint64_t numOfErrorCorrectionSymbols = 5;

// Create Reed-Solomon object
const ReedSolomon rs(bitsPerWord, numOfErrorCorrectionSymbols);

void PrintStartupHeader()
{
    std::cout << "**********************************************" << std::endl;
    std::cout << "*    " << rs.GetDescription() << "    *" << std::endl;
    std::cout << "*                                            *" << std::endl;
    std::cout << "*              Simple Example                *" << std::endl;
    std::cout << "*                                            *" << std::endl;
    std::cout << "* Lib version: " << rs.GetVersionString() << "                        *" << std::endl;
    std::cout << "**********************************************" << std::endl << std::endl;
}

int main()
{
    // Print startup header to console
    PrintStartupHeader();
    
    // Create our message to encode. RSWord is the datatype for a symbol (currently uint8_t)
    const std::vector<RSWord> message = Utils::StringToRSWordVector("Hello World!");
    
    // ************************************************
    // Encode our message
    std::vector<RSWord> encoded = rs.Encode(message);
    
    // ************************************************
    // Print original message
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
    // Corrupt message with random letters (errors with unknown position)
    encoded[0] = 'X';
    encoded[1] = 'X';
    
    // Treat this one as erasure with known position
    encoded[10] = 'Q';
    const std::vector<uint64_t> erasurePositions = {10};
    
    // ************************************************
    // Print corrupted message
    std::cout << "----------------------------------------------" << std::endl;
    Utils::PrintVector(encoded, "Corrupted", true, true);
    Utils::PrintVectorAsASCIICharacters(encoded, "ASCII", false);
    std::cout << std::endl << std::endl;
    
    // ************************************************
    // Check for corruption
    std::cout << "----------------------------------------------" << std::endl;
    const bool isCorrupted = rs.IsMessageCorrupted(encoded);
    std::cout << "Is corrupted: " << (isCorrupted ? "Yes" : "No") << std::endl << std::endl << std::endl;
    
    // ************************************************
    // Decode corrupted message and fix it
    std::vector<RSWord> fixedMessage;
    uint64_t numOfErrorsFound = 0;
    const uint64_t numOfErasures = erasurePositions.size();
    
    try {
        fixedMessage = rs.Decode(encoded, &erasurePositions, &numOfErrorsFound); // With known erasure positions
        //fixedMessage = rs.Decode(encoded, nullptr, &numOfErrorsFound); // Unknown erasure/error positions
    }
    catch(const std::runtime_error& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    // ************************************************
    // Print fixed message
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "Number of errors found by decoding: " << numOfErrorsFound << std::endl;
    std::cout << "Number of erasures known before decoding: " << numOfErasures << std::endl << std::endl;
    
    Utils::PrintVector(fixedMessage, "Fixed", true, true);
    Utils::PrintVectorAsASCIICharacters(fixedMessage, "ASCII", false);
    
    std::cout << std::endl;
    
    return 0;
}
