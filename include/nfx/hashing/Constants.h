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
 * @file Constants.h
 * @brief Mathematical constants for hash algorithms
 * @details Defines FNV-1a primes, integer hashing multipliers (Knuth/Wang),
 *          hash combining constants (golden ratio, MurmurHash3), and seed mixing parameters
 */

#pragma once

#include <cstdint>

namespace nfx::hashing::constants
{
	//=====================================================================
	// Hash algorithm constants
	//=====================================================================

	//----------------------------------------------
	// FNV-1a hash algorithm constants
	//----------------------------------------------

	/** @brief FNV-1a 32-bit offset basis constant. */
	inline constexpr uint32_t FNV_OFFSET_BASIS_32{ 0x811C9DC5 };

	/** @brief FNV-1a 32-bit prime constant. */
	inline constexpr uint32_t FNV_PRIME_32{ 0x01000193 };

	/** @brief FNV-1a 64-bit offset basis constant. */
	inline constexpr uint64_t FNV_OFFSET_BASIS_64{ 0xCBF29CE484222325ULL };

	/** @brief FNV-1a 64-bit prime constant. */
	inline constexpr uint64_t FNV_PRIME_64{ 0x00000100000001B3ULL };

	//----------------------------------------------
	// Integer hashing constants
	//----------------------------------------------

	/** @brief Knuth's multiplicative constant for 32-bit integer hashing. */
	inline constexpr uint32_t KNUTH_MULTIPLIER_32{ 0x45d9f3b };

	/** @brief Wang's first multiplicative constant for 64-bit integer hashing. */
	inline constexpr uint64_t WANG_MULTIPLIER_64_C1{ 0xbf58476d1ce4e5b9ull };

	/** @brief Wang's second multiplicative constant for 64-bit integer hashing. */
	inline constexpr uint64_t WANG_MULTIPLIER_64_C2{ 0x94d049bb133111ebull };

	//----------------------------------------------
	// Hash combining constants
	//----------------------------------------------

	/** @brief Golden ratio constant for 32-bit hash combining (φ = 2^32 / golden_ratio). */
	inline constexpr uint32_t GOLDEN_RATIO_32{ 0x9e3779b9 };

	/** @brief Golden ratio constant for 64-bit hash combining (φ = 2^64 / golden_ratio). */
	inline constexpr uint64_t GOLDEN_RATIO_64{ 0x9e3779b97f4a7c15ULL };

	/** @brief MurmurHash3 first multiplicative constant for avalanche mixing. */
	inline constexpr uint64_t MURMUR3_MULTIPLIER_C1{ 0xff51afd7ed558ccdULL };

	/** @brief MurmurHash3 second multiplicative constant for avalanche mixing. */
	inline constexpr uint64_t MURMUR3_MULTIPLIER_C2{ 0xc4ceb9fe1a85ec53ULL };

	//----------------------------------------------
	// Seed mixing constants
	//----------------------------------------------

	/** @brief Multiplicative constant for seed mixing. */
	inline constexpr uint64_t SEED_MIX_MULTIPLIER_64{ 0x2545F4914F6CDD1DUL };
} // namespace nfx::hashing::constants
