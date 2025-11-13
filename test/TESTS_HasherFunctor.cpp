/**
 * @file TESTS_HasherFunctor.cpp
 * @brief Tests for STL-compatible Hasher functor
 * @details Tests for Hasher<HashType, Seed> supporting strings, integers, floats,
 *          pointers, enums, pairs, tuples, arrays, vectors, spans, optionals, and variants
 */

#include <unordered_set>

#include <gtest/gtest.h>

#include <nfx/Hashing.h>

namespace nfx::hashing::test
{
	using namespace nfx::hashing::constants;

	//=====================================================================
	// Hasher Functor Tests
	//=====================================================================

	//----------------------------------------------
	// Primitive types
	//----------------------------------------------

	TEST( HasherFunctor, StringTypes32Bit )
	{
		Hasher<uint32_t> hasher;

		// std::string
		std::string str{ "test string" };
		uint32_t hash1 = hasher( str );
		EXPECT_NE( hash1, 0u );

		// std::string_view
		std::string_view sv{ "test string" };
		uint32_t hash2 = hasher( sv );
		EXPECT_EQ( hash1, hash2 ); // Should match

		// const char*
		const char* cstr = "test string";
		uint32_t hash3 = hasher( cstr );
		EXPECT_EQ( hash1, hash3 ); // Should match

		// Different strings
		uint32_t hash4 = hasher( "different" );
		EXPECT_NE( hash1, hash4 );
	}

	TEST( HasherFunctor, StringTypes64Bit )
	{
		Hasher<uint64_t> hasher;

		// std::string
		std::string str{ "test string 64" };
		uint64_t hash1 = hasher( str );
		EXPECT_NE( hash1, 0u );

		// std::string_view
		std::string_view sv{ "test string 64" };
		uint64_t hash2 = hasher( sv );
		EXPECT_EQ( hash1, hash2 );

		// const char*
		const char* cstr = "test string 64";
		uint64_t hash3 = hasher( cstr );
		EXPECT_EQ( hash1, hash3 );
	}

	TEST( HasherFunctor, IntegerTypes )
	{
		Hasher<> hasher; // 32-bit default

		// Various integer types
		uint32_t hashInt8 = hasher( int8_t{ 42 } );
		uint32_t hashUInt8 = hasher( uint8_t{ 42 } );
		uint32_t hashInt16 = hasher( int16_t{ 42 } );
		uint32_t hashUInt16 = hasher( uint16_t{ 42 } );
		uint32_t hashInt32 = hasher( int32_t{ 42 } );
		uint32_t hashUInt32 = hasher( uint32_t{ 42 } );
		uint32_t hashInt64 = hasher( int64_t{ 42 } );
		uint32_t hashUInt64 = hasher( uint64_t{ 42 } );

		// All should produce same hash for value 42
		EXPECT_EQ( hashInt8, hashUInt8 );
		EXPECT_EQ( hashInt8, hashInt16 );
		EXPECT_EQ( hashInt8, hashUInt16 );
		EXPECT_EQ( hashInt8, hashInt32 );
		EXPECT_EQ( hashInt8, hashUInt32 );
		EXPECT_EQ( hashInt8, hashInt64 );
		EXPECT_EQ( hashInt8, hashUInt64 );

		// Different values should hash differently
		EXPECT_NE( hasher( 42 ), hasher( 43 ) );
	}

	TEST( HasherFunctor, IntegerTypes64Bit )
	{
		Hasher<uint64_t> hasher;

		uint64_t hash1 = hasher( int32_t{ 123 } );
		uint64_t hash2 = hasher( int64_t{ 123 } );
		uint64_t hash3 = hasher( uint64_t{ 123 } );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_EQ( hash1, hash3 );
		EXPECT_NE( hash1, 0u );
	}

	TEST( HasherFunctor, PointerTypes )
	{
		Hasher<> hasher;

		int value1 = 42;
		int value2 = 43;
		double dvalue = 3.14;

		// Hash pointers by address
		uint32_t hashPtr1 = hasher( &value1 );
		uint32_t hashPtr2 = hasher( &value2 );
		uint32_t hashPtrD = hasher( &dvalue );

		// Different addresses should hash differently
		EXPECT_NE( hashPtr1, hashPtr2 );
		EXPECT_NE( hashPtr1, hashPtrD );

		// Same pointer should hash consistently
		EXPECT_EQ( hasher( &value1 ), hashPtr1 );
	}

