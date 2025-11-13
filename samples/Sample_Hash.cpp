/**
 * @file Sample_Hash.cpp
 * @brief Demonstrates the unified hash<T> API
 * @details Shows how to use the new template-based hash<T, HashType>() function
 *          for various types with explicit type specification and optional seeding.
 */

#include <iostream>
#include <string>
#include <vector>

#include <nfx/Hashing.h>

using namespace nfx::hashing;

int main()
{
	std::cout << "=== nfx-hashing: Unified hash<T> API Demo ===\n\n";

	//=====================================================================
	// Basic usage with explicit type specification
	//=====================================================================

	std::cout << "--- Basic Types ---\n";

	// Hash integers (returns uint32_t by default)
	auto h1 = hash<int>( 42 );
	std::cout << "hash<int>(42)                    = 0x" << std::hex << h1 << std::dec << "\n";

	// Hash strings
	auto h2 = hash<std::string>( "hello world" );
	std::cout << "hash<std::string>(\"hello world\") = 0x" << std::hex << h2 << std::dec << "\n";

	// Hash floating-point
	auto h3 = hash<double>( 3.14159 );
	std::cout << "hash<double>(3.14159)            = 0x" << std::hex << h3 << std::dec << "\n";

	// Hash string_view
	std::string_view sv = "string_view example";
	auto h4 = hash<std::string_view>( sv );
	std::cout << "hash<std::string_view>(sv)       = 0x" << std::hex << h4 << std::dec << "\n";

	std::cout << "\n";

	//=====================================================================
	// 64-bit hashes with explicit HashType parameter
	//=====================================================================

	std::cout << "--- 64-bit Hashes ---\n";

	// Hash int to 64-bit
	auto h5 = hash<int, uint64_t>( 42 );
	std::cout << "hash<int, uint64_t>(42)                    = 0x" << std::hex << h5 << std::dec << "\n";

	// Hash string to 64-bit
	auto h6 = hash<std::string, uint64_t>( "hello world" );
	std::cout << "hash<std::string, uint64_t>(\"hello world\") = 0x" << std::hex << h6 << std::dec << "\n";

	std::cout << "\n";

	//=====================================================================
	// Custom seeding (compile-time)
	//=====================================================================

	std::cout << "--- Compile-Time Seeding ---\n";

	// Default seed is FNV_OFFSET_BASIS
	auto h7a = hash<int>( 42 );
	std::cout << "hash<int>(42)                               = 0x" << std::hex << h7a << std::dec << " (default FNV_OFFSET_BASIS seed)\n";

	// Hash with custom seed at compile-time
	auto h7 = hash<int, uint32_t, 0xDEADBEEF>( 42 );
	std::cout << "hash<int, uint32_t, 0xDEADBEEF>(42)         = 0x" << std::hex << h7 << std::dec << "\n";

	// String with default FNV basis
	auto h8a = hash<std::string, uint32_t, 0x811C9DC5>( "hello" );
	std::cout << "hash<string, uint32_t, 0x811C9DC5>(\"hello\") = 0x" << std::hex << h8a << std::dec << " (explicit FNV, same as default)\n";

	auto h8 = hash<std::string, uint32_t, 0xCAFEBABE>( "hello" );
	std::cout << "hash<string, uint32_t, 0xCAFEBABE>(\"hello\") = 0x" << std::hex << h8 << std::dec << "\n";

	std::cout << "\n";

	//=====================================================================
	// Complex types (pairs, tuples, vectors)
	//=====================================================================

	std::cout << "--- Complex Types ---\n";

	// Hash a pair
	std::pair<int, std::string> p = { 42, "answer" };
	auto h11 = hash<std::pair<int, std::string>>( p );
	std::cout << "hash<pair<int, string>>({42, \"answer\"}) = 0x" << std::hex << h11 << std::dec << "\n";

	// Hash a tuple
	std::tuple<int, double, std::string> t = { 1, 2.5, "three" };
	auto h12 = hash<std::tuple<int, double, std::string>>( t );
	std::cout << "hash<tuple<int, double, string>>({1, 2.5, \"three\"}) = 0x" << std::hex << h12 << std::dec << "\n";

	// Hash a vector
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	auto h13 = hash<std::vector<int>>( vec );
	std::cout << "hash<vector<int>>({1,2,3,4,5})  = 0x" << std::hex << h13 << std::dec << "\n";

	std::cout << "\n";

	return 0;
}
