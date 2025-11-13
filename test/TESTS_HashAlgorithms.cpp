/**
 * @file TESTS_HashAlgorithms.cpp
 * @brief Tests for hashing algorithms
 * @details Tests covering FNV-1a, CRC32, Larson, integer hashing, hash combining, and seed mixing
 */

#include <gtest/gtest.h>

#include <nfx/Hashing.h>

namespace nfx::hashing::test
{
	using namespace nfx::hashing::constants;

	//=====================================================================
	// Hash algorithm primitives
	//=====================================================================

	TEST( HashingBasic, LarsonHash )
	{
		// Larson hash: hash = 37 * hash + ch
		uint32_t hash{ 0 };

		hash = larson( hash, 'A' );
		EXPECT_EQ( hash, 65u ); // 37 * 0 + 65

		hash = larson( hash, 'B' );
		EXPECT_EQ( hash, 2471u ); // 37 * 65 + 66

		hash = larson( hash, 'C' );
		EXPECT_EQ( hash, 91494u ); // 37 * 2471 + 67
	}

	TEST( HashingBasic, FNV1aHash )
	{
		// FNV-1a: hash = (hash ^ ch) * prime
		uint32_t hash{ FNV_OFFSET_BASIS_32 };

		hash = fnv1a( hash, 'T' );
		EXPECT_NE( hash, FNV_OFFSET_BASIS_32 );

		uint32_t prevHash{ hash };
		hash = fnv1a( hash, 'e' );
		EXPECT_NE( hash, prevHash );

		// Same input should produce same output
		uint32_t hash2{ FNV_OFFSET_BASIS_32 };
		hash2 = fnv1a( hash2, 'T' );
		hash2 = fnv1a( hash2, 'e' );
		EXPECT_EQ( hash, hash2 );
	}

	TEST( HashingBasic, CRC32Hash )
	{
		uint32_t hash{ 0 };

		hash = crc32c( hash, 'A' );
		EXPECT_NE( hash, 0u );

		uint32_t prevHash{ hash };
		hash = crc32c( hash, 'B' );
		EXPECT_NE( hash, prevHash );

		// Same input should produce same output
		uint32_t hash2{ 0 };
		hash2 = crc32c( hash2, 'A' );
		hash2 = crc32c( hash2, 'B' );
		EXPECT_EQ( hash, hash2 );
	}

	TEST( HashingBasic, CRC32SoftwareVsHardware )
	{
		// Verify software implementation matches hardware (or auto-selected) implementation
		uint32_t hashHW{ 0 };
		uint32_t hashSW{ 0 };

		// Test single bytes
		hashHW = crc32c( hashHW, 'T' );
		hashSW = crc32cSoft( hashSW, 'T' );
		EXPECT_EQ( hashHW, hashSW );

		hashHW = crc32c( hashHW, 'e' );
		hashSW = crc32cSoft( hashSW, 'e' );
		EXPECT_EQ( hashHW, hashSW );

		hashHW = crc32c( hashHW, 's' );
		hashSW = crc32cSoft( hashSW, 's' );
		EXPECT_EQ( hashHW, hashSW );

		hashHW = crc32c( hashHW, 't' );
		hashSW = crc32cSoft( hashSW, 't' );
		EXPECT_EQ( hashHW, hashSW );

		// Test full string
		std::string_view testString{ "The quick brown fox jumps over the lazy dog" };
		uint32_t hashHW2{ 0 };
		uint32_t hashSW2{ 0 };

		for ( char ch : testString )
		{
			hashHW2 = crc32c( hashHW2, static_cast<uint8_t>( ch ) );
			hashSW2 = crc32cSoft( hashSW2, static_cast<uint8_t>( ch ) );
		}

		EXPECT_EQ( hashHW2, hashSW2 );
		EXPECT_NE( hashHW2, 0u ); // Verify non-zero result

		// Test all byte values (0-255)
		for ( uint32_t i = 0; i < 256; ++i )
		{
			uint32_t hw = crc32c( 0, static_cast<uint8_t>( i ) );
			uint32_t sw = crc32cSoft( 0, static_cast<uint8_t>( i ) );
			EXPECT_EQ( hw, sw ) << "Mismatch at byte value " << i;
		}
	}