	TEST( HasherFunctor, FloatingPointTypes )
	{
		Hasher<> hasher;

		// float
		float f1 = 3.14f;
		float f2 = 3.14f;
		float f3 = 2.71f;
		uint32_t hashF1 = hasher( f1 );
		uint32_t hashF2 = hasher( f2 );
		uint32_t hashF3 = hasher( f3 );

		EXPECT_EQ( hashF1, hashF2 ); // Same value
		EXPECT_NE( hashF1, hashF3 ); // Different value

		// double
		double d1 = 2.718281828;
		double d2 = 2.718281828;
		double d3 = 3.141592653;
		uint32_t hashD1 = hasher( d1 );
		uint32_t hashD2 = hasher( d2 );
		uint32_t hashD3 = hasher( d3 );

		EXPECT_EQ( hashD1, hashD2 );
		EXPECT_NE( hashD1, hashD3 );
	}

	TEST( HasherFunctor, FloatingPointNormalization )
	{
		Hasher<> hasher;

		// +0.0 and -0.0 should hash the same
		double positive_zero = +0.0;
		double negative_zero = -0.0;
		EXPECT_EQ( hasher( positive_zero ), hasher( negative_zero ) );

		// All NaNs should hash the same
		double nan1 = std::numeric_limits<double>::quiet_NaN();
		double nan2 = std::numeric_limits<double>::signaling_NaN();
		EXPECT_EQ( hasher( nan1 ), hasher( nan2 ) );

		// Non-zero values should hash to non-zero
		EXPECT_NE( hasher( 1.0 ), 0u );
		EXPECT_NE( hasher( 3.14 ), 0u );
		EXPECT_NE( hasher( -2.71 ), 0u );
	}

	TEST( HasherFunctor, FloatingPointNoCollisions_32bit )
	{
		Hasher<uint32_t, FNV_OFFSET_BASIS_32> hasher;

		double zero = 0.0;
		double one = 1.0;
		double minus_one = -1.0;

		uint32_t hash_zero = hasher( zero );
		uint32_t hash_one = hasher( one );
		uint32_t hash_minus_one = hasher( minus_one );

		EXPECT_NE( hash_zero, hash_one ) << "0.0 and 1.0 should hash differently";
		EXPECT_NE( hash_zero, hash_minus_one ) << "0.0 and -1.0 should hash differently";
		EXPECT_NE( hash_one, hash_minus_one ) << "1.0 and -1.0 should hash differently";

		std::unordered_set<uint32_t> unique_hashes = { hash_zero, hash_one, hash_minus_one };
		EXPECT_EQ( unique_hashes.size(), 3 ) << "All three floating-point values must produce unique hashes";
	}

	TEST( HasherFunctor, EnumTypes )
	{
		enum class Color : uint32_t
		{
			Red = 1,
			Green = 2,
			Blue = 3
		};

		enum OldStyleEnum
		{
			ValueA = 10,
			ValueB = 20
		};

		Hasher<> hasher;

		// Enum class
		uint32_t hashRed = hasher( Color::Red );
		uint32_t hashGreen = hasher( Color::Green );
		uint32_t hashBlue = hasher( Color::Blue );

		EXPECT_NE( hashRed, hashGreen );
		EXPECT_NE( hashRed, hashBlue );
		EXPECT_NE( hashGreen, hashBlue );

		// Should match underlying integer hash
		EXPECT_EQ( hashRed, hasher( 1u ) );
		EXPECT_EQ( hashGreen, hasher( 2u ) );

		// Old-style enum
		uint32_t hashA = hasher( ValueA );
		uint32_t hashB = hasher( ValueB );
		EXPECT_NE( hashA, hashB );
		EXPECT_EQ( hashA, hasher( 10 ) );
	}

