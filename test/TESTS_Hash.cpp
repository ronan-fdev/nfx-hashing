/**
 * @file TESTS_Hash.cpp
 * @brief Unit tests for the unified hash<T> API
 * @details Tests the template-based hash<T, HashType>() function with various types,
 *          hash sizes, and seeding strategies.
 */

#include <array>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include <gtest/gtest.h>

#include <nfx/Hashing.h>

namespace nfx::hashing::test
{
	using nfx::hashing::hash;
	using nfx::hashing::Hasher;

	//=====================================================================
	// Basic type hashing tests
	//=====================================================================

	TEST( Hash, IntegerTypes )
	{
		// Test various integer types
		EXPECT_NE( hash<int>( 42 ), 0u );
		EXPECT_NE( hash<long>( 42L ), 0u );
		EXPECT_NE( hash<unsigned int>( 42u ), 0u );
		EXPECT_NE( hash<int64_t>( 42LL ), 0u );

		// Same value, same type should produce same hash
		EXPECT_EQ( hash<int>( 42 ), hash<int>( 42 ) );

		// Different values should produce different hashes
		EXPECT_NE( hash<int>( 42 ), hash<int>( 43 ) );
	}

	TEST( Hash, StringTypes )
	{
		// Test various string types
		std::string str = "hello";
		std::string_view sv = "hello";
		const char* cstr = "hello";

		auto h1 = hash<std::string>( str );
		auto h2 = hash<std::string_view>( sv );
		auto h3 = hash<const char*>( cstr );

		// All should produce same hash for same content
		EXPECT_EQ( h1, h2 );
		EXPECT_EQ( h2, h3 );

		// Different strings should hash differently
		EXPECT_NE( hash<std::string>( "hello" ), hash<std::string>( "world" ) );
	}

	TEST( Hash, FloatingPointTypes )
	{
		// Test floating-point hashing
		EXPECT_NE( hash<float>( 3.14f ), 0u );
		EXPECT_NE( hash<double>( 3.14159 ), 0u );

		// Same value should hash the same
		EXPECT_EQ( hash<double>( 3.14 ), hash<double>( 3.14 ) );

		// Different values should hash differently
		EXPECT_NE( hash<double>( 3.14 ), hash<double>( 2.71 ) );

		// +0.0 and -0.0 should hash the same
		EXPECT_EQ( hash<double>( 0.0 ), hash<double>( -0.0 ) );
	}

	TEST( Hash, PointerTypes )
	{
		int x = 42;
		int y = 42;
		int* px = &x;
		int* py = &y;

		// Different pointers should hash differently
		EXPECT_NE( hash<int*>( px ), hash<int*>( py ) );

		// Same pointer should hash the same
		EXPECT_EQ( hash<int*>( px ), hash<int*>( px ) );
	}

	TEST( Hash, EnumTypes )
	{
		enum class Color
		{
			Red,
			Green,
			Blue
		};

		EXPECT_NE( hash<Color>( Color::Red ), hash<Color>( Color::Blue ) );
		EXPECT_EQ( hash<Color>( Color::Red ), hash<Color>( Color::Red ) );
	}

	//=====================================================================
	// Complex type hashing tests
	//=====================================================================

	TEST( Hash, PairTypes )
	{
		std::pair<int, std::string> p1{ 1, "one" };
		std::pair<int, std::string> p2{ 1, "one" };
		std::pair<int, std::string> p3{ 2, "two" };

		auto h1 = hash<std::pair<int, std::string>>( p1 );
		auto h2 = hash<std::pair<int, std::string>>( p2 );
		auto h3 = hash<std::pair<int, std::string>>( p3 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 );
	}

	TEST( Hash, TupleTypes )
	{
		std::tuple<int, double, std::string> t1{ 1, 2.5, "three" };
		std::tuple<int, double, std::string> t2{ 1, 2.5, "three" };
		std::tuple<int, double, std::string> t3{ 1, 2.5, "four" };

		auto h1 = hash<std::tuple<int, double, std::string>>( t1 );
		auto h2 = hash<std::tuple<int, double, std::string>>( t2 );
		auto h3 = hash<std::tuple<int, double, std::string>>( t3 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 );
	}

