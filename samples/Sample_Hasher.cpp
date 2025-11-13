/**
 * @file Sample_Hasher.cpp
 * @brief Demonstrates hashing utilities with the Hasher functor
 * @details This sample shows examples covering basic types (strings, integers),
 *          composite keys (pairs, tuples, arrays), containers (vectors, optionals,
 *          variants), 64-bit hashing, custom seeds, transparent lookup, and nested
 *          structures. Uses CRC32-C with hardware acceleration when available.
 */

#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <nfx/Hashing.h>

using namespace nfx::hashing;

int main()
{
	std::cout << "=== nfx-hashing Hasher Examples ===\n\n";

	//=====================================================================
	// 1. Basic string hashing
	//=====================================================================
	{
		std::cout << "1. String hashing with std::unordered_map\n";
		std::cout << "-------------------------------------------\n";

		// Using Hasher functor with default configuration:
		//   Hasher<> = Hasher<uint32_t, FNV_OFFSET_BASIS_32>
		//   - HashType: uint32_t (32-bit hash output)
		//   - Seed: FNV_OFFSET_BASIS_32 (0x811C9DC5)
		std::unordered_map<std::string, int, Hasher<>> nameToAge;

		nameToAge["Alice"] = 30;
		nameToAge["Bob"] = 25;
		nameToAge["Charlie"] = 35;

		std::cout << "Name-to-Age map:\n";
		for ( const auto& [name, age] : nameToAge )
		{
			std::cout << "  " << name << " -> " << age << " years\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 2. Integer hashing
	//=====================================================================
	{
		std::cout << "2. Integer hashing with std::unordered_set\n";
		std::cout << "-------------------------------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_set<int, Hasher<>> uniqueIds{ 100, 200, 300, 100 }; // Duplicate 100 ignored

		std::cout << "Unique IDs: ";
		for ( int id : uniqueIds )
		{
			std::cout << id << " ";
		}
		std::cout << "\n\n";
	}

	//=====================================================================
	// 3. Pair hashing (composite keys)
	//=====================================================================
	{
		std::cout << "3. Pair hashing for composite keys\n";
		std::cout << "-----------------------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_map<std::pair<int, int>, std::string, Hasher<>> grid;

		grid[{ 0, 0 }] = "Origin";
		grid[{ 1, 0 }] = "East";
		grid[{ 0, 1 }] = "North";
		grid[{ 1, 1 }] = "Northeast";

		std::cout << "Grid positions:\n";
		for ( const auto& [pos, label] : grid )
		{
			std::cout << "  (" << pos.first << ", " << pos.second << ") -> " << label << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 4. Tuple hashing (multi-field keys)
	//=====================================================================
	{
		std::cout << "4. Tuple hashing for multi-field keys\n";
		std::cout << "--------------------------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_map<std::tuple<int, int, int>, std::string, Hasher<>> events;

		events[{ 2025, 11, 11 }] = "Hash quality release";
		events[{ 2025, 11, 8 }] = "Container support release";
		events[{ 2025, 11, 1 }] = "Initial release";

		std::cout << "Event calendar:\n";
		for ( const auto& [date, event] : events )
		{
			std::cout << "  " << std::get<0>( date ) << "-"
					  << std::get<1>( date ) << "-"
					  << std::get<2>( date ) << " -> " << event << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 5. Array hashing (fixed-size sequences)
	//=====================================================================
	{
		std::cout << "5. Array hashing for Fixed-size sequences\n";
		std::cout << "------------------------------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_map<std::array<int, 3>, std::string, Hasher<>> colors;

		colors[{ 255, 0, 0 }] = "Red";
		colors[{ 0, 255, 0 }] = "Green";
		colors[{ 0, 0, 255 }] = "Blue";
		colors[{ 255, 255, 0 }] = "Yellow";

		std::cout << "RGB color palette:\n";
		for ( const auto& [rgb, name] : colors )
		{
			std::cout << "  RGB(" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ") -> " << name << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 6. Vector hashing (dynamic sequences)
	//=====================================================================
	{
		std::cout << "6. Vector hashing for dynamic sequences\n";
		std::cout << "----------------------------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_map<std::vector<std::string>, std::string, Hasher<>> roles;

		roles[{ "read", "write", "execute" }] = "Admin";
		roles[{ "read", "write" }] = "Editor";
		roles[{ "read" }] = "Viewer";

		std::cout << "Permission roles:\n";
		for ( const auto& [perms, role] : roles )
		{
			std::cout << "  [ ";
			for ( const auto& perm : perms )
			{
				std::cout << perm << " ";
			}
			std::cout << "] -> " << role << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 7. Optional hashing
	//=====================================================================
	{
		std::cout << "7. Optional hashing\n";
		std::cout << "-------------------\n";

		// Default config: 32-bit hash with FNV seed
		std::unordered_map<std::optional<int>, std::string, Hasher<>> config;

		config[100] = "Custom timeout";
		config[std::nullopt] = "Default timeout";

		std::cout << "Configuration:\n";
		for ( const auto& [timeout, description] : config )
		{
			if ( timeout.has_value() )
			{
				std::cout << "  Timeout(" << *timeout << "ms) -> " << description << "\n";
			}
			else
			{
				std::cout << "  Timeout(none) -> " << description << "\n";
			}
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 8. Variant hashing
	//=====================================================================
	{
		std::cout << "8. Variant hashing\n";
		std::cout << "------------------\n";

		// Hash variants (e.g., polymorphic value -> handler)
		// NOTE: Variant hashing includes both the type index AND the value
		//       so variant<int>(42) != variant<double>(42.0) in hash value
		using Value = std::variant<int, double, std::string>;
		std::unordered_map<Value, std::string, Hasher<>> handlers;

		handlers[42] = "Integer handler";
		handlers[3.14] = "Double handler";
		handlers[std::string{ "text" }] = "String handler";

		std::cout << "Value handlers:\n";
		for ( const auto& [value, handler] : handlers )
		{
			std::cout << "  Value(index=" << value.index() << ") -> " << handler << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 9. 64-bit hashing for large datasets
	//=====================================================================
	{
		std::cout << "9. 64-bit hashing for large Datasets\n";
		std::cout << "-------------------------------------\n";

		// Use 64-bit hash for reduced collision probability
		// Template parameter: Hasher<uint64_t> = 64-bit hash output (instead of default uint32_t)
		std::unordered_map<std::string, uint64_t, Hasher<uint64_t>> bigDataIds;

		bigDataIds["dataset_1"] = 1ULL << 40; // 1 TB = 2^40 bytes = 1,099,511,627,776 bytes
		bigDataIds["dataset_2"] = 1ULL << 50; // 1 PB = 2^50 bytes = 1,125,899,906,842,624 bytes

		std::cout << "Large dataset IDs (using 64-bit hashes):\n";
		for ( const auto& [name, size] : bigDataIds )
		{
			std::cout << "  " << name << " -> " << size << " bytes\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 10. Custom seed for deterministic Hashing
	//=====================================================================
	{
		std::cout << "10. Custom seed for deterministic hashing\n";
		std::cout << "------------------------------------------\n";

		// Use custom seed for reproducible results across runs
		constexpr uint32_t customSeed = 0xDEADBEEF;
		std::unordered_set<std::string, Hasher<uint32_t, customSeed>> deterministicSet;

		deterministicSet.insert( "reproducible" );
		deterministicSet.insert( "deterministic" );
		deterministicSet.insert( "consistent" );

		std::cout << "Deterministic hash set (custom seed 0xDEADBEEF):\n";
		for ( const auto& word : deterministicSet )
		{
			std::cout << "  " << word << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 11. Transparent (heterogeneous) lookup
	//=====================================================================
	{
		std::cout << "11. Transparent (heterogeneous) lookup (avoid temporary std::string)\n";
		std::cout << "-----------------------------------------------------\n";

		// Using transparent lookup to avoid temporary string allocations
		std::unordered_set<std::string, Hasher<>, std::equal_to<>> names;

		names.insert( "Alice" );
		names.insert( "Bob" );
		names.insert( "Charlie" );

		// Lookup with string_view (no temporary std::string allocation)
		std::string_view searchName = "Bob";
		if ( names.contains( searchName ) )
		{
			std::cout << "  Found: " << searchName << " (using string_view, no allocation)\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 12. Nested containers
	//=====================================================================
	{
		std::cout << "12. Nested containers (vector of tuples)\n";
		std::cout << "----------------------------------------\n";

		// Hash complex nested structures
		using PointList = std::vector<std::tuple<int, int>>;
		std::unordered_map<PointList, std::string, Hasher<>> polygons;

		polygons[{ { 0, 0 }, { 1, 0 }, { 1, 1 } }] = "Triangle";
		polygons[{ { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }] = "Square";

		std::cout << "Polygon shapes:\n";
		for ( const auto& [points, shape] : polygons )
		{
			std::cout << "  " << shape << " with " << points.size() << " vertices\n";
		}
		std::cout << "\n";
	}

	return 0;
}