	TEST( HasherFunctor, PairTypes )
	{
		Hasher<> hasher;

		// std::pair<int, int>
		std::pair<int, int> p1{ 1, 2 };
		std::pair<int, int> p2{ 1, 2 };
		std::pair<int, int> p3{ 2, 1 };

		uint32_t hash1 = hasher( p1 );
		uint32_t hash2 = hasher( p2 );
		uint32_t hash3 = hasher( p3 );

		EXPECT_EQ( hash1, hash2 ); // Same pair
		EXPECT_NE( hash1, hash3 ); // Different order matters

		// std::pair<std::string, int>
		std::pair<std::string, int> p4{ "key", 42 };
		std::pair<std::string, int> p5{ "key", 42 };
		std::pair<std::string, int> p6{ "key", 43 };

		EXPECT_EQ( hasher( p4 ), hasher( p5 ) );
		EXPECT_NE( hasher( p4 ), hasher( p6 ) );
	}

	//----------------------------------------------
	// STL containers
	//----------------------------------------------

	TEST( HasherFunctor, TransparentLookup )
	{
		// Transparent lookup allows finding with string_view in string-keyed map
		std::unordered_map<std::string, int, Hasher<>, std::equal_to<>> map;

		map["hello"] = 1;
		map["world"] = 2;

		// Find using string_view (no temporary string allocation)
		std::string_view sv{ "hello" };
		auto it = map.find( sv );

		ASSERT_NE( it, map.end() );
		EXPECT_EQ( it->second, 1 );

		// Find using const char*
		auto it2 = map.find( "world" );
		ASSERT_NE( it2, map.end() );
		EXPECT_EQ( it2->second, 2 );
	}

	TEST( HasherFunctor, STLContainerUsage32Bit )
	{
		// Test in unordered_set
		std::unordered_set<std::string, Hasher<>> set;

		set.insert( "apple" );
		set.insert( "banana" );
		set.insert( "cherry" );

		EXPECT_EQ( set.size(), 3u );
		EXPECT_TRUE( set.find( "banana" ) != set.end() );
		EXPECT_TRUE( set.find( "grape" ) == set.end() );

		// Test in unordered_map
		std::unordered_map<int, std::string, Hasher<>> map;

		map[1] = "one";
		map[2] = "two";
		map[42] = "answer";

		EXPECT_EQ( map.size(), 3u );
		EXPECT_EQ( map[42], "answer" );
	}

	TEST( HasherFunctor, STLContainerUsage64Bit )
	{
		// Test 64-bit hasher in STL containers
		std::unordered_set<std::string, Hasher<uint64_t>> set64;

		set64.insert( "test1" );
		set64.insert( "test2" );
		set64.insert( "test3" );

		EXPECT_EQ( set64.size(), 3u );
		EXPECT_TRUE( set64.find( "test2" ) != set64.end() );

		// Test with 64-bit integers
		std::unordered_map<uint64_t, std::string, Hasher<uint64_t>> map64;

		map64[0xCAFEBABEDEADC0DEULL] = "magic";
		map64[0xFEEDFACE12345678ULL] = "number";

		EXPECT_EQ( map64.size(), 2u );
		EXPECT_EQ( map64[0xCAFEBABEDEADC0DEULL], "magic" );
	}

	//----------------------------------------------
	// Custom seeds and consistency
	//----------------------------------------------

	TEST( HasherFunctor, CustomSeed )
	{
		// Test with custom seed values
		Hasher<uint32_t, 0> hasher0;		  // Seed = 0
		Hasher<uint32_t, 0x12345678> hasher1; // Custom seed

		std::string test{ "test" };

		uint32_t hash0 = hasher0( test );
		uint32_t hash1 = hasher1( test );

		// Different seeds should produce different hashes
		EXPECT_NE( hash0, hash1 );

		// But same seed should be consistent
		Hasher<uint32_t, 0x12345678> hasher2;
		EXPECT_EQ( hash1, hasher2( test ) );
	}

	TEST( HasherFunctor, ConsistencyAcrossTypes )
	{
		Hasher<> hasher;

		// Integer consistency
		int value = 42;
		EXPECT_EQ( hasher( value ), hasher( 42 ) );
		EXPECT_EQ( hasher( value ), hasher( static_cast<uint32_t>( 42 ) ) );

		// String consistency
		std::string str{ "test" };
		std::string_view sv{ str };
		const char* cstr = "test";

		EXPECT_EQ( hasher( str ), hasher( sv ) );
		EXPECT_EQ( hasher( str ), hasher( cstr ) );
	}

	//----------------------------------------------
	// std::array
	//----------------------------------------------

