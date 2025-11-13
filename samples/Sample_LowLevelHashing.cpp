/**
 * @file Sample_LowLevelHashing.cpp
 * @brief Demonstrates low-level hashing functions and building blocks
 * @details This sample shows direct usage of hash functions (CRC32-C, FNV-1a),
 *          hash combining, integer hashing, and algorithm constants for advanced
 *          use cases requiring manual hash construction.
 */

#include <iomanip>
#include <iostream>
#include <string>

#include <nfx/Hashing.h>

using namespace nfx::hashing;

int main()
{
	std::cout << "=== nfx-hashing Low-Level Hashing Functions ===\n\n";

	//=====================================================================
	// 1. String hashing with hash<T>()
	//=====================================================================
	{
		std::cout << "1. String hashing with hash<T>()\n";
		std::cout << "--------------------------------\n";

		std::string_view text = "Hello, World!";
		uint32_t hash32 = hash<std::string_view, uint32_t>( text );
		uint64_t hash64 = hash<std::string_view, uint64_t>( text );

		std::cout << "Input: \"" << text << "\"\n";
		std::cout << "32-bit hash: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << hash32 << "\n";
		std::cout << "64-bit hash: 0x" << std::setw( 16 ) << hash64 << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 2. Integer hashing with hash<T>()
	//=====================================================================
	{
		std::cout << "2. Integer hashing with hash<T>()\n";
		std::cout << "----------------------------------\n";

		uint32_t value = 0xDEADBEEF;
		uint32_t hash32 = hash<uint32_t>( value );
		uint64_t hash64 = hash<uint32_t, uint64_t>( value );

		std::cout << "Input: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << value << "\n";
		std::cout << "32-bit hash: 0x" << std::setw( 8 ) << hash32 << "\n";
		std::cout << "64-bit hash: 0x" << std::setw( 16 ) << hash64 << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 3. Hash combining with combine()
	//=====================================================================
	{
		std::cout << "3. Hash combining with combine()\n";
		std::cout << "---------------------------------\n";

		uint32_t hash1 = hash<std::string_view, uint32_t>( "user" );
		uint32_t hash2 = hash<int, uint32_t>( 12345 );
		uint32_t combined = combine<uint32_t>( hash1, hash2 );

		std::cout << "Hash 1 (\"user\"): 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << hash1 << "\n";
		std::cout << "Hash 2 (12345):  0x" << std::setw( 8 ) << hash2 << "\n";
		std::cout << "Combined:        0x" << std::setw( 8 ) << combined << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 4. Manual CRC32-C construction
	//=====================================================================
	{
		std::cout << "4. Manual CRC32-C construction\n";
		std::cout << "-------------------------------\n";

		std::string_view text = "test";
		uint32_t hashValue = constants::FNV_OFFSET_BASIS_32;

		std::cout << "Input: \"" << text << "\"\n";
		std::cout << "Initial: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << hashValue << "\n";

		for ( char ch : text )
		{
			hashValue = crc32c( hashValue, static_cast<uint8_t>( ch ) );
			std::cout << "  '" << ch << "' -> 0x" << std::setw( 8 ) << hashValue << "\n";
		}

		uint32_t libraryHash = hash<std::string_view>( text );
		std::cout << "Final:   0x" << std::setw( 8 ) << hashValue << "\n";
		std::cout << "Library: 0x" << std::setw( 8 ) << libraryHash << "\n";
		std::cout << "Match: " << ( hashValue == libraryHash ? "YES" : "NO" ) << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 5. Manual FNV-1a construction
	//=====================================================================
	{
		std::cout << "5. Manual FNV-1a construction\n";
		std::cout << "------------------------------\n";

		std::string_view text = "test";
		uint32_t hashValue = constants::FNV_OFFSET_BASIS_32;

		std::cout << "Input: \"" << text << "\"\n";
		std::cout << "Initial: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << hashValue << "\n";

		for ( char ch : text )
		{
			hashValue = fnv1a<uint32_t>( hashValue, static_cast<uint8_t>( ch ) );
			std::cout << "  '" << ch << "' -> 0x" << std::setw( 8 ) << hashValue << "\n";
		}

		std::cout << "Final: 0x" << std::setw( 8 ) << hashValue << std::dec << "\n";
		std::cout << "Note: FNV-1a produces different hashes than CRC32-C\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 6. Hash algorithm constants
	//=====================================================================
	{
		std::cout << "6. Hash algorithm constants\n";
		std::cout << "----------------------------\n";

		std::cout << "FNV-1a Offset Basis (32-bit): 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << constants::FNV_OFFSET_BASIS_32 << "\n";
		std::cout << "FNV-1a Prime (32-bit):        0x" << std::setw( 8 ) << constants::FNV_PRIME_32 << "\n";
		std::cout << "Golden Ratio (32-bit):        0x" << std::setw( 8 ) << constants::GOLDEN_RATIO_32 << "\n";
		std::cout << "FNV-1a Offset Basis (64-bit): 0x" << std::setw( 16 ) << constants::FNV_OFFSET_BASIS_64 << "\n";
		std::cout << "FNV-1a Prime (64-bit):        0x" << std::setw( 16 ) << constants::FNV_PRIME_64 << "\n";
		std::cout << "Golden Ratio (64-bit):        0x" << std::setw( 16 ) << constants::GOLDEN_RATIO_64 << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 7. Seed mixing for hash table probing
	//=====================================================================
	{
		std::cout << "7. Seed mixing for hash table probing\n";
		std::cout << "--------------------------------------\n";

		uint32_t originalHash = 0xABCDEF01;
		uint32_t tableSize = 256;

		std::cout << "Original hash: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << originalHash << "\n";
		std::cout << "Table size: " << std::dec << tableSize << "\n\n";

		std::cout << "Probe sequence (collision resolution):\n";
		for ( uint32_t seed = 0; seed < 5; ++seed )
		{
			uint32_t slot = seedMix<uint32_t>( seed, originalHash, tableSize );
			std::cout << "  Probe " << seed << " -> slot " << slot << " (0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << slot << std::dec << ")\n";
		}
		std::cout << "Note: Different slots for collision resolution\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 8. Composite key hashing (manual)
	//=====================================================================
	{
		std::cout << "8. Composite key hashing (manual)\n";
		std::cout << "----------------------------------\n";

		std::string firstName = "John";
		std::string lastName = "Doe";
		int age = 30;

		uint32_t h = hash<std::string, uint32_t>( firstName );
		h = combine<uint32_t>( h, hash<std::string, uint32_t>( lastName ) );
		h = combine<uint32_t>( h, hash<int, uint32_t>( age ) );

		std::cout << "First name: \"" << firstName << "\"\n";
		std::cout << "Last name:  \"" << lastName << "\"\n";
		std::cout << "Age:        " << age << "\n";
		std::cout << "Combined hash: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << h << std::dec << "\n";
		std::cout << "\n";
	}

	return 0;
}
