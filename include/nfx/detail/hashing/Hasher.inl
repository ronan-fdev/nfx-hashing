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
 * @file Hasher.inl
 * @brief Implementation of STL-compatible Hasher functor and overloads
 * @details Implements Hasher<HashType, Seed> for strings, integers, floats, pointers, enums,
 *          pairs, tuples, arrays, and custom types, using the appropriate hash primitives.
 */

#include <cmath>
#include <cstring>

namespace nfx::hashing
{
	//=====================================================================
	// General-purpose STL-compatible hash functor
	//=====================================================================

	namespace internal
	{
		//----------------------------------------------
		// String types hashing
		//----------------------------------------------

		template <Hash32or64 HashType, HashType Seed>
		inline HashType hashStringView( std::string_view key ) noexcept
		{
			if ( key.empty() )
			{
				// Empty strings always hash to 0, regardless of seed
				// This ensures consistent empty-container identity across all Hasher<> instances
				return 0;
			}

			if constexpr ( sizeof( HashType ) == 4 ) // 32-bit
			{
				HashType hashValue = Seed;

				for ( size_t i = 0; i < key.length(); ++i )
				{
					hashValue = crc32c( hashValue, static_cast<uint8_t>( key[i] ) );
				}

				return hashValue;
			}
			else // 64-bit
			{
				uint32_t low = static_cast<uint32_t>( Seed );
				uint32_t high = static_cast<uint32_t>( Seed >> 32 );

				for ( size_t i = 0; i < key.length(); ++i )
				{
					uint8_t byte = static_cast<uint8_t>( key[i] );
					low = crc32c( low, byte );
					// Compute high half with inverted byte pattern to ensure
					// distinct hash values (prevents identical high/low halves)
					high = crc32c( high, byte ^ 0xFF );
				}

				return ( static_cast<uint64_t>( high ) << 32 ) | low;
			}
		}

		//----------------------------------------------
		// Integer types hashing
		//----------------------------------------------

		template <Hash32or64 HashType, HashType Seed, typename T>
		inline constexpr std::enable_if_t<std::is_integral_v<T>, HashType> hashInteger( T value ) noexcept
		{
			if ( value == 0 )
			{
				return 0;
			}

			/*
			 * Integer hashing using avalanche mixing for uniform bit distribution.
			 *
			 * Purpose: Transform integer input into well-distributed hash values where small
			 * changes in input produce large, unpredictable changes in output (avalanche effect).
			 *
			 * Seed: XORed with input value before hashing for compile-time randomization.
			 *       Default is FNV_OFFSET_BASIS for consistency with hash<T> API.
			 *
			 * 32-bit Algorithm (Knuth-style):
			 * - Mix high/low bits with XOR and right shift to spread bit dependencies
			 * - Multiply by carefully chosen odd constant for mathematical guarantees
			 * - Repeat mixing steps to ensure full avalanche across all bit positions
			 *
			 * 64-bit Algorithm (Wang's method):
			 * - Use different shift amounts (30, 27, 31) to mix bit regions
			 * - Apply two different multiplicative constants for maximum entropy
			 * - Each step ensures bits from different input regions influence output
			 *
			 * Both provide O(1) performance with excellent statistical properties.
			 */

			if constexpr ( sizeof( HashType ) == 4 ) // 32-bit output
			{
				/* 32-bit integer hashing */
				if constexpr ( sizeof( T ) == 8 ) // 64-bit input -> 32-bit output
				{
					// Fold 64-bit value to 32-bit before hashing to preserve high bits
					uint64_t v64 = static_cast<uint64_t>( value ) ^ static_cast<uint64_t>( Seed );
					uint32_t x = static_cast<uint32_t>( v64 ^ ( v64 >> 32 ) );
					x = ( ( x >> 16 ) ^ x ) * constants::KNUTH_MULTIPLIER_32;
					x = ( ( x >> 16 ) ^ x ) * constants::KNUTH_MULTIPLIER_32;
					x = ( x >> 16 ) ^ x;
					return static_cast<HashType>( x );
				}
				else // 32-bit or smaller input
				{
					uint32_t x = static_cast<uint32_t>( value ) ^ static_cast<uint32_t>( Seed );
					x = ( ( x >> 16 ) ^ x ) * constants::KNUTH_MULTIPLIER_32;
					x = ( ( x >> 16 ) ^ x ) * constants::KNUTH_MULTIPLIER_32;
					x = ( x >> 16 ) ^ x;
					return static_cast<HashType>( x );
				}
			}
			else // 64-bit output
			{
				/* 64-bit integer hashing */
				uint64_t x = static_cast<uint64_t>( value ) ^ static_cast<uint64_t>( Seed );
				x = ( x ^ ( x >> 30 ) ) * constants::WANG_MULTIPLIER_64_C1;
				x = ( x ^ ( x >> 27 ) ) * constants::WANG_MULTIPLIER_64_C2;
				x = x ^ ( x >> 31 );

				return static_cast<HashType>( x );
			}
		}
	} // namespace internal

