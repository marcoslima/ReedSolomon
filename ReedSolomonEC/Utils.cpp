//
//  Utils.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 10.06.24.
//

#include "Utils.hpp"

using namespace ReedSolomon;

bool Utils::IsEven(const uint8_t& i)
{
    return !(i & 1);
}