	TEST( HasherFunctor, ArrayHashing )
	{
		Hasher<> hasher;

		// Integer arrays
		std::array<int, 3> arr1{ 1, 2, 3 };
		std::array<int, 3> arr2{ 1, 2, 3 };
		std::array<int, 3> arr3{ 3, 2, 1 };

		uint32_t hash1 = hasher( arr1 );
		uint32_t hash2 = hasher( arr2 );
		uint32_t hash3 = hasher( arr3 );

		EXPECT_EQ( hash1, hash2 ); // Same array
		EXPECT_NE( hash1, hash3 ); // Different order matters
	}

	TEST( HasherFunctor, ArrayHashingStrings )
	{
		Hasher<uint64_t> hasher;

		// String arrays
		std::array<std::string, 2> arr1{ "hello", "world" };
		std::array<std::string, 2> arr2{ "hello", "world" };
		std::array<std::string, 2> arr3{ "world", "hello" };

		uint64_t hash1 = hasher( arr1 );
		uint64_t hash2 = hasher( arr2 );
		uint64_t hash3 = hasher( arr3 );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HasherFunctor, ArrayInSTLContainer )
	{
		// Use array as key in unordered_set
		std::unordered_set<std::array<int, 3>, Hasher<>, std::equal_to<>> set;

		set.insert( { 1, 2, 3 } );
		set.insert( { 4, 5, 6 } );
		set.insert( { 1, 2, 3 } ); // Duplicate

		EXPECT_EQ( set.size(), 2u ); // Only 2 unique arrays
		EXPECT_TRUE( set.find( { 1, 2, 3 } ) != set.end() );
		EXPECT_TRUE( set.find( { 4, 5, 6 } ) != set.end() );
		EXPECT_TRUE( set.find( { 7, 8, 9 } ) == set.end() );
	}

