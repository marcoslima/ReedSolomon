//
//  ReedSolomon.cpp
//  ReedSolomonEC
//
//  Created by Marc Schöndorf on 11.06.24.
//

#include "Utils.hpp"
#include "GaloisField.hpp"
#include "Polynomial.hpp"
#include "ReedSolomon.hpp"

using namespace RS;

ReedSolomon::ReedSolomon(const uint8_t exponent)
{
    m_GaloisField = new GaloisField(exponent);
}

ReedSolomon::~ReedSolomon()
{
    delete m_GaloisField;
}