	//----------------------------------------------
	// Integer types
	//----------------------------------------------

	TEST( HashingInteger, Int32Values )
	{
		// Test 32-bit integer hashing
		auto hash1 = hash<int32_t>( int32_t{ 42 } );
		auto hash2 = hash<int32_t>( int32_t{ 42 } );
		auto hash3 = hash<int32_t>( int32_t{ 43 } );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HashingInteger, Int64Values )
	{
		// Test 64-bit integer hashing
		auto hash1 = hash<int64_t>( static_cast<int64_t>( 0xCAFEBABEDEADC0DEULL ) );
		auto hash2 = hash<int64_t>( static_cast<int64_t>( 0xCAFEBABEDEADC0DEULL ) );
		auto hash3 = hash<int64_t>( static_cast<int64_t>( 0xFEEDFACEDEADBEEFULL ) );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HashingInteger, UInt32Values )
	{
		auto hash1 = hash<uint32_t>( uint32_t{ 123456 } );
		auto hash2 = hash<uint32_t>( uint32_t{ 123456 } );
		auto hash3 = hash<uint32_t>( uint32_t{ 654321 } );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HashingInteger, ZeroValues )
	{
		auto hashZero32 = hash<int32_t>( int32_t{ 0 } );
		auto hashZero64 = hash<int64_t>( int64_t{ 0 } );

		// Zero hashes to zero (multiplicative hashing property)
		EXPECT_EQ( hashZero32, 0u );
		EXPECT_EQ( hashZero64, 0u );

		// But non-zero values should hash differently
		auto hashOne32 = hash<int32_t>( int32_t{ 1 } );
		auto hashOne64 = hash<int64_t>( int64_t{ 1 } );
		EXPECT_NE( hashOne32, 0u );
		EXPECT_NE( hashOne64, 0u );
	}

	//----------------------------------------------
	// String types
	//----------------------------------------------

	TEST( HashingString, EmptyString )
	{
		uint32_t hashValue = hash<std::string_view>( "" );

		EXPECT_EQ( hashValue, 0u );
	}

	TEST( HashingString, ShortString )
	{
		auto hash1 = hash<std::string_view>( "test" );
		auto hash2 = hash<std::string_view>( "test" );
		auto hash3 = hash<std::string_view>( "Test" );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output (case sensitive)
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HashingString, LongerStrings )
	{
		auto hash1 = hash<std::string_view>( "The quick brown fox jumps over the lazy dog" );
		auto hash2 = hash<std::string_view>( "The quick brown fox jumps over the lazy dog" );
		auto hash3 = hash<std::string_view>( "The quick brown fox jumps over the lazy cat" );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HashingString, StringViewVsStdString )
	{
		std::string str{ "consistency test" };
		std::string_view view{ str };

		auto hashFromView = hash<std::string_view>( view );
		auto hashFromString = hash<std::string>( str );

		// Should produce identical hashes
		EXPECT_EQ( hashFromView, hashFromString );
	}

	//----------------------------------------------
	// Combination
	//----------------------------------------------

	TEST( HashingCombine, Combine32Bit )
	{
		uint32_t hash1{ 0x12345678 };
		uint32_t hash2{ 0xABCDEF00 };

		uint32_t combined = combine( hash1, hash2, FNV_PRIME_32 );

		// Combined hash should be different from both inputs
		EXPECT_NE( combined, hash1 );
		EXPECT_NE( combined, hash2 );
		EXPECT_NE( combined, 0u );

		// Same inputs should produce same output
		uint32_t combined2 = combine( hash1, hash2, FNV_PRIME_32 );
		EXPECT_EQ( combined, combined2 );
	}

	TEST( HashingCombine, Combine64Bit )
	{
		size_t hash1{ 0xCAFEBABEDEADC0DE };
		size_t hash2{ 0xFEEDFACE12345678 };

		size_t combined = combine( hash1, hash2 );

		// Combined hash should be different from both inputs
		EXPECT_NE( combined, hash1 );
		EXPECT_NE( combined, hash2 );
		EXPECT_NE( combined, 0u );

		// Same inputs should produce same output
		size_t combined2 = combine( hash1, hash2 );
		EXPECT_EQ( combined, combined2 );
	}