	TEST( HasherFunctor, EmptyArray )
	{
		Hasher<> hasher;

		std::array<int, 0> empty1{};
		std::array<int, 0> empty2{};

		uint32_t hash1 = hasher( empty1 );
		uint32_t hash2 = hasher( empty2 );

		// Empty arrays should hash to seed value
		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, 0u );
	}

	TEST( HasherFunctor, LargeArray )
	{
		Hasher<uint64_t> hasher;

		std::array<int, 100> arr1;
		std::array<int, 100> arr2;

		// Fill with sequential values
		for ( size_t i = 0; i < 100; ++i )
		{
			arr1[i] = static_cast<int>( i );
			arr2[i] = static_cast<int>( i );
		}

		uint64_t hash1 = hasher( arr1 );
		uint64_t hash2 = hasher( arr2 );

		EXPECT_EQ( hash1, hash2 );

		// Change one element
		arr2[50] = 999;
		uint64_t hash3 = hasher( arr2 );

		EXPECT_NE( hash1, hash3 );
	}

	TEST( HasherFunctor, ArrayOfArrays )
	{
		Hasher<> hasher;

		std::array<std::array<int, 2>, 2> nested1{ { { { 1, 2 } }, { { 3, 4 } } } };
		std::array<std::array<int, 2>, 2> nested2{ { { { 1, 2 } }, { { 3, 4 } } } };
		std::array<std::array<int, 2>, 2> nested3{ { { { 1, 2 } }, { { 3, 5 } } } };

		EXPECT_EQ( hasher( nested1 ), hasher( nested2 ) );
		EXPECT_NE( hasher( nested1 ), hasher( nested3 ) );
	}

	TEST( HasherFunctor, MixedTupleAndArray )
	{
		Hasher<uint64_t> hasher;

		// Tuple containing array
		std::tuple<std::string, std::array<int, 3>> t1{ "test", { 1, 2, 3 } };
		std::tuple<std::string, std::array<int, 3>> t2{ "test", { 1, 2, 3 } };
		std::tuple<std::string, std::array<int, 3>> t3{ "test", { 1, 2, 4 } };

		EXPECT_EQ( hasher( t1 ), hasher( t2 ) );
		EXPECT_NE( hasher( t1 ), hasher( t3 ) );

		// Array containing tuples - use explicit construction
		std::array<std::tuple<int, std::string>, 2> arr1{ { std::tuple<int, std::string>{ 1, "a" },
			std::tuple<int, std::string>{ 2, "b" } } };
		std::array<std::tuple<int, std::string>, 2> arr2{ { std::tuple<int, std::string>{ 1, "a" },
			std::tuple<int, std::string>{ 2, "b" } } };

		EXPECT_EQ( hasher( arr1 ), hasher( arr2 ) );
	}

	//----------------------------------------------
	// std::optional
	//----------------------------------------------

	TEST( HasherFunctor, OptionalWithValue )
	{
		Hasher<> hasher;

		std::optional<int> opt1{ 42 };
		std::optional<int> opt2{ 42 };
		std::optional<int> opt3{ 99 };

		EXPECT_EQ( hasher( opt1 ), hasher( opt2 ) );
		EXPECT_NE( hasher( opt1 ), hasher( opt3 ) );
	}

	TEST( HasherFunctor, OptionalNullopt )
	{
		Hasher<> hasher;

		std::optional<int> opt1;
		std::optional<int> opt2;
		std::optional<int> opt3{ 42 };

		EXPECT_EQ( hasher( opt1 ), hasher( opt2 ) );
		EXPECT_NE( hasher( opt1 ), hasher( opt3 ) );
	}

	TEST( HasherFunctor, OptionalString )
	{
		Hasher<uint64_t> hasher;

		std::optional<std::string> opt1{ "hello" };
		std::optional<std::string> opt2{ "hello" };
		std::optional<std::string> opt3{ "world" };
		std::optional<std::string> optEmpty;

		EXPECT_EQ( hasher( opt1 ), hasher( opt2 ) );
		EXPECT_NE( hasher( opt1 ), hasher( opt3 ) );
		EXPECT_NE( hasher( opt1 ), hasher( optEmpty ) );
	}

	TEST( HasherFunctor, OptionalNested )
	{
		Hasher<> hasher;

		std::optional<std::vector<int>> opt1{ std::vector<int>{ 1, 2, 3 } };
		std::optional<std::vector<int>> opt2{ std::vector<int>{ 1, 2, 3 } };
		std::optional<std::vector<int>> opt3{ std::vector<int>{ 1, 2, 4 } };
		std::optional<std::vector<int>> optEmpty;

		EXPECT_EQ( hasher( opt1 ), hasher( opt2 ) );
		EXPECT_NE( hasher( opt1 ), hasher( opt3 ) );
		EXPECT_NE( hasher( opt1 ), hasher( optEmpty ) );
	}

	//----------------------------------------------
	// std::span
	//----------------------------------------------

	TEST( HasherFunctor, SpanHashing )
	{
		Hasher<> hasher;

		std::array<int, 5> arr1{ 1, 2, 3, 4, 5 };
		std::array<int, 5> arr2{ 1, 2, 3, 4, 5 };
		std::array<int, 5> arr3{ 1, 2, 3, 4, 6 };

		std::span<int> span1{ arr1 };
		std::span<int> span2{ arr2 };
		std::span<int> span3{ arr3 };

		EXPECT_EQ( hasher( span1 ), hasher( span2 ) );
		EXPECT_NE( hasher( span1 ), hasher( span3 ) );
	}

	TEST( HasherFunctor, SpanSubrange )
	{
		Hasher<> hasher;

		std::array<int, 5> arr{ 1, 2, 3, 4, 5 };

		std::span<int> fullSpan{ arr };
		std::span<int> subSpan1{ arr.data(), 3 };	  // [1, 2, 3]
		std::span<int> subSpan2{ arr.data() + 2, 3 }; // [3, 4, 5]

		EXPECT_NE( hasher( fullSpan ), hasher( subSpan1 ) );
		EXPECT_NE( hasher( subSpan1 ), hasher( subSpan2 ) );
	}

	TEST( HasherFunctor, SpanFromVector )
	{
		Hasher<uint64_t> hasher;

		std::vector<int> vec1{ 10, 20, 30 };
		std::vector<int> vec2{ 10, 20, 30 };

		std::span<int> span1{ vec1 };
		std::span<int> span2{ vec2 };

		EXPECT_EQ( hasher( span1 ), hasher( span2 ) );
	}

	TEST( HasherFunctor, SpanConst )
	{
		Hasher<> hasher;

		const std::array<int, 3> arr1{ 1, 2, 3 };
		const std::array<int, 3> arr2{ 1, 2, 3 };

		std::span<const int> span1{ arr1 };
		std::span<const int> span2{ arr2 };

		EXPECT_EQ( hasher( span1 ), hasher( span2 ) );
	}

	//----------------------------------------------
	// std::tuple
	//----------------------------------------------

	TEST( HasherFunctor, TupleHashing )
	{
		Hasher<> hasher;

		// 2-element tuple (similar to pair but using tuple)
		std::tuple<int, int> t1{ 1, 2 };
		std::tuple<int, int> t2{ 1, 2 };
		std::tuple<int, int> t3{ 2, 1 };

		uint32_t hash1 = hasher( t1 );
		uint32_t hash2 = hasher( t2 );
		uint32_t hash3 = hasher( t3 );

		EXPECT_EQ( hash1, hash2 ); // Same tuple
		EXPECT_NE( hash1, hash3 ); // Different order matters

		// 3-element tuple
		std::tuple<int, std::string, double> t4{ 42, "test", 3.14 };
		std::tuple<int, std::string, double> t5{ 42, "test", 3.14 };
		std::tuple<int, std::string, double> t6{ 42, "test", 2.71 };

		EXPECT_EQ( hasher( t4 ), hasher( t5 ) );
		EXPECT_NE( hasher( t4 ), hasher( t6 ) );
	}

	TEST( HasherFunctor, TupleHashingMixedTypes )
	{
		Hasher<uint64_t> hasher;

		// Complex nested tuple
		std::tuple<std::string, int, std::pair<double, float>> complex1{
			"key", 123, { 3.14, 2.71f } };
		std::tuple<std::string, int, std::pair<double, float>> complex2{
			"key", 123, { 3.14, 2.71f } };
		std::tuple<std::string, int, std::pair<double, float>> complex3{
			"key", 124, { 3.14, 2.71f } };

		uint64_t hash1 = hasher( complex1 );
		uint64_t hash2 = hasher( complex2 );
		uint64_t hash3 = hasher( complex3 );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HasherFunctor, TupleInSTLContainer )
	{
		// Use tuple as key in unordered_map
		std::unordered_map<std::tuple<int, std::string>, int, Hasher<>, std::equal_to<>> map;

		map[{ 1, "one" }] = 100;
		map[{ 2, "two" }] = 200;
		map[{ 3, "three" }] = 300;

		EXPECT_EQ( map.size(), 3u );

		// Store in variables to avoid macro comma issues
		int val1 = map[{ 1, "one" }];
		int val2 = map[{ 2, "two" }];
		EXPECT_EQ( val1, 100 );
		EXPECT_EQ( val2, 200 );

		// Check that same key doesn't add new entry
		map[{ 1, "one" }] = 111;
		EXPECT_EQ( map.size(), 3u );
		int val3 = map[{ 1, "one" }];
		EXPECT_EQ( val3, 111 );
	}

	TEST( HasherFunctor, EmptyTuple )
	{
		Hasher<> hasher;

		std::tuple<> empty1;
		std::tuple<> empty2;

		uint32_t hash1 = hasher( empty1 );
		uint32_t hash2 = hasher( empty2 );

		// Empty tuples should hash to seed value
		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, 0u ); // Should not be zero (uses seed)
	}

	TEST( HasherFunctor, SingleElementTuple )
	{
		Hasher<> hasher;

		std::tuple<int> t1{ 42 };
		std::tuple<int> t2{ 42 };
		std::tuple<int> t3{ 43 };

		EXPECT_EQ( hasher( t1 ), hasher( t2 ) );
		EXPECT_NE( hasher( t1 ), hasher( t3 ) );
	}

	//----------------------------------------------
	// std::variant
	//----------------------------------------------

	TEST( HasherFunctor, VariantSameType )
	{
		Hasher<> hasher;

		std::variant<int, std::string> var1{ 42 };
		std::variant<int, std::string> var2{ 42 };
		std::variant<int, std::string> var3{ 99 };

		EXPECT_EQ( hasher( var1 ), hasher( var2 ) );
		EXPECT_NE( hasher( var1 ), hasher( var3 ) );
	}

	TEST( HasherFunctor, VariantDifferentTypes )
	{
		Hasher<> hasher;

		std::variant<int, std::string> var1{ 42 };
		std::variant<int, std::string> var2{ std::string{ "42" } };

		// Even if string representation is "42", it should hash differently
		// because index differs
		EXPECT_NE( hasher( var1 ), hasher( var2 ) );
	}

	TEST( HasherFunctor, VariantStrings )
	{
		Hasher<uint64_t> hasher;

		std::variant<std::string, int> var1{ std::string{ "hello" } };
		std::variant<std::string, int> var2{ std::string{ "hello" } };
		std::variant<std::string, int> var3{ std::string{ "world" } };

		EXPECT_EQ( hasher( var1 ), hasher( var2 ) );
		EXPECT_NE( hasher( var1 ), hasher( var3 ) );
	}

	TEST( HasherFunctor, VariantComplex )
	{
		Hasher<> hasher;

		using ComplexVariant = std::variant<int, std::string, std::vector<int>>;

		ComplexVariant var1{ std::vector<int>{ 1, 2, 3 } };
		ComplexVariant var2{ std::vector<int>{ 1, 2, 3 } };
		ComplexVariant var3{ std::vector<int>{ 1, 2, 4 } };
		ComplexVariant var4{ 123 };

		EXPECT_EQ( hasher( var1 ), hasher( var2 ) );
		EXPECT_NE( hasher( var1 ), hasher( var3 ) );
		EXPECT_NE( hasher( var1 ), hasher( var4 ) );
	}

	//----------------------------------------------
	// std::vector
	//----------------------------------------------

	TEST( HasherFunctor, VectorHashing )
	{
		Hasher<> hasher;

		std::vector<int> vec1{ 1, 2, 3, 4, 5 };
		std::vector<int> vec2{ 1, 2, 3, 4, 5 };
		std::vector<int> vec3{ 1, 2, 3, 4, 6 };
		std::vector<int> vec4{ 1, 2, 3, 4 }; // Different size

		EXPECT_EQ( hasher( vec1 ), hasher( vec2 ) );
		EXPECT_NE( hasher( vec1 ), hasher( vec3 ) );
		EXPECT_NE( hasher( vec1 ), hasher( vec4 ) ); // Different sizes hash differently
	}

	TEST( HasherFunctor, VectorEmpty )
	{
		Hasher<> hasher;

		std::vector<int> empty1;
		std::vector<int> empty2;
		std::vector<int> nonEmpty{ 1 };

		EXPECT_EQ( hasher( empty1 ), hasher( empty2 ) );
		EXPECT_NE( hasher( empty1 ), hasher( nonEmpty ) );
	}

	TEST( HasherFunctor, VectorStrings )
	{
		Hasher<uint64_t> hasher;

		std::vector<std::string> vec1{ "hello", "world" };
		std::vector<std::string> vec2{ "hello", "world" };
		std::vector<std::string> vec3{ "hello", "rust" };

		EXPECT_EQ( hasher( vec1 ), hasher( vec2 ) );
		EXPECT_NE( hasher( vec1 ), hasher( vec3 ) );
	}

	TEST( HasherFunctor, VectorNested )
	{
		Hasher<> hasher;

		std::vector<std::vector<int>> nested1{ { 1, 2 }, { 3, 4 } };
		std::vector<std::vector<int>> nested2{ { 1, 2 }, { 3, 4 } };
		std::vector<std::vector<int>> nested3{ { 1, 2 }, { 3, 5 } };

		EXPECT_EQ( hasher( nested1 ), hasher( nested2 ) );
		EXPECT_NE( hasher( nested1 ), hasher( nested3 ) );
	}

	//----------------------------------------------
	// Mixed Container
	//----------------------------------------------

	TEST( HasherFunctor, VectorOfOptionals )
	{
		Hasher<uint64_t> hasher;

		std::vector<std::optional<int>> vec1{ 1, 2, std::nullopt, 4 };
		std::vector<std::optional<int>> vec2{ 1, 2, std::nullopt, 4 };
		std::vector<std::optional<int>> vec3{ 1, 2, 3, 4 };

		EXPECT_EQ( hasher( vec1 ), hasher( vec2 ) );
		EXPECT_NE( hasher( vec1 ), hasher( vec3 ) );
	}

	TEST( HasherFunctor, OptionalOfVariant )
	{
		Hasher<> hasher;

		using Var = std::variant<int, std::string>;

		std::optional<Var> opt1{ Var{ 42 } };
		std::optional<Var> opt2{ Var{ 42 } };
		std::optional<Var> opt3{ Var{ std::string{ "hello" } } };
		std::optional<Var> optEmpty;

		EXPECT_EQ( hasher( opt1 ), hasher( opt2 ) );
		EXPECT_NE( hasher( opt1 ), hasher( opt3 ) );
		EXPECT_NE( hasher( opt1 ), hasher( optEmpty ) );
	}

	TEST( HasherFunctor, VectorInSTLContainer )
	{
		// Test using vector as key in unordered_set
		std::unordered_set<std::vector<int>, Hasher<>> set;

		std::vector<int> vec1{ 1, 2, 3 };
		std::vector<int> vec2{ 4, 5, 6 };
		std::vector<int> vec3{ 1, 2, 3 }; // Duplicate of vec1

		set.insert( vec1 );
		set.insert( vec2 );
		set.insert( vec3 ); // Should not be added (duplicate)

		EXPECT_EQ( set.size(), 2u );
		EXPECT_TRUE( set.contains( vec1 ) );
		EXPECT_TRUE( set.contains( vec2 ) );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( HasherFunctor, EmptyStringHandling )
	{
		Hasher<> hasher32;
		Hasher<uint64_t> hasher64;

		// Empty strings should hash to 0
		EXPECT_EQ( hasher32( "" ), 0u );
		EXPECT_EQ( hasher32( std::string{} ), 0u );
		EXPECT_EQ( hasher32( std::string_view{} ), 0u );

		EXPECT_EQ( hasher64( "" ), 0u );
		EXPECT_EQ( hasher64( std::string{} ), 0u );
		EXPECT_EQ( hasher64( std::string_view{} ), 0u );
	}

	TEST( HasherFunctor, ComplexPairs )
	{
		Hasher<uint64_t> hasher;

		// Nested pairs
		std::pair<std::string, std::pair<int, double>> complex1{ "key", { 42, 3.14 } };
		std::pair<std::string, std::pair<int, double>> complex2{ "key", { 42, 3.14 } };
		std::pair<std::string, std::pair<int, double>> complex3{ "key", { 42, 2.71 } };

		uint64_t hash1 = hasher( complex1 );
		uint64_t hash2 = hasher( complex2 );
		uint64_t hash3 = hasher( complex3 );

		EXPECT_EQ( hash1, hash2 );
		EXPECT_NE( hash1, hash3 );
	}

	TEST( HasherFunctor, MixedTypeMap )
	{
		// Map with different key types using the same hasher
		std::unordered_map<int, std::string, Hasher<>> intMap;
		std::unordered_map<std::string, int, Hasher<>> stringMap;
		std::unordered_map<double, std::string, Hasher<>> doubleMap;

		intMap[1] = "one";
		intMap[2] = "two";

		stringMap["alpha"] = 1;
		stringMap["beta"] = 2;

		doubleMap[3.14] = "pi";
		doubleMap[2.71] = "e";

		EXPECT_EQ( intMap.size(), 2u );
		EXPECT_EQ( stringMap.size(), 2u );
		EXPECT_EQ( doubleMap.size(), 2u );

		EXPECT_EQ( intMap[1], "one" );
		EXPECT_EQ( stringMap["beta"], 2 );
		EXPECT_EQ( doubleMap[3.14], "pi" );
	}

	TEST( HasherFunctor, NoCollisionsInDistribution )
	{
		Hasher<> hasher;
		std::unordered_set<uint32_t> hashes;

		// Hash many different strings
		for ( int i = 0; i < 1000; ++i )
		{
			std::string str = "hasher_test_" + std::to_string( i );
			hashes.insert( hasher( str ) );
		}

		// Should have no collisions
		EXPECT_EQ( hashes.size(), 1000u );
	}

	TEST( HasherFunctor, DefaultHashTypeIs32Bit )
	{
		// Verify default template parameter is uint32_t
		Hasher<> defaultHasher;
		static_assert( std::is_same_v<decltype( defaultHasher( "test" ) ), uint32_t>, "Default hash type should be uint32_t" );
	}

	TEST( HasherFunctor, Explicit64BitHashType )
	{
		// Verify explicit 64-bit template parameter
		Hasher<uint64_t> hasher64;
		static_assert( std::is_same_v<decltype( hasher64( "test" ) ), uint64_t>, "Hash type should be uint64_t" );
	}
} // namespace nfx::hashing::test