	//----------------------------------------------
	// String type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	inline HashType Hasher<HashType, Seed>::operator()( std::string_view key ) const noexcept
	{
		return internal::hashStringView<HashType, Seed>( key );
	}

	template <Hash32or64 HashType, HashType Seed>
	inline HashType Hasher<HashType, Seed>::operator()( const std::string& key ) const noexcept
	{
		return internal::hashStringView<HashType, Seed>( key );
	}

	template <Hash32or64 HashType, HashType Seed>
	inline HashType Hasher<HashType, Seed>::operator()( const char* key ) const noexcept
	{
		return internal::hashStringView<HashType, Seed>( key );
	}

	//----------------------------------------------
	// Integer type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename TKey>
	inline std::enable_if_t<std::is_integral_v<TKey>, HashType> Hasher<HashType, Seed>::operator()( const TKey& key ) const noexcept
	{
		// Delegate to hashInteger with the same seed
		// This ensures consistent behavior between hash<T>, Hasher<>, and hashInteger
		return internal::hashInteger<HashType, Seed>( key );
	}

	//----------------------------------------------
	// Floating-point type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T>
	inline std::enable_if_t<std::is_floating_point_v<T>, HashType> Hasher<HashType, Seed>::operator()( T value ) const noexcept
	{
		// Normalize: +0.0 and -0.0 should hash the same
		if ( value == 0.0 )
		{
			value = 0.0;
		}
		// Normalize: all NaNs should hash the same
		if ( std::isnan( value ) )
		{
			value = std::numeric_limits<T>::quiet_NaN();
		}

		// Hash the bit representation
		if constexpr ( sizeof( T ) == sizeof( uint32_t ) )
		{
			uint32_t bits;
			std::memcpy( &bits, &value, sizeof( bits ) );

			// Pass seed to hashInteger
			return internal::hashInteger<HashType, Seed>( bits );
		}
		else if constexpr ( sizeof( T ) == sizeof( uint64_t ) )
		{
			uint64_t bits;
			std::memcpy( &bits, &value, sizeof( bits ) );

			// Pass seed to hashInteger - when hashing 64-bit value with 32-bit seed,
			// hashInteger will handle the XOR with appropriate casts (line 129: uint64_t v64 = value ^ Seed)
			return internal::hashInteger<HashType, Seed>( bits );
		}
		else
		{
			// long double or other exotic types
			return static_cast<HashType>( std::hash<T>{}( value ) );
		}
	}

	//----------------------------------------------
	// Pointer type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T>
	inline std::enable_if_t<std::is_pointer_v<T> && !std::is_same_v<T, const char*> && !std::is_same_v<T, char*>, HashType> Hasher<HashType, Seed>::operator()( T ptr ) const noexcept
	{
		// Hash the address as an integer - seed XOR is handled inside hashInteger
		// When hashing 64-bit pointer with 32-bit seed, hashInteger handles the cast (line 129)
		uintptr_t address = reinterpret_cast<uintptr_t>( ptr );
		return internal::hashInteger<HashType, Seed>( address );
	}

	//----------------------------------------------
	// Enum type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename TKey>
	inline std::enable_if_t<std::is_enum_v<TKey>, HashType> Hasher<HashType, Seed>::operator()( const TKey& key ) const noexcept
	{
		// Hash enum by converting to underlying integral type
		using UnderlyingType = std::underlying_type_t<TKey>;

		return ( *this )( static_cast<UnderlyingType>( key ) );
	}

	//----------------------------------------------
	// std::array type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T, size_t N>
	inline HashType Hasher<HashType, Seed>::operator()( const std::array<T, N>& arr ) const noexcept
	{
		// Note: Empty arrays (N=0) will return Seed unchanged (not 0)
		// This differs from empty std::vector which returns 0 after combining size
		HashType result = Seed;
		for ( const auto& elem : arr )
		{
			result = combine( result, ( *this )( elem ) );
		}
		return result;
	}

