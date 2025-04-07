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
/*  ChunkExample.cpp                                                */
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
    std::cout << "*              Chunking Example              *" << std::endl;
    std::cout << "*                                            *" << std::endl;
    std::cout << "* Lib version: " << rs.GetVersionString() << "                        *" << std::endl;
    std::cout << "**********************************************" << std::endl << std::endl;
}

int main()
{
    // Print startup header to console
    PrintStartupHeader();
    
    // Use 569 bytes/characters
    std::string longString = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis congue, quam eget imperdiet sollicitudin, libero ipsum tincidunt massa, at vulputate leo erat ut urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse sit amet nisi ex. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Aenean efficitur consequat mauris, ac scelerisque est suscipit sed. Suspendisse tempor venenatis est, a ultricies mi gravida et. Vestibulum laoreet velit lorem, ac sollicitudin tortor rutrum vel. Etiam iaculis velit quis aliquam.";
        
    // Fill large vector
    std::vector<uint8_t> largeVec(501);
    for(uint32_t i = 0; i < 501; i++)
        largeVec[i] = i;
    
    const std::vector<std::vector<uint8_t>> chunks = DataChunker::ChunkData<uint8_t>(largeVec, 100);
    
    const std::vector<uint8_t> assembled = DataChunker::AssembleChunks<uint8_t>(chunks);
    
    return 0;
}
