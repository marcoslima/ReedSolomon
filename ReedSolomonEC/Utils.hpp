//
//  Utils.h
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 10.06.24.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <algorithm>
#include <vector>

namespace RS
{
using RSWord = uint8_t; // The size of a code word

template <typename IntegerType>
concept IsInteger = std::is_integral_v<IntegerType>;

class Utils
{
public:
    Utils() = delete;
    
    template <IsInteger Integer>
    static bool IsEven(const Integer& i) noexcept;

    template <IsInteger Integer>
    static std::vector<uint8_t> IntegerToBytes(const Integer& i);

    template <IsInteger Integer>
    static Integer BytesToInteger(std::vector<uint8_t> bytes, const bool& reverseEndianess = false);
    
    static std::vector<RSWord> StringToRSWordVector(const std::string& str)
    {
        std::vector<RSWord> result(str.length());
        std::copy(str.begin(), str.end(), result.begin());
        
        return result;
    }
    
    template <IsInteger Integer>
    static void PrintVector(const std::vector<Integer>& vec, const std::string& name, const bool printAsHexadecimal = true, const bool printSize = true);
    
    template <IsInteger Integer>
    static void PrintVectorAsASCIICharacters(const std::vector<Integer>& vec, const std::string& name, const bool printSize = true);
};

template <IsInteger Integer>
bool Utils::IsEven(const Integer& i) noexcept
{
    return !(i & 1);
}

template <IsInteger Integer>
std::vector<uint8_t> Utils::IntegerToBytes(const Integer& i)
{
    std::vector<uint8_t> bytes(sizeof(i));
    std::memcpy(bytes.data(), &i, sizeof(i));

    return bytes;
}

template <IsInteger Integer>
Integer Utils::BytesToInteger(std::vector<uint8_t> bytes, const bool& reverseEndianess)
{
    if(sizeof(Integer) > bytes.size())
        throw std::runtime_error("Sizeof vector is to small to hold all bytes of Integer.");
    
    if(reverseEndianess)
        std::reverse(bytes.begin(), bytes.end());
    
    Integer i = 0;
    std::memcpy(&i, bytes.data(), sizeof(Integer));
    
    return i;
}

template <IsInteger Integer>
void Utils::PrintVector(const std::vector<Integer>& vec, const std::string& name, const bool printAsHexadecimal, const bool printSize)
{
    // Lambda for comma separated printing
    auto printFormatted = [printComma = false](const Integer value, const bool printAsHex) mutable
    {
        if(printComma)
            std::cout << ", ";
        
        if(printAsHex)
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int64_t>(value) << std::dec;
        else
            std::cout << static_cast<int64_t>(value);
        
        printComma = true;
    };
    
    std::cout << name;
    
    if(printSize)
        std::cout << " (size=" << vec.size() << "): [ ";
    else
        std::cout << ": [ ";
    
    for(const Integer i : vec)
        printFormatted(i, printAsHexadecimal);
    
    std::cout << " ]" << std::endl;
}

template <IsInteger Integer>
void Utils::PrintVectorAsASCIICharacters(const std::vector<Integer>& vec, const std::string& name, const bool printSize)
{
    std::cout << name;
    
    if(printSize)
        std::cout << " (size=" << vec.size() << "): [ ";
    else
        std::cout << ": [ ";
    
    for(const Integer i : vec)
        std::cout << static_cast<char>(i);
    
    std::cout << " ]" << std::endl;
}
};
 
#endif /* Utils_hpp */