	TEST( HashingCombine, MultipleValues )
	{
		// Combine multiple hashes sequentially
		uint32_t result{ 0 };

		result = combine( result, hash<std::string_view>( "name" ), FNV_PRIME_32 );
		result = combine( result, hash<int>( 42 ), FNV_PRIME_32 );
		result = combine( result, hash<std::string_view>( "value" ), FNV_PRIME_32 );

		EXPECT_NE( result, 0u );

		// Same sequence should produce same result
		uint32_t result2{ 0 };
		result2 = combine( result2, hash<std::string_view>( "name" ), FNV_PRIME_32 );
		result2 = combine( result2, hash<int>( 42 ), FNV_PRIME_32 );
		result2 = combine( result2, hash<std::string_view>( "value" ), FNV_PRIME_32 );

		EXPECT_EQ( result, result2 );
	}

	//----------------------------------------------
	// Seed mixing
	//----------------------------------------------

	TEST( HashingSeedMix, BasicSeedMixing )
	{
		uint32_t seed{ 0x1A21DA };
		uint32_t hash{ 0xCAFEBABE };
		size_t tableSize{ 256 }; // Must be power of 2

		uint32_t index = seedMix( seed, hash, tableSize );

		// Index should be within table bounds
		EXPECT_LT( index, tableSize );

		// Same inputs -> same output
		uint32_t index2 = seedMix( seed, hash, tableSize );
		EXPECT_EQ( index, index2 );
	}

	TEST( HashingSeedMix, DifferentSeeds )
	{
		uint32_t hash{ 0xDEADC0DE };
		size_t tableSize{ 1024 };

		uint32_t index1 = seedMix<uint32_t>( 0xCA7, hash, tableSize ); // "CAT"
		uint32_t index2 = seedMix<uint32_t>( 0xD06, hash, tableSize ); // "DOG"

		// Note: Could theoretically collide, but very unlikely
		EXPECT_TRUE( index1 < tableSize );
		EXPECT_TRUE( index2 < tableSize );
	}

	//----------------------------------------------
	// Constexpr verification
	//----------------------------------------------

	TEST( HashingConstexpr, CompileTimeHashing )
	{
		// These should be evaluable at compile time
		constexpr uint32_t compileTimeHash = fnv1a( FNV_OFFSET_BASIS_32, 'X' );
		constexpr uint32_t compileTimeLarson = larson<uint32_t>( 0, 'Y' );
		constexpr uint32_t compileTimeCombine = combine<uint32_t>( 0x12345678, 0xABCDEF00, FNV_PRIME_32 );
		constexpr uint32_t compileTimeSeedMix = seedMix<uint32_t>( 0x1234, 0xABCD, 256 );

		// Runtime versions should match
		EXPECT_EQ( fnv1a( FNV_OFFSET_BASIS_32, 'X' ), compileTimeHash );
		EXPECT_EQ( larson<uint32_t>( 0, 'Y' ), compileTimeLarson );
		EXPECT_EQ( combine<uint32_t>( 0x12345678, 0xABCDEF00, FNV_PRIME_32 ), compileTimeCombine );
		EXPECT_EQ( seedMix<uint32_t>( 0x1234, 0xABCD, 256 ), compileTimeSeedMix );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( HashingEdgeCases, VeryLongStrings )
	{
		std::string longString( 10000, 'X' );
		uint32_t hashValue = hash<std::string>( longString );

		EXPECT_NE( hashValue, FNV_OFFSET_BASIS_32 );

		// Same content should produce same hash
		std::string longString2( 10000, 'X' );
		uint32_t hash2 = hash<std::string>( longString2 );
		EXPECT_EQ( hashValue, hash2 );
	}

	TEST( HashingEdgeCases, SpecialCharacters )
	{
		std::string_view sv1( "\n\t\r\0test", 8 );
		std::string_view sv2( "\n\t\r\0test", 8 );

		auto hash1 = hash<std::string_view>( sv1 );
		auto hash2 = hash<std::string_view>( sv2 );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, FNV_OFFSET_BASIS_32 );
	}

