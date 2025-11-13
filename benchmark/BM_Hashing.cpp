/**
 * @file BM_Hashing.cpp
 * @brief Benchmark core hash algorithms and infrastructure
 * @details Benchmarks for FNV-1a, CRC32-C, string hashing,
 *          integer hashing, and hash combining performance
 */

#include <random>

#include <benchmark/benchmark.h>

#include <nfx/Hashing.h>

namespace nfx::hashing::benchmark
{
	//=====================================================================
	// Hashing infrastructure benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Test data generation
	//----------------------------------------------

	static std::vector<std::string> generateTestStrings( size_t count, size_t minLength = 5, size_t maxLength = 50 )
	{
		std::vector<std::string> strings;
		strings.reserve( count );

		std::mt19937 gen( 42 ); // Fixed seed for reproducibility
		std::uniform_int_distribution<> lengthDist( static_cast<int>( minLength ), static_cast<int>( maxLength ) );
		std::uniform_int_distribution<> charDist( 'a', 'z' );

		for ( size_t i = 0; i < count; ++i )
		{
			const size_t len = static_cast<size_t>( lengthDist( gen ) );
			std::string str;
			str.reserve( len );

			for ( size_t j = 0; j < len; ++j )
			{
				str.push_back( static_cast<char>( charDist( gen ) ) );
			}

			strings.emplace_back( std::move( str ) );
		}

		return strings;
	}

	static std::vector<uint32_t> generateTestIntegers( size_t count )
	{
		std::vector<uint32_t> integers;
		integers.reserve( count );

		std::mt19937 gen( 42 );
		std::uniform_int_distribution<uint32_t> dist( 0, UINT32_MAX );

		for ( size_t i = 0; i < count; ++i )
		{
			integers.push_back( dist( gen ) );
		}

		return integers;
	}

	// Test data sets
	static const auto shortStrings = generateTestStrings( 100, 3, 8 );
	static const auto mediumStrings = generateTestStrings( 100, 10, 25 );
	static const auto longStrings = generateTestStrings( 100, 50, 200 );
	static const auto testIntegers = generateTestIntegers( 1000 );

	//----------------------------------------------
	// Low-level hash building blocks benchmarks
	//----------------------------------------------

	//----------------------------
	// Single-step hash functions
	//----------------------------

