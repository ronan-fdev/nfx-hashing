/*
 * MIT License
 *
 * Copyright (c) 2025 nfx
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file Algorithms.h
 * @brief Low-level hash algorithm primitives and mixing functions
 * @details Provides core hash building blocks including Larson, FNV-1a, CRC32-C,
 *          seed mixing, and hash combination operations for use across hash implementations
 */

#pragma once

#include <cstdint>

#include "Concepts.h"
#include "Constants.h"

namespace nfx::hashing
{
	//=====================================================================
	// Low - level hash building blocks
	//=====================================================================

	//----------------------------------------------
	// Hash algorithm primitives
	//----------------------------------------------

	/**
	 * @brief Larson multiplicative hash function: 37 * hash + ch
	 * @details Simple hash by Paul Larson, provided for benchmarking.
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t
	 * @param hash Current hash value to update
	 * @param ch Character (byte) to incorporate into the hash
	 * @return Updated hash value after incorporating the character
	 * @see Paul Larson, "The Art of Computer Programming"
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <Hash32or64 HashType = uint32_t>
	[[nodiscard]] inline constexpr HashType larson( HashType hash, uint8_t ch ) noexcept;

	/**
	 * @brief Computes one step of the FNV-1a hash function.
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t
	 * @tparam FnvPrime The FNV prime constant for mixing (default: FNV_PRIME_32 for 32-bit, FNV_PRIME_64 for 64-bit)
	 * @param[in] hash The current hash value.
	 * @param[in] ch The character (byte) to incorporate into the hash.
	 * @return The updated hash value.
	 * @see https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <Hash32or64 HashType = uint32_t, uint64_t FnvPrime = ( sizeof( HashType ) == 4 ? constants::FNV_PRIME_32 : constants::FNV_PRIME_64 )>
	[[nodiscard]] inline constexpr HashType fnv1a( HashType hash, uint8_t ch ) noexcept;

	/**
	 * @brief Computes one step of the CRC32-C hash function with runtime hardware acceleration.
	 * @param[in] hash The current hash value.
	 * @param[in] ch The character (byte) to incorporate into the hash.
	 * @return The updated hash value.
	 * @details This function uses SSE4.2 hardware instructions (_mm_crc32_u8) when available.
	 *          Falls back to crc32cSoft() if CPU doesn't support SSE4.2.
	 *
	 * @warning **Compiler flags required for hardware acceleration:**
	 *          - GCC/Clang: `-march=native` or `-msse4.2`
	 *          - MSVC: `/arch:AVX` or `/arch:AVX2`
	 *          Without these flags, the compiler won't emit SSE4.2 instructions even if
	 *          the CPU supports them, resulting in software fallback performance.
	 *
	 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
	 * @see https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_crc32_u8
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] inline uint32_t crc32c( uint32_t hash, uint8_t ch ) noexcept;

	/**
	 * @brief Software implementation of CRC32-C (Castagnoli) hash function
	 * @param[in] hash The current hash value.
	 * @param[in] ch The character (byte) to incorporate into the hash.
	 * @return The updated hash value.
	 * @details Pure software implementation using reflected polynomial (0x82F63B78).
	 *          Produces identical results to SSE4.2 hardware instructions.
	 *          Useful for testing, benchmarking, or when hardware acceleration is unavailable.
	 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	[[nodiscard]] inline constexpr uint32_t crc32cSoft( uint32_t hash, uint8_t ch ) noexcept;

	//----------------------------------------------
	// Seed and bit mixing
	//----------------------------------------------

	/**
	 * @brief Mixes a seed value with a hash using bit-mixing and multiplicative hashing
	 * @details Combines a seed and hash value using Thomas Wang's bit-mixing algorithm
	 *          followed by multiplicative hashing to produce a well-distributed output
	 *          value within a specified range. The result is deterministic and suitable
	 *          for hash table probing, double hashing, or any seed-based rehashing scheme.
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t
	 * @tparam MixConstant Multiplicative hashing constant
	 * @param seed Additional entropy source to mix with the hash
	 * @param hash Primary hash value to be mixed
	 * @param size Output range - must be a power of 2
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 * @return Mixed hash value in range [0, size-1]
	 */
	template <Hash32or64 HashType = uint32_t, uint64_t MixConstant = constants::SEED_MIX_MULTIPLIER_64>
	[[nodiscard]] inline constexpr HashType seedMix( HashType seed, HashType hash, uint64_t size ) noexcept;

	//----------------------------------------------
	// Hash combination
	//----------------------------------------------

	/**
	 * @brief Combines two hash values using FNV-1a mixing.
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t
	 * @param[in] existingHash The current accumulated hash value.
	 * @param[in] newHash The new hash value to combine.
	 * @param[in] prime The FNV prime constant for mixing.
	 * @details Uses XOR followed by multiplication for optimal bit mixing.
	 * @return Combined hash value with good distribution properties.
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <Hash32or64 HashType = uint32_t>
	[[nodiscard]] inline constexpr HashType combine( HashType existingHash, HashType newHash, HashType prime ) noexcept;

	/**
	 * @brief Combines two hash values using Boost hash_combine with MurmurHash3 finalizer
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t
	 * @param[in] existingHash The current accumulated hash value.
	 * @param[in] newHash The new hash value to combine.
	 * @details Hybrid algorithm combining Boost's hash_combine formula with MurmurHash3 finalizer.
	 *
	 *          **32-bit algorithm:**
	 *          - Uses golden ratio constant (φ = 0x9E3779B9) for uniform distribution
	 *          - Simpler mixing suitable for 32-bit hash values
	 *
	 *          **64-bit algorithm:**
	 *          - Phase 1: Boost hash_combine formula with golden ratio constant
	 *          - Phase 2: MurmurHash3 triple avalanche finalization
	 *          - Ensures single-bit changes affect ~50% of output bits
	 *
	 *          **Performance:** O(1) with excellent collision resistance and uniform distribution.
	 * @return Combined hash value with strong collision resistance and uniform distribution.
	 * @see https://github.com/aappleby/smhasher/wiki/MurmurHash3
	 * @see https://www.boost.org/doc/libs/1_89_0/boost/hash2/legacy/murmur3.hpp
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 */
	template <Hash32or64 HashType = uint32_t>
	[[nodiscard]] inline constexpr HashType combine( HashType existingHash, HashType newHash ) noexcept;
} // namespace nfx::hashing

#include "nfx/detail/hashing/Algorithms.inl"