	TEST( EdgeCases, ExtremeIntegerValues )
	{
		auto hashMin32 = hash<int32_t>( std::numeric_limits<int32_t>::min() );
		auto hashMax32 = hash<int32_t>( std::numeric_limits<int32_t>::max() );
		auto hashMin64 = hash<int64_t, uint64_t>( std::numeric_limits<int64_t>::min() );
		auto hashMax64 = hash<int64_t, uint64_t>( std::numeric_limits<int64_t>::max() );

		// All should produce valid non-zero hashes
		EXPECT_NE( hashMin32, 0u );
		EXPECT_NE( hashMax32, 0u );
		EXPECT_NE( hashMin64, 0u );
		EXPECT_NE( hashMax64, 0u );

		// Should be different from each other
		EXPECT_NE( hashMin32, hashMax32 );
		EXPECT_NE( hashMin64, hashMax64 );
	}

	//----------------------------------------------
	// 64-bit Hash functions
	//----------------------------------------------

	TEST( Hashing64Bit, larson64 )
	{
		// Larson hash: hash = 37 * hash + ch (64-bit version)
		uint64_t hash{ 0 };

		hash = larson<uint64_t>( hash, 'A' );
		EXPECT_EQ( hash, 65u ); // 37 * 0 + 65

		hash = larson<uint64_t>( hash, 'B' );
		EXPECT_EQ( hash, 2471u ); // 37 * 65 + 66

		hash = larson<uint64_t>( hash, 'C' );
		EXPECT_EQ( hash, 91494u ); // 37 * 2471 + 67

		// Verify 64-bit result is different from 32-bit for large values
		uint64_t hash64{ 0xFFFFFFFF00000000ULL };
		hash64 = larson<uint64_t>( hash64, 'X' );
		EXPECT_GT( hash64, 0xFFFFFFFFULL ); // Should overflow 32-bit range
	}

	TEST( Hashing64Bit, FNV1a64 )
	{
		// FNV-1a 64-bit: hash = (hash ^ ch) * FNV_PRIME_64
		uint64_t hash{ FNV_OFFSET_BASIS_64 };

		hash = fnv1a<uint64_t>( hash, 'T' );
		EXPECT_NE( hash, FNV_OFFSET_BASIS_64 );

		uint64_t prevHash{ hash };
		hash = fnv1a<uint64_t>( hash, 'e' );
		EXPECT_NE( hash, prevHash );

		// Same input should produce same output
		uint64_t hash2{ FNV_OFFSET_BASIS_64 };
		hash2 = fnv1a<uint64_t>( hash2, 'T' );
		hash2 = fnv1a<uint64_t>( hash2, 'e' );
		EXPECT_EQ( hash, hash2 );

		// Verify using correct 64-bit prime
		uint64_t manualHash = ( FNV_OFFSET_BASIS_64 ^ 'T' ) * FNV_PRIME_64;
		uint64_t testHash = fnv1a<uint64_t>( FNV_OFFSET_BASIS_64, 'T' );
		EXPECT_EQ( manualHash, testHash );
	}

	TEST( Hashing64Bit, HashStringView64 )
	{
		// 64-bit string hashing uses dual CRC32-C streams
		uint64_t hash1 = hash<std::string_view, uint64_t>( "test" );
		uint64_t hash2 = hash<std::string_view, uint64_t>( "test" );
		uint64_t hash3 = hash<std::string_view, uint64_t>( "Test" );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output
		EXPECT_NE( hash1, hash3 );

		// Should be full 64-bit value (not just 32-bit extended)
		EXPECT_TRUE( ( hash1 >> 32 ) != 0 || ( hash1 & 0xFFFFFFFF ) != 0 );
	}

	TEST( Hashing64Bit, HashStringView64EmptyString )
	{
		uint64_t hashValue = hash<std::string_view, uint64_t>( "" );

		EXPECT_EQ( hashValue, 0u );
	}

	TEST( Hashing64Bit, HashStringView64VsHashStringView32 )
	{
		// Test that when using the SAME initial seed, 64-bit low 32 bits match 32-bit hash
		std::string_view testStr{ "Hello, World!" };

		// Use 0 as initial seed for both to ensure low 32 bits match
		uint32_t hash32 = hash<std::string_view, uint32_t, 0>( testStr );
		uint64_t hash64 = hash<std::string_view, uint64_t, 0>( testStr );

		// Low 32 bits should match (both are CRC32-C of same data with same seed)
		EXPECT_EQ( hash64 & 0xFFFFFFFF, hash32 );

		// But 64-bit should use high 32 bits too (dual CRC32 with inverted bytes)
		uint32_t high = static_cast<uint32_t>( hash64 >> 32 );
		EXPECT_NE( high, 0u );								  // High bits should be non-zero for non-empty strings
		EXPECT_NE( high, hash32 );							  // High should differ from low (due to byte ^ 0xFF)
		EXPECT_NE( hash64, static_cast<uint64_t>( hash32 ) ); // Full 64-bit hash should not just be 32-bit extended
	}