	static void BM_Fnv1a_SingleStep( ::benchmark::State& state )
	{
		uint32_t initialHash = nfx::hashing::constants::FNV_OFFSET_BASIS_32;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint32_t hash = nfx::hashing::fnv1a( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32C_SingleStep( ::benchmark::State& state )
	{
		uint32_t initialHash = 0;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint32_t hash = nfx::hashing::crc32c( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32CSoft_SingleStep( ::benchmark::State& state )
	{
		uint32_t initialHash = 0;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint32_t hash = nfx::hashing::crc32cSoft( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32C_ShortString( ::benchmark::State& state )
	{
		std::string_view testStr = "Hello";
		uint32_t initialHash = 0;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testStr );

		for ( auto _ : state )
		{
			uint32_t hash = initialHash;
			for ( char ch : testStr )
			{
				hash = nfx::hashing::crc32c( hash, static_cast<uint8_t>( ch ) );
			}
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32CSoft_ShortString( ::benchmark::State& state )
	{
		std::string_view testStr = "Hello";
		uint32_t initialHash = 0;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testStr );

		for ( auto _ : state )
		{
			uint32_t hash = initialHash;
			for ( char ch : testStr )
			{
				hash = nfx::hashing::crc32cSoft( hash, static_cast<uint8_t>( ch ) );
			}
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32C_MediumString( ::benchmark::State& state )
	{
		std::string_view testStr = "The quick brown fox jumps over the lazy dog";
		uint32_t initialHash = 0;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testStr );

		for ( auto _ : state )
		{
			uint32_t hash = initialHash;
			for ( char ch : testStr )
			{
				hash = nfx::hashing::crc32c( hash, static_cast<uint8_t>( ch ) );
			}
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Crc32CSoft_MediumString( ::benchmark::State& state )
	{
		std::string_view testStr = "The quick brown fox jumps over the lazy dog";
		uint32_t initialHash = 0;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testStr );

		for ( auto _ : state )
		{
			uint32_t hash = initialHash;
			for ( char ch : testStr )
			{
				hash = nfx::hashing::crc32cSoft( hash, static_cast<uint8_t>( ch ) );
			}
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Larson_SingleStep( ::benchmark::State& state )
	{
		uint32_t initialHash = 0;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint32_t hash = nfx::hashing::larson( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	//----------------------------
	// Seed mixing utilities
	//----------------------------

	static void BM_SeedMix_Function( ::benchmark::State& state )
	{
		uint32_t seed = 12345;
		uint32_t hash = 0xABCDEF01;
		size_t tableSize = 1024;
		::benchmark::DoNotOptimize( seed );
		::benchmark::DoNotOptimize( hash );
		::benchmark::DoNotOptimize( tableSize );

		for ( auto _ : state )
		{
			uint32_t result = nfx::hashing::seedMix( seed, hash, tableSize );
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------------------------
	// String hashing benchmarks
	//----------------------------------------------

	//----------------------------
	// High-level string hashing
	//----------------------------

	static void BM_HashStringView_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// Manual FNV-1a
	//----------------------------

	static void BM_ByteByByteFNV1a_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				uint32_t hash = nfx::hashing::constants::FNV_OFFSET_BASIS_32;
				for ( char c : str )
				{
					hash = nfx::hashing::fnv1a( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ByteByByteFNV1a_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				uint32_t hash = nfx::hashing::constants::FNV_OFFSET_BASIS_32;
				for ( char c : str )
				{
					hash = nfx::hashing::fnv1a( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ByteByByteFNV1a_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				uint32_t hash = nfx::hashing::constants::FNV_OFFSET_BASIS_32;
				for ( char c : str )
				{
					hash = nfx::hashing::fnv1a( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// Manual CRC32-C
	//----------------------------

	static void BM_ByteByByteCRC32C_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::hashing::crc32c( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ByteByByteCRC32C_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::hashing::crc32c( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_ByteByByteCRC32C_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				uint32_t hash = 0;
				for ( char c : str )
				{
					hash = nfx::hashing::crc32c( hash, static_cast<uint8_t>( c ) );
				}
				totalHash += hash;
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// std::hash
	//----------------------------

	static void BM_StdHash_Short( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_Medium( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_Long( ::benchmark::State& state )
	{
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				totalHash += hasher( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// Integer hashing benchmarks
	//----------------------------------------------

	static void BM_HashInteger_uint32( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				totalHash += nfx::hashing::hash<uint32_t>( value );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashInteger_uint64( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				uint64_t largeValue = static_cast<uint64_t>( value ) << 32 | value;
				totalHash += nfx::hashing::hash<uint64_t>( largeValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashInteger_int32( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				int32_t signedValue = static_cast<int32_t>( value );
				totalHash += nfx::hashing::hash<int32_t>( signedValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// std::hash
	//----------------------------

	static void BM_StdHash_uint32( ::benchmark::State& state )
	{
		std::hash<uint32_t> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				totalHash += hasher( value );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_StdHash_uint64( ::benchmark::State& state )
	{
		std::hash<uint64_t> hasher;

		for ( auto _ : state )
		{
			size_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				uint64_t largeValue = static_cast<uint64_t>( value ) << 32 | value;
				totalHash += hasher( largeValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// Algorithm comparison benchmarks
	//----------------------------------------------

	//----------------------------
	// Single string
	//----------------------------

	static void BM_SingleString_HashStringView( ::benchmark::State& state )
	{
		const std::string testStr = "performance_test_string_for_comparison";

		for ( auto _ : state )
		{
			uint32_t hash = nfx::hashing::hash<std::string>( testStr );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_SingleString_StdHash( ::benchmark::State& state )
	{
		const std::string testStr = "performance_test_string_for_comparison";
		std::hash<std::string> hasher;

		for ( auto _ : state )
		{
			size_t hash = hasher( testStr );
			::benchmark::DoNotOptimize( hash );
		}
	}

	//----------------------------------------------
	// Cache and memory patterns
	//----------------------------------------------

	static void BM_HashStringView_CacheTest( ::benchmark::State& state )
	{
		// Test cache behavior with repeated hashing of same strings
		std::vector<std::string> repeatedStrings;
		for ( int i = 0; i < 10; ++i )
		{
			for ( const auto& str : shortStrings )
			{
				repeatedStrings.push_back( str );
			}
		}

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : repeatedStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_Sequential_StringHashing( ::benchmark::State& state )
	{
		std::vector<std::string> sequentialStrings;
		for ( size_t i = 0; i < 1000; ++i )
		{
			sequentialStrings.push_back( "string_" + std::to_string( i ) );
		}

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : sequentialStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_Random_StringHashing( ::benchmark::State& state )
	{
		auto randomStrings = generateTestStrings( 1000, 8, 32 );

		for ( auto _ : state )
		{
			uint32_t totalHash = 0;
			for ( const auto& str : randomStrings )
			{
				totalHash += nfx::hashing::hash<std::string>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------------------------
	// 64-bit hash function benchmarks
	//----------------------------------------------

	//----------------------------
	// 64-bit low-level operations
	//----------------------------

	static void BM_Fnv1a64_SingleStep( ::benchmark::State& state )
	{
		uint64_t initialHash = nfx::hashing::constants::FNV_OFFSET_BASIS_64;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint64_t hash = nfx::hashing::fnv1a<uint64_t>( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_Larson64_SingleStep( ::benchmark::State& state )
	{
		uint64_t initialHash = 0;
		uint8_t testByte = 'A';
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( testByte );

		for ( auto _ : state )
		{
			uint64_t hash = nfx::hashing::larson<uint64_t>( initialHash, testByte );
			::benchmark::DoNotOptimize( hash );
		}
	}

	static void BM_SeedMix64_Function( ::benchmark::State& state )
	{
		uint64_t seed = 12345;
		uint64_t hash = 0xABCDEF0123456789ULL;
		size_t tableSize = 1024;
		::benchmark::DoNotOptimize( seed );
		::benchmark::DoNotOptimize( hash );
		::benchmark::DoNotOptimize( tableSize );

		for ( auto _ : state )
		{
			uint64_t result = nfx::hashing::seedMix<uint64_t>( seed, hash, tableSize );
			::benchmark::DoNotOptimize( result );
		}
	}

	//----------------------------
	// 64-bit string hashing
	//----------------------------

	static void BM_HashStringView64_Short( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint64_t totalHash = 0;
			for ( const auto& str : shortStrings )
			{
				totalHash += nfx::hashing::hash<std::string, uint64_t>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView64_Medium( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint64_t totalHash = 0;
			for ( const auto& str : mediumStrings )
			{
				totalHash += nfx::hashing::hash<std::string, uint64_t>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	static void BM_HashStringView64_Long( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint64_t totalHash = 0;
			for ( const auto& str : longStrings )
			{
				totalHash += nfx::hashing::hash<std::string, uint64_t>( str );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// 64-bit integer hashing
	//----------------------------

	static void BM_HashInteger64_uint64( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			uint64_t totalHash = 0;
			for ( uint32_t value : testIntegers )
			{
				uint64_t largeValue = static_cast<uint64_t>( value ) << 32 | value;
				totalHash += nfx::hashing::hash<uint64_t, uint64_t>( largeValue );
			}
			::benchmark::DoNotOptimize( totalHash );
		}
	}

	//----------------------------
	// 64-bit hash combining
	//----------------------------

	static void BM_Combine64_FNV( ::benchmark::State& state )
	{
		uint64_t initialHash = nfx::hashing::constants::FNV_OFFSET_BASIS_64;
		uint64_t newHash = 0x123456789ABCDEF0ULL;
		uint64_t prime = nfx::hashing::constants::FNV_PRIME_64;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( newHash );
		::benchmark::DoNotOptimize( prime );

		for ( auto _ : state )
		{
			uint64_t result = nfx::hashing::combine( initialHash, newHash, prime );
			::benchmark::DoNotOptimize( result );
		}
	}

	static void BM_Combine64_BoostMurmur( ::benchmark::State& state )
	{
		uint64_t initialHash = nfx::hashing::constants::FNV_OFFSET_BASIS_64;
		uint64_t newHash = 0x123456789ABCDEF0ULL;
		::benchmark::DoNotOptimize( initialHash );
		::benchmark::DoNotOptimize( newHash );

		for ( auto _ : state )
		{
			uint64_t result = nfx::hashing::combine( initialHash, newHash );
			::benchmark::DoNotOptimize( result );
		}
	}
} // namespace nfx::hashing::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

//----------------------------------------------
// Low-level hash building blocks
//----------------------------------------------

BENCHMARK( nfx::hashing::benchmark::BM_Fnv1a_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32C_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32CSoft_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32C_ShortString )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32CSoft_ShortString )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32C_MediumString )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Crc32CSoft_MediumString )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Larson_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_SeedMix_Function )->Repetitions( 3 );

//----------------------------------------------
// String hashing comparisons
//----------------------------------------------

//----------------------------
// High-level API
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_HashStringView_Short )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashStringView_Medium )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashStringView_Long )->Repetitions( 3 );

//----------------------------
// Manual FNV-1a
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteFNV1a_Short )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteFNV1a_Medium )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteFNV1a_Long )->Repetitions( 3 );

//----------------------------
// Manual CRC32
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteCRC32C_Short )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteCRC32C_Medium )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_ByteByByteCRC32C_Long )->Repetitions( 3 );

//----------------------------
// std::hash comparison
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_StdHash_Short )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_StdHash_Medium )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_StdHash_Long )->Repetitions( 3 );

//----------------------------------------------
// Integer hashing comparisons
//----------------------------------------------

BENCHMARK( nfx::hashing::benchmark::BM_HashInteger_uint32 )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashInteger_uint64 )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashInteger_int32 )->Repetitions( 3 );

BENCHMARK( nfx::hashing::benchmark::BM_StdHash_uint32 )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_StdHash_uint64 )->Repetitions( 3 );

//----------------------------------------------
// Single string comparisons
//----------------------------------------------

BENCHMARK( nfx::hashing::benchmark::BM_SingleString_HashStringView )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_SingleString_StdHash )->Repetitions( 3 );

//----------------------------------------------
// Cache and memory patterns
//----------------------------------------------

BENCHMARK( nfx::hashing::benchmark::BM_HashStringView_CacheTest )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Sequential_StringHashing )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Random_StringHashing )->Repetitions( 3 );

//----------------------------------------------
// 64-bit hash function benchmarks
//----------------------------------------------

//----------------------------
// 64-bit low-level operations
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_Fnv1a64_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Larson64_SingleStep )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_SeedMix64_Function )->Repetitions( 3 );

//----------------------------
// 64-bit string hashing
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_HashStringView64_Short )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashStringView64_Medium )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_HashStringView64_Long )->Repetitions( 3 );

//----------------------------
// 64-bit integer hashing
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_HashInteger64_uint64 )->Repetitions( 3 );

//----------------------------
// 64-bit hash combining
//----------------------------

BENCHMARK( nfx::hashing::benchmark::BM_Combine64_FNV )->Repetitions( 3 );
BENCHMARK( nfx::hashing::benchmark::BM_Combine64_BoostMurmur )->Repetitions( 3 );

BENCHMARK_MAIN();
