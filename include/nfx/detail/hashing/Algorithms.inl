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
 * @file Algorithms.inl
 * @brief Implementation of low-level hash primitives and mixing functions
 * @details Implements Larson, fnv1a, crc32c, seedMix, and combine for use in higher-level hash APIs.
 */

#if defined( __GNUC__ ) || defined( __clang__ )
#	include <cpuid.h>
#endif
#if defined( _MSC_VER )
#	include <intrin.h>
#	include <nmmintrin.h>
#endif

#include <array>

namespace nfx::hashing
{
	//=====================================================================
	// Low - level hash building blocks
	//=====================================================================

	namespace internal
	{
		constexpr int CPUID_FEATURE_INFO_LEAF = 1;
		constexpr int ECX_SSE42_BIT = 20;

		//----------------------------------------------
		// SSE4.2 Detection
		//----------------------------------------------

		inline bool hasSse42Support() noexcept
		{
			static const bool s_hasSse42 = []() {
				bool hasSupport = false;
#if defined( __GNUC__ ) || defined( __clang__ )
				unsigned int eax, ebx, ecx, edx;
				if ( __get_cpuid( internal::CPUID_FEATURE_INFO_LEAF, &eax, &ebx, &ecx, &edx ) )
				{
					hasSupport = ( ecx & ( 1 << internal::ECX_SSE42_BIT ) ) != 0; // ECX bit 20 = SSE4.2
				}
#elif defined( _MSC_VER )
				std::array<int, 4> cpuInfo{};
				__cpuid( cpuInfo.data(), internal::CPUID_FEATURE_INFO_LEAF );
				hasSupport = ( cpuInfo[2] & ( 1 << internal::ECX_SSE42_BIT ) ) != 0; // ECX bit 20 = SSE4.2
#endif
				return hasSupport;
			}();

			return s_hasSse42;
		}
	} // namespace internal

	//----------------------------------------------
	// Hash algorithm primitives
	//----------------------------------------------

	template <Hash32or64 HashType>
	inline constexpr HashType larson( HashType hash, uint8_t ch ) noexcept
	{
		return 37 * hash + ch;
	}

	template <Hash32or64 HashType, uint64_t FnvPrime>
	inline constexpr HashType fnv1a( HashType hash, uint8_t ch ) noexcept
	{
		hash ^= ch;		  // XOR byte into hash first
		hash *= FnvPrime; // Then multiply by prime

		return hash;
	}

	inline uint32_t crc32c( uint32_t hash, uint8_t ch ) noexcept
	{
		if ( internal::hasSse42Support() )
		{
#if defined( __GNUC__ ) || defined( __clang__ )
			return __builtin_ia32_crc32qi( hash, ch );
#elif defined( _MSC_VER )
			return _mm_crc32_u8( hash, ch );

#else
			// Compiler doesn't support intrinsics, fall back to software
			return crc32cSoft( hash, ch );
#endif
		}
		else
		{
			// CPU doesn't support SSE4.2, use software implementation
			return crc32cSoft( hash, ch );
		}
	}

	inline constexpr uint32_t crc32cSoft( uint32_t hash, uint8_t ch ) noexcept
	{
		// Software implementation of CRC32-C (Castagnoli) matching SSE4.2 _mm_crc32_u8
		// Polynomial: 0x1EDC6F41 (x^32 + x^28 + x^27 + x^26 + x^25 + x^23 + x^22 + ...)
		constexpr uint32_t polynomial = 0x82F63B78;
		uint32_t crc = hash ^ ch;
		for ( int i = 0; i < 8; ++i )
		{
			crc = ( crc >> 1 ) ^ ( ( crc & 1 )
										 ? polynomial
										 : 0 );
		}
		return crc;
	}

	//----------------------------------------------
	// Seed and bit mixing
	//----------------------------------------------

	template <Hash32or64 HashType, uint64_t MixConstant>
	inline constexpr HashType seedMix( HashType seed, HashType hash, uint64_t size ) noexcept
	{
		if constexpr ( sizeof( HashType ) == 4 ) // 32-bit
		{
			uint32_t x = seed + hash;
			x ^= x >> 12; // Thomas Wang's bit-mixing: spread high bits to low positions
			x ^= x << 25; // Fold low bits back to high positions for avalanche effect
			x ^= x >> 27; // Final avalanche step ensures all bits influence result

			return static_cast<HashType>( ( x * MixConstant ) & ( size - 1 ) );
		}
		else // 64-bit
		{
			uint64_t x = seed + hash;
			x ^= x >> 33;						   // MurmurHash3 avalanche: spread high bits
			x *= constants::MURMUR3_MULTIPLIER_C1; // First multiplicative mixing
			x ^= x >> 33;						   // Second avalanche round
			x *= constants::MURMUR3_MULTIPLIER_C2; // Second multiplicative mixing
			x ^= x >> 33;						   // Final avalanche ensures full bit mixing

			return static_cast<HashType>( ( x * MixConstant ) & ( size - 1 ) );
		}
	}

	//----------------------------------------------
	// Hash combination
	//----------------------------------------------

	template <Hash32or64 HashType>
	inline constexpr HashType combine( HashType existingHash, HashType newHash, HashType prime ) noexcept
	{
		// FNV-1a style combination: XOR then multiply
		existingHash ^= newHash;
		existingHash *= prime;

		return existingHash;
	}

	template <Hash32or64 HashType>
	inline constexpr HashType combine( HashType existingHash, HashType newHash ) noexcept
	{
		if constexpr ( sizeof( HashType ) == 4 ) // 32-bit
		{
			// Boost hash_combine for 32-bit
			existingHash ^= newHash + constants::GOLDEN_RATIO_32 + ( existingHash << 6 ) + ( existingHash >> 2 );
			return existingHash;
		}
		else // 64-bit
		{
			// Boost hash_combine with MurmurHash3 avalanche for 64-bit
			existingHash ^= newHash + constants::GOLDEN_RATIO_64 + ( existingHash << 6 ) + ( existingHash >> 2 );

			// Triple avalanche finalization (MurmurHash3)
			existingHash ^= existingHash >> 33;
			existingHash *= constants::MURMUR3_MULTIPLIER_C1;
			existingHash ^= existingHash >> 33;
			existingHash *= constants::MURMUR3_MULTIPLIER_C2;
			existingHash ^= existingHash >> 33;

			return existingHash;
		}
	}
} // namespace nfx::hashing
