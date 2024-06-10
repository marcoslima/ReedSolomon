//
//  GaloisField.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 09.06.24.
//

#include <cstdint>

#include "GaloisField.hpp"
#include "Utils.hpp"

using namespace ReedSolomon;

GaloisField::GaloisField()
{
    Utils::IsEven(2);
}

uint8_t GaloisField::Add(uint8_t x, uint8_t y) const
{
    return x ^ y;
}

uint8_t GaloisField::Subtract(uint8_t x, uint8_t y) const
{
    return x ^ y; // Same as addition for binary Galois field (mod 2)
}
