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
/*  DataChunker.hpp                                                 */
/*  Created: 20.06.2024                                             */
/*------------------------------------------------------------------*/

#ifndef DataChunker_hpp
#define DataChunker_hpp

namespace NReedSolomon
{
class DataChunker
{
public:
    // Static class, non copyable
    DataChunker() = delete;
    DataChunker(const DataChunker&) = delete;
    DataChunker& operator=(const DataChunker&) = delete;
    
    // Chunk data
    template <typename T>
    static std::vector<std::vector<T>> ChunkData(const std::vector<T>& data, uint64_t chunkSize);
    
    // Chunk string
    template <typename T>
    static std::vector<std::vector<T>> ChunkString(const std::string& str, uint64_t chunkSize);
    
    // Assemble chunks to continuous data stream
    template <typename T>
    static std::vector<T> AssembleChunks(const std::vector<std::vector<T>>& chunks);
};

template <typename T>
std::vector<std::vector<T>> DataChunker::ChunkData(const std::vector<T>& data, const uint64_t chunkSize)
{
    if(data.empty())
        throw std::runtime_error("Data size is zero.");
    
    uint64_t numOfChunks = data.size() / chunkSize;
    const uint64_t remainderChunkSize = data.size() % chunkSize;
    
    if(remainderChunkSize > 0)
        ++numOfChunks;
    
    // Chunked data vector
    std::vector<std::vector<T>> allChunks(numOfChunks);
    
    // Allocate memory
    for(uint64_t i = 0; i < numOfChunks; i++)
    {
        if(i == numOfChunks - 1)
            allChunks[i].resize(remainderChunkSize); // Last chunk
        else
            allChunks[i].resize(chunkSize);
    }
    
    // Copy data into chunks
    uint64_t writtenInCurrentChunk = 0;
    uint64_t currentChunkIndex = 0;
    
    for(uint64_t i = 0; i < data.size(); i++)
    {
        allChunks[currentChunkIndex][writtenInCurrentChunk] = data[i];
        writtenInCurrentChunk++;
        
        if(writtenInCurrentChunk >= chunkSize)
        {
            // Current chunk is full, next chunk
            writtenInCurrentChunk = 0;
            currentChunkIndex++;
        }
    }
    
    return allChunks;
}

template <typename T>
std::vector<std::vector<T>> DataChunker::ChunkString(const std::string& str, const uint64_t chunkSize)
{
    if(chunkSize < 1)
        throw std::invalid_argument("Chunk size cannot be smaller than one byte.");
    
    if(str.empty())
        throw std::runtime_error("Data size is zero.");
    
    uint64_t numOfChunks = str.size() / chunkSize;
    const uint64_t remainderChunkSize = str.size() % chunkSize;
    
    if(remainderChunkSize > 0)
        numOfChunks++;
    
    // Chunked data vector
    std::vector<std::vector<T>> allChunks(numOfChunks);
    
    // Allocate memory
    for(uint64_t i = 0; i < numOfChunks; i++)
    {
        if(i == numOfChunks - 1)
            allChunks[i].resize(remainderChunkSize); // Last chunk
        else
            allChunks[i].resize(chunkSize);
    }
    
    // Copy data into chunks
    uint64_t writtenInCurrentChunk = 0;
    uint64_t currentChunkIndex = 0;
    
    for(auto i : str)
    {
        allChunks[currentChunkIndex][writtenInCurrentChunk] = i;
        writtenInCurrentChunk++;
        
        if(writtenInCurrentChunk >= chunkSize)
        {
            // Current chunk is full, next chunk
            writtenInCurrentChunk = 0;
            currentChunkIndex++;
        }
    }
    
    return allChunks;
}

template <typename T>
std::vector<T> DataChunker::AssembleChunks(const std::vector<std::vector<T>>& chunks)
{
    uint64_t totalSize = 0;
    
    // Calculate total size
    for(uint64_t i = 0; i < chunks.size(); i++)
        totalSize += chunks[i].size();
    
    // Allocate memory
    std::vector<T> assembled(totalSize);
    
    // Assemble chunks
    uint64_t totalIndex = 0;
    for(uint64_t chunk = 0; chunk < chunks.size(); chunk++)
    {
        for(uint64_t index = 0; index < chunks[chunk].size(); index++)
        {
            assembled[totalIndex] = chunks[chunk][index];
            totalIndex++;
        }
    }
    
    return assembled;
}
}

#endif /* DataChunker_hpp */