	TEST( Hashing64Bit, HashInteger64 )
	{
		// 64-bit integer hashing uses Wang hash algorithm
		uint64_t hash1 = hash<uint64_t, uint64_t>( 0xCAFEBABEDEADC0DEULL );
		uint64_t hash2 = hash<uint64_t, uint64_t>( 0xCAFEBABEDEADC0DEULL );
		uint64_t hash3 = hash<uint64_t, uint64_t>( 0xFEEDFACEDEADBEEFULL );

		// Same input -> same output
		EXPECT_EQ( hash1, hash2 );

		// Different input -> different output
		EXPECT_NE( hash1, hash3 );

		// Should use full 64-bit space
		EXPECT_NE( hash1, 0u );
	}

	TEST( Hashing64Bit, HashInteger64SmallValues )
	{
		// Even small values should produce well-distributed 64-bit hashes
		uint64_t hash0 = hash<uint64_t, uint64_t>( 0ULL );
		uint64_t hash1 = hash<uint64_t, uint64_t>( 1ULL );
		uint64_t hash42 = hash<uint64_t, uint64_t>( 42ULL );

		// Zero hashes to zero (Wang hash property)
		EXPECT_EQ( hash0, 0u );

		// Non-zero values should be well-distributed
		EXPECT_NE( hash1, 0u );
		EXPECT_NE( hash42, 0u );
		EXPECT_NE( hash1, hash42 );

		// Should use upper bits too
		EXPECT_TRUE( ( hash1 >> 32 ) != 0 || ( hash1 & 0xFFFFFFFF ) != 0 );
		EXPECT_TRUE( ( hash42 >> 32 ) != 0 || ( hash42 & 0xFFFFFFFF ) != 0 );
	}

	TEST( Hashing64Bit, Combine64WithPrime )
	{
		// Test 64-bit combine with explicit prime (FNV-1a style)
		uint64_t hash1{ 0x123456789ABCDEF0ULL };
		uint64_t hash2{ 0xFEDCBA9876543210ULL };

		uint64_t combined = combine<uint64_t>( hash1, hash2, FNV_PRIME_64 );

		// Combined hash should be different from both inputs
		EXPECT_NE( combined, hash1 );
		EXPECT_NE( combined, hash2 );
		EXPECT_NE( combined, 0u );

		// Same inputs should produce same output
		uint64_t combined2 = combine<uint64_t>( hash1, hash2, FNV_PRIME_64 );
		EXPECT_EQ( combined, combined2 );
	}

	TEST( Hashing64Bit, Combine64BoostMurmur )
	{
		// Test 64-bit combine without prime (Boost + MurmurHash3 finalizer)
		uint64_t hash1{ 0xCAFEBABEDEADC0DEULL };
		uint64_t hash2{ 0xFEEDFACE12345678ULL };

		uint64_t combined = combine<uint64_t>( hash1, hash2 );

		// Combined hash should be different from both inputs
		EXPECT_NE( combined, hash1 );
		EXPECT_NE( combined, hash2 );
		EXPECT_NE( combined, 0u );

		// Same inputs should produce same output
		uint64_t combined2 = combine<uint64_t>( hash1, hash2 );
		EXPECT_EQ( combined, combined2 );

		// Should use MurmurHash3 finalizer (different from FNV-1a)
		uint64_t combinedFNV = combine<uint64_t>( hash1, hash2, FNV_PRIME_64 );
		EXPECT_NE( combined, combinedFNV );
	}