	TEST( Hash, ArrayTypes )
	{
		std::array<int, 3> arr1{ 1, 2, 3 };
		std::array<int, 3> arr2{ 1, 2, 3 };
		std::array<int, 3> arr3{ 1, 2, 4 };

		auto h1 = hash<std::array<int, 3>>( arr1 );
		auto h2 = hash<std::array<int, 3>>( arr2 );
		auto h3 = hash<std::array<int, 3>>( arr3 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 );
	}

	TEST( Hash, VectorTypes )
	{
		std::vector<int> vec1{ 1, 2, 3 };
		std::vector<int> vec2{ 1, 2, 3 };
		std::vector<int> vec3{ 1, 2, 4 };
		std::vector<int> vec4{}; // empty vector

		auto h1 = hash<std::vector<int>>( vec1 );
		auto h2 = hash<std::vector<int>>( vec2 );
		auto h3 = hash<std::vector<int>>( vec3 );
		auto h4 = hash<std::vector<int>>( vec4 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 );
		EXPECT_NE( h1, h4 ); // Empty vector should hash differently
	}

	TEST( Hash, OptionalTypes )
	{
		std::optional<int> opt1 = 42;
		std::optional<int> opt2 = 42;
		std::optional<int> opt3 = 43;
		std::optional<int> opt4 = std::nullopt;

		auto h1 = hash<std::optional<int>>( opt1 );
		auto h2 = hash<std::optional<int>>( opt2 );
		auto h3 = hash<std::optional<int>>( opt3 );
		auto h4 = hash<std::optional<int>>( opt4 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 );
		EXPECT_NE( h1, h4 ); // nullopt should hash differently
	}

	TEST( Hash, VariantTypes )
	{
		std::variant<int, std::string> var1 = 42;
		std::variant<int, std::string> var2 = 42;
		std::variant<int, std::string> var3 = std::string( "42" );
		std::variant<int, std::string> var4 = 43;

		auto h1 = hash<std::variant<int, std::string>>( var1 );
		auto h2 = hash<std::variant<int, std::string>>( var2 );
		auto h3 = hash<std::variant<int, std::string>>( var3 );
		auto h4 = hash<std::variant<int, std::string>>( var4 );

		EXPECT_EQ( h1, h2 );
		EXPECT_NE( h1, h3 ); // int(42) vs string("42") should differ
		EXPECT_NE( h1, h4 );
	}

	//=====================================================================
	// Hash size (32-bit vs 64-bit) tests
	//=====================================================================

	TEST( Hash, HashSize32Bit )
	{
		// Default should be 32-bit
		auto h1 = hash<int>( 42 );
		static_assert( std::is_same_v<decltype( h1 ), uint32_t>, "Default hash should be uint32_t" );

		auto h2 = hash<std::string>( "hello" );
		static_assert( std::is_same_v<decltype( h2 ), uint32_t>, "Default hash should be uint32_t" );
	}

	TEST( Hash, HashSize64Bit )
	{
		// Explicit 64-bit hash
		auto h1 = hash<int, uint64_t>( 42 );
		static_assert( std::is_same_v<decltype( h1 ), uint64_t>, "Explicit uint64_t hash should be uint64_t" );

		auto h2 = hash<std::string, uint64_t>( "hello" );
		static_assert( std::is_same_v<decltype( h2 ), uint64_t>, "Explicit uint64_t hash should be uint64_t" );

		// 64-bit hashes should differ from 32-bit hashes
		auto h32 = hash<int>( 42 );
		auto h64 = hash<int, uint64_t>( 42 );
		EXPECT_NE( static_cast<uint64_t>( h32 ), h64 );
	}

	//=====================================================================
	// Seeding tests
	//=====================================================================

