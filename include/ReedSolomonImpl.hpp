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
/*  ReedSolomonImpl.hpp                                             */
/*  Created: 11.06.2024                                             */
/*------------------------------------------------------------------*/


// ReSharper disable CppMemberFunctionMayBeStatic
#ifndef ReedSolomonImpl_hpp
#define ReedSolomonImpl_hpp

namespace NReedSolomon
{
class ReedSolomon
{
public:
    const uint64_t          m_BitsPerWord = 0;
    const uint64_t          m_NumOfErrorCorrectingSymbols = 0;

    const GaloisField*      m_GaloisField = nullptr;
    Polynomial*             m_GeneratorPolynomial = nullptr;

    // Methods
    void        CreateGeneratorPolynomial() const;

    // Syndromes
    [[nodiscard]] Polynomial  CalculateSyndromes(const Polynomial& message) const;
    Polynomial  CalculateForneySyndromes(const Polynomial& syndromes, const std::vector<uint64_t>*erasurePositions, uint64_t n) const;
    [[nodiscard]] bool        CheckSyndromes(const Polynomial& syndromes) const;

    // Erasure
    [[nodiscard]] Polynomial  CalculateErasureLocatorPolynomial(const std::vector<uint64_t>& erasurePositions) const;
    [[nodiscard]] Polynomial  CalculateErrorEvaluatorPolynomial(const Polynomial& syndromes, const Polynomial& erasureLocatorPolynomial, uint64_t n) const;
    [[nodiscard]] Polynomial  CorrectErasures(const Polynomial& message, const Polynomial& syndromes, const std::vector<uint64_t>& erasurePositions) const;

    // Error
    Polynomial  CalculateErrorLocatorPolynomial(const Polynomial &syndromes, uint64_t n, const Polynomial *erasureLocatorPolynomial, uint64_t erasureCount) const;
    [[nodiscard]] std::vector<uint64_t> FindErrors(const Polynomial &errorLocatorPolynomial,
                                                   uint64_t messageLength) const;

    ReedSolomon(uint64_t bitsPerWord, uint64_t numOfErrorCorrectingSymbols);
    ReedSolomon(const ReedSolomon& other);
    ReedSolomon& operator=(const ReedSolomon& other) = delete;
    const ReedSolomon& operator=(const ReedSolomon& other) const = delete;
    ~ReedSolomon();

    [[nodiscard]] std::vector<RSWord> Encode(const std::vector<RSWord>& message) const;
    std::vector<RSWord> Decode(const std::vector<RSWord>& data, const std::vector<uint64_t>*erasurePositions = nullptr, uint64_t*numOfErrorsFound = nullptr) const;

    [[nodiscard]] bool IsMessageCorrupted(const std::vector<RSWord>& message) const;

    // Version info
    [[nodiscard]] int32_t GetVersionMajor()const{ return RS_VERSION_MAJOR; } // NOLINT(*-convert-member-functions-to-static)
    [[nodiscard]] int32_t GetVersionMinor()const{ return RS_VERSION_MINOR; } // NOLINT(*-convert-member-functions-to-static)
    [[nodiscard]] int32_t GetVersionPatch()const{ return RS_VERSION_PATCH; } // NOLINT(*-convert-member-functions-to-static)
    [[nodiscard]] std::string GetVersionString()const{ return RS_VERSION_STRING; } // NOLINT(*-convert-member-functions-to-static)
    [[nodiscard]] std::string GetDescription()const{ return RS_APP_CMAKE_DESCRIPTION; } // NOLINT(*-convert-member-functions-to-static)
};
}

#endif /* ReedSolomonImpl_hpp */