	TEST( Hashing64Bit, SeedMix64 )
	{
		uint64_t seed{ 0x1A21DA };
		uint64_t hash{ 0xCAFEBABEDEADC0DEULL };
		uint64_t tableSize{ 256 }; // Must be power of 2

		uint64_t index = seedMix<uint64_t>( seed, hash, tableSize );

		// Index should be within table bounds
		EXPECT_LT( index, tableSize );

		// Same inputs -> same output
		uint64_t index2 = seedMix<uint64_t>( seed, hash, tableSize );
		EXPECT_EQ( index, index2 );
	}

	TEST( Hashing64Bit, SeedMix64LargeTable )
	{
		uint64_t seed{ 0xDEADBEEF };
		uint64_t hash{ 0x123456789ABCDEF0ULL };
		uint64_t tableSize{ 0x100000000ULL }; // 4GB table (requires 64-bit)

		uint64_t index = seedMix<uint64_t>( seed, hash, tableSize );

		// Index should be within bounds
		EXPECT_LT( index, tableSize );

		// Different seeds should produce different indices
		uint64_t index2 = seedMix<uint64_t>( 0xCAFEBABE, hash, tableSize );
		EXPECT_TRUE( index < tableSize );
		EXPECT_TRUE( index2 < tableSize );
	}

	TEST( Hashing64Bit, Constexpr64 )
	{
		// 64-bit versions should also be compile-time evaluable
		constexpr uint64_t compileTimeHash64 = fnv1a<uint64_t>( FNV_OFFSET_BASIS_64, 'X' );
		constexpr uint64_t compileTimeLarson64 = larson<uint64_t>( 0, 'Y' );
		constexpr uint64_t compileTimeCombine64 = combine<uint64_t>( 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL, FNV_PRIME_64 );
		constexpr uint64_t compileTimeSeedMix64 = seedMix<uint64_t>( 0x1234, 0xABCDEF, 256 );

		// Runtime versions should match
		EXPECT_EQ( fnv1a<uint64_t>( FNV_OFFSET_BASIS_64, 'X' ), compileTimeHash64 );
		EXPECT_EQ( larson<uint64_t>( 0, 'Y' ), compileTimeLarson64 );
		EXPECT_EQ( combine<uint64_t>( 0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL, FNV_PRIME_64 ), compileTimeCombine64 );
		EXPECT_EQ( seedMix<uint64_t>( 0x1234, 0xABCDEF, 256 ), compileTimeSeedMix64 );
	}

	TEST( Hashing64Bit, CompositeKey64 )
	{
		// Test combining multiple 64-bit hashes
		uint64_t result{ 0 };

		result = combine<uint64_t>( result, hash<std::string_view, uint64_t>( "username" ) );
		result = combine<uint64_t>( result, hash<uint64_t, uint64_t>( 12345ULL ) );
		result = combine<uint64_t>( result, hash<std::string_view, uint64_t>( "session_id" ) );

		EXPECT_NE( result, 0u );

		// Same sequence should produce same result
		uint64_t result2{ 0 };
		result2 = combine<uint64_t>( result2, hash<std::string_view, uint64_t>( "username" ) );
		result2 = combine<uint64_t>( result2, hash<uint64_t, uint64_t>( 12345ULL ) );
		result2 = combine<uint64_t>( result2, hash<std::string_view, uint64_t>( "session_id" ) );

		EXPECT_EQ( result, result2 );
	}

	TEST( Hashing64Bit, DualCRC32Coverage )
	{
		// Test that 64-bit string hashing uses both high and low 32 bits
		std::vector<std::string> testStrings{
			"test1", "test2", "test3", "test4", "test5",
			"hello", "world", "foo", "bar", "baz" };

		bool hasHighBits = false;
		bool hasLowBits = false;

		for ( const auto& str : testStrings )
		{
			uint64_t hashValue = hash<std::string, uint64_t>( str );

			uint32_t high = static_cast<uint32_t>( hashValue >> 32 );
			uint32_t low = static_cast<uint32_t>( hashValue & 0xFFFFFFFF );

			if ( high != 0 )
				hasHighBits = true;
			if ( low != 0 )
				hasLowBits = true;

			// High and low should be different (dual CRC32 with byte inversion)
			EXPECT_NE( high, low );
		}

		// Both 32-bit halves should be utilized
		EXPECT_TRUE( hasHighBits );
		EXPECT_TRUE( hasLowBits );
	}
} // namespace nfx::hashing::test