	TEST( Hash, CompileTimeSeed )
	{
		// Test compile-time seed
		auto h1 = hash<int, uint32_t, 0>( 42 );
		auto h2 = hash<int, uint32_t, 0xDEADBEEF>( 42 );
		auto h3 = hash<int, uint32_t, 0xCAFEBABE>( 42 );

		// Different seeds should produce different hashes
		EXPECT_NE( h1, h2 );
		EXPECT_NE( h2, h3 );
		EXPECT_NE( h1, h3 );
	}

	//=====================================================================
	// Consistency tests (hash<T> should match Hasher<>)
	//=====================================================================

	TEST( Hash, ConsistencyWithHasher32 )
	{
		// hash<T>() should match Hasher<uint32_t>{}(value) - both use FNV default
		auto h1 = hash<int>( 42 );
		auto expected1 = Hasher<uint32_t>{}( 42 );
		EXPECT_EQ( h1, expected1 );

		auto h2 = hash<std::string>( "hello" );
		auto expected2 = Hasher<uint32_t>{}( "hello" );
		EXPECT_EQ( h2, expected2 );

		auto h3 = hash<double>( 3.14 );
		auto expected3 = Hasher<uint32_t>{}( 3.14 );
		EXPECT_EQ( h3, expected3 );
	}

	TEST( Hash, ConsistencyWithHasher64 )
	{
		// hash<T, uint64_t>() should match Hasher<uint64_t>{}(value) - both use FNV default
		auto h1 = hash<int, uint64_t>( 42 );
		auto expected1 = Hasher<uint64_t>{}( 42 );
		EXPECT_EQ( h1, expected1 );

		auto h2 = hash<std::string, uint64_t>( "hello" );
		auto expected2 = Hasher<uint64_t>{}( "hello" );
		EXPECT_EQ( h2, expected2 );

		auto h3 = hash<double, uint64_t>( 3.14 );
		auto expected3 = Hasher<uint64_t>{}( 3.14 );
		EXPECT_EQ( h3, expected3 );
	}

	TEST( Hash, ConsistencyWithHasherCustomSeed )
	{
		// hash<T, HashType, Seed>() should match Hasher<HashType, Seed>{}(value)
		constexpr uint32_t seed = 0xABCDEF01;

		auto h1 = hash<int, uint32_t, seed>( 42 );
		auto expected1 = Hasher<uint32_t, seed>{}( 42 );
		EXPECT_EQ( h1, expected1 );

		auto h2 = hash<std::string, uint32_t, seed>( "hello" );
		auto expected2 = Hasher<uint32_t, seed>{}( "hello" );
		EXPECT_EQ( h2, expected2 );
	}
} // namespace nfx::hashing::test

//=====================================================================
// Edge cases
//=====================================================================

TEST( Hash, EmptyStrings )
{
	auto h1 = nfx::hashing::hash<std::string>( "" );
	auto h2 = nfx::hashing::hash<std::string_view>( "" );

	EXPECT_EQ( h1, h2 );
	// Empty string with default seed of 0 may produce 0, which is acceptable
}

TEST( Hash, ZeroValues )
{
	auto h1 = nfx::hashing::hash<int>( 0 );
	auto h2 = nfx::hashing::hash<double>( 0.0 );

	// Zero values with default seed may produce 0, which is acceptable
	// Test that they're consistent instead
	EXPECT_EQ( h1, nfx::hashing::hash<int>( 0 ) );
	EXPECT_EQ( h2, nfx::hashing::hash<double>( 0.0 ) );
}

TEST( Hash, NegativeValues )
{
	auto h1 = nfx::hashing::hash<int>( -1 );
	auto h2 = nfx::hashing::hash<int>( 1 );
	auto h3 = nfx::hashing::hash<double>( -3.14 );
	auto h4 = nfx::hashing::hash<double>( 3.14 );

	EXPECT_NE( h1, h2 );
	EXPECT_NE( h3, h4 );
}
