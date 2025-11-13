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
 * @file Hash.h
 * @brief Unified template-based hash function API
 * @details Provides a clean, STL-style hash<T>() interface that delegates to Hasher<HashType, Seed>.
 *          Supports explicit type and hash size specification with optional custom seeding.
 */

#pragma once

#include "nfx/hashing/Concepts.h"
#include "nfx/hashing/Hasher.h"

namespace nfx::hashing
{
	//=====================================================================
	// Unified hash<T, HashType> API
	//=====================================================================

	/**
	 * @brief Hash a value with explicit type and hash size specification
	 * @tparam T The type of value to hash
	 * @tparam HashType The hash output type (uint32_t or uint64_t)
	 * @tparam Seed The seed value for hash initialization (default: FNV_OFFSET_BASIS_32 for 32-bit, FNV_OFFSET_BASIS_64 for 64-bit)
	 * @param value The value to hash
	 * @return Hash value of type HashType
	 * @note This function is marked [[nodiscard]] - the return value should not be ignored
	 *
	 * @details
	 * Unified template-based hash function that provides a clean, STL-style API for hashing values.
	 * The function automatically selects appropriate algorithms based on input type:
	 * - **Strings**: CRC32-C (hardware-accelerated when available)
	 * - **Integers**: Multiplicative hashing (Knuth for 32-bit, Wang for 64-bit)
	 * - **Floating-point**: Bit-representation hashing with normalization
	 * - **Complex types**: Recursive element hashing with combining
	 *
	 * **Default Seed**: FNV offset basis (0x811C9DC5 for 32-bit, 0xCBF29CE484222325 for 64-bit)
	 * provides consistent initialization across the library. For zero-seed behavior,
	 * explicitly specify: `hash<T, HashType, 0>(value)`
	 *
	 * **Internally** delegates to `Hasher<HashType, Seed>` functor for type-specific implementations.
	 *
	 * @code
	 * // Basic usage with default FNV seed
	 * auto h1 = hash<int>(42);                           // uint32_t, seed=0x811C9DC5
	 * auto h2 = hash<std::string>("hello");              // uint32_t, seed=0x811C9DC5
	 * 
	 * // 64-bit hash output
	 * auto h3 = hash<std::string, uint64_t>("hello");    // uint64_t, seed=0xCBF29CE484222325
	 * 
	 * // Custom seeds
	 * auto h4 = hash<int, uint32_t, 0>(42);              // Zero seed (no mixing)
	 * auto h5 = hash<int, uint32_t, 0xDEADBEEF>(42);     // Custom seed
	 * @endcode
	 */
	template <typename T, Hash32or64 HashType = uint32_t, HashType Seed = ( sizeof( HashType ) == 4 ? constants::FNV_OFFSET_BASIS_32 : constants::FNV_OFFSET_BASIS_64 )>
	[[nodiscard]] inline HashType hash( const T& value ) noexcept;
} // namespace nfx::hashing

#include "nfx/detail/hashing/Hash.inl"
