//
//  Utils.h
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 10.06.24.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <vector>

namespace ReedSolomon
{
template <typename IntegerType>
concept IsInteger = std::is_integral_v<IntegerType>;

class Utils
{
public:
    Utils() = delete;
    
    static bool IsEven(const uint8_t& i);

    template <IsInteger Integer>
    static std::vector<uint8_t> IntegerToBytes(const Integer& i);

    template <IsInteger Integer>
    static Integer BytesToInteger(std::vector<uint8_t> bytes, const bool& reverseEndianess = false);
};

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
    std::memcpy(&i, bytes, sizeof(Integer));
    
    return i;
}
};
 
#endif /* Utils_hpp */