	//----------------------------------------------
	// std::optional type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T>
	inline HashType Hasher<HashType, Seed>::operator()( const std::optional<T>& opt ) const noexcept
	{
		if ( opt.has_value() )
		{
			// Hash the contained value with a marker bit
			return combine( ( *this )( *opt ), static_cast<HashType>( 1 ) );
		}
		else
		{
			// Hash nullopt with a distinct marker (0 combined with seed)
			return combine( Seed, static_cast<HashType>( 0 ) );
		}
	}

	//----------------------------------------------
	// std::pair type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T1, typename T2>
	inline HashType Hasher<HashType, Seed>::operator()( const std::pair<T1, T2>& p ) const noexcept
	{
		HashType h1 = ( *this )( p.first );
		HashType h2 = ( *this )( p.second );

		return combine<HashType>( h1, h2 );
	}

	//----------------------------------------------
	// std::span type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T, std::size_t Extent>
	inline HashType Hasher<HashType, Seed>::operator()( std::span<T, Extent> sp ) const noexcept
	{
		// Note: Empty spans will return Seed unchanged
		HashType result = Seed;
		for ( const auto& elem : sp )
		{
			result = combine( result, ( *this )( elem ) );
		}

		return result;
	}

	//----------------------------------------------
	// std::tuple type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename... Ts>
	inline HashType Hasher<HashType, Seed>::operator()( const std::tuple<Ts...>& t ) const noexcept
	{
		auto hash = [this, &t]<size_t... Is>( std::index_sequence<Is...> ) {
			HashType result = Seed;
			( ( result = combine( result, ( *this )( std::get<Is>( t ) ) ) ), ... );

			return result;
		};

		return hash( std::index_sequence_for<Ts...>{} );
	}

	//----------------------------------------------
	// std::variant type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename... Ts>
	inline HashType Hasher<HashType, Seed>::operator()( const std::variant<Ts...>& var ) const noexcept
	{
		// Hash the index to distinguish different alternatives
		HashType indexHash = ( *this )( var.index() );

		// Visit and hash the active alternative
		auto visitor = [this]( const auto& value ) -> HashType {
			return ( *this )( value );
		};

		HashType valueHash = std::visit( visitor, var );

		return combine( indexHash, valueHash );
	}

	//----------------------------------------------
	// std::vector type overloads
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename T>
	inline HashType Hasher<HashType, Seed>::operator()( const std::vector<T>& vec ) const noexcept
	{
		// Include size in hash to distinguish empty from non-empty vectors
		HashType result = combine( Seed, ( *this )( vec.size() ) );
		for ( const auto& elem : vec )
		{
			result = combine( result, ( *this )( elem ) );
		}

		return result;
	}

	//----------------------------------------------
	// Custom type fallback
	//----------------------------------------------

	template <Hash32or64 HashType, HashType Seed>
	template <typename TKey>
	inline std::enable_if_t<!std::is_same_v<std::decay_t<TKey>, std::string_view> &&
								!std::is_same_v<std::decay_t<TKey>, std::string> &&
								!std::is_same_v<std::decay_t<TKey>, const char*> &&
								!std::is_integral_v<TKey> &&
								!std::is_floating_point_v<TKey> &&
								!std::is_pointer_v<TKey> &&
								!std::is_enum_v<TKey> &&
								!is_std_array<TKey>::value &&
								!is_std_optional<TKey>::value &&
								!is_std_pair<TKey>::value &&
								!is_std_span<TKey>::value &&
								!is_std_tuple<TKey>::value &&
								!is_std_variant<TKey>::value &&
								!is_std_vector<TKey>::value,

		HashType>
	Hasher<HashType, Seed>::operator()( const TKey& key ) const noexcept
	{
		if constexpr ( std::same_as<HashType, uint32_t> )
		{
			// 32-bit hash path
			size_t hashValue = std::hash<TKey>{}( key );
			HashType result;
			if constexpr ( sizeof( size_t ) == 8 )
			{
				result = static_cast<HashType>( hashValue ^ ( hashValue >> 32 ) );
			}
			else
			{
				result = static_cast<HashType>( hashValue );
			}

			return result ^ Seed;
		}
		else // 64-bit hash path
		{
			HashType result = static_cast<HashType>( std::hash<TKey>{}( key ) );

			return result ^ Seed;
		}
	}
} // namespace nfx::hashing
