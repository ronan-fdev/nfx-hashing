/**
 * @file Sample_AdvancedHashing.cpp
 * @brief Demonstrates advanced hashing patterns and custom hash functors
 * @details This sample shows custom hash structs, hash table integration,
 *          transparent lookup, string deduplication, and practical hashing
 *          patterns for real-world applications.
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <nfx/Hashing.h>

using namespace nfx::hashing;

int main()
{
	std::cout << "=== nfx-hashing Advanced Hashing Patterns ===\n\n";

	//=====================================================================
	// 1. Custom hash functor for string_view
	//=====================================================================
	{
		std::cout << "1. Custom hash functor for string_view\n";
		std::cout << "---------------------------------------\n";

		struct StringViewHash
		{
			uint32_t operator()( std::string_view sv ) const noexcept
			{
				return Hasher<uint32_t>{}( sv );
			}
		};

		std::unordered_map<std::string_view, int, StringViewHash> ages;
		ages["Alice"] = 30;
		ages["Bob"] = 25;
		ages["Charlie"] = 35;

		std::cout << "Age lookup:\n";
		for ( const auto& [name, age] : ages )
		{
			std::cout << "  " << name << " -> " << age << " years\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 2. Custom hash for composite struct
	//=====================================================================
	{
		std::cout << "2. Custom hash for composite struct\n";
		std::cout << "------------------------------------\n";

		struct Point
		{
			int x;
			int y;

			bool operator==( const Point& other ) const
			{
				return x == other.x && y == other.y;
			}
		};

		struct PointHash
		{
			uint32_t operator()( const Point& p ) const noexcept
			{
				uint32_t h = hash<int, uint32_t>( p.x );
				return combine<uint32_t>( h, hash<int, uint32_t>( p.y ) );
			}
		};

		std::unordered_map<Point, std::string, PointHash> locations;
		locations[{ 0, 0 }] = "Origin";
		locations[{ 1, 0 }] = "East";
		locations[{ 0, 1 }] = "North";
		locations[{ 1, 1 }] = "Northeast";

		std::cout << "Grid locations:\n";
		for ( const auto& [point, label] : locations )
		{
			std::cout << "  (" << point.x << ", " << point.y << ") -> " << label << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 3. String deduplication with hash set
	//=====================================================================
	{
		std::cout << "3. String deduplication with hash set\n";
		std::cout << "--------------------------------------\n";

		std::string_view words[] = { "apple", "banana", "apple", "cherry", "banana", "date", "apple" };

		struct StringViewHash
		{
			uint32_t operator()( std::string_view sv ) const noexcept
			{
				return Hasher<uint32_t>{}( sv );
			}
		};

		std::unordered_set<std::string_view, StringViewHash> uniqueWords;
		for ( auto word : words )
		{
			uniqueWords.insert( word );
		}

		std::cout << "Input: " << words[0];
		for ( size_t i = 1; i < std::size( words ); ++i )
		{
			std::cout << ", " << words[i];
		}
		std::cout << "\n";

		std::cout << "Unique words (" << uniqueWords.size() << "):\n";
		for ( const auto& word : uniqueWords )
		{
			std::cout << "  " << word << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 4. Hash-based word frequency counter
	//=====================================================================
	{
		std::cout << "4. Hash-based word frequency counter\n";
		std::cout << "-------------------------------------\n";

		std::string text = "the quick brown fox jumps over the lazy dog and the fox runs.";

		struct StringViewHash
		{
			uint32_t operator()( std::string_view sv ) const noexcept
			{
				return Hasher<uint32_t>{}( sv );
			}
		};

		std::unordered_map<std::string_view, int, StringViewHash> wordCount;

		size_t start = 0;
		for ( size_t i = 0; i <= text.size(); ++i )
		{
			if ( i == text.size() || text[i] == ' ' || text[i] == '.' || text[i] == ',' )
			{
				if ( i > start )
				{
					std::string_view word{ text.data() + start, i - start };
					wordCount[word]++;
				}
				start = i + 1;
			}
		}

		std::cout << "Text: \"" << text << "\"\n\n";
		std::cout << "Word frequencies:\n";
		for ( const auto& [word, count] : wordCount )
		{
			std::cout << "  " << word << ": " << count << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 5. Fast permission lookup (O(1) vs O(n))
	//=====================================================================
	{
		std::cout << "5. Fast permission lookup (O(1) vs O(n))\n";
		std::cout << "-----------------------------------------\n";

		struct StringViewHash
		{
			uint32_t operator()( std::string_view sv ) const noexcept
			{
				return Hasher<uint32_t>{}( sv );
			}
		};

		std::unordered_set<std::string_view, StringViewHash> adminUsers;
		adminUsers.insert( "alice" );
		adminUsers.insert( "bob" );
		adminUsers.insert( "charlie" );

		std::string_view usersToCheck[] = { "alice", "dave", "bob", "eve", "charlie" };

		std::cout << "Admin users: alice, bob, charlie\n";
		std::cout << "Checking permissions (O(1) hash lookup):\n";

		for ( auto user : usersToCheck )
		{
			bool isAdmin = adminUsers.contains( user );
			std::cout << "  " << user << ": " << ( isAdmin ? "ADMIN" : "USER" ) << "\n";
		}

		std::cout << "\nNote: Hash table provides O(1) lookup vs O(n) linear search\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 6. Multi-field composite key hashing
	//=====================================================================
	{
		std::cout << "6. Multi-field composite key hashing\n";
		std::cout << "-------------------------------------\n";

		struct Employee
		{
			std::string department;
			std::string role;
			int level;

			bool operator==( const Employee& other ) const
			{
				return department == other.department && role == other.role && level == other.level;
			}
		};

		struct EmployeeHash
		{
			uint32_t operator()( const Employee& e ) const noexcept
			{
				uint32_t h = hash<std::string, uint32_t>( e.department );
				h = combine<uint32_t>( h, hash<std::string, uint32_t>( e.role ) );
				h = combine<uint32_t>( h, hash<int, uint32_t>( e.level ) );
				return h;
			}
		};

		std::unordered_map<Employee, int, EmployeeHash> salaries;
		salaries[{ "Engineering", "Developer", 3 }] = 95000;
		salaries[{ "Engineering", "Manager", 5 }] = 120000;
		salaries[{ "Sales", "Representative", 2 }] = 65000;

		std::cout << "Salary lookup by composite key:\n";
		for ( const auto& [emp, salary] : salaries )
		{
			std::cout << "  " << emp.department << " / " << emp.role << " (L" << emp.level << ") -> $" << salary << "\n";
		}
		std::cout << "\n";
	}

	//=====================================================================
	// 7. Transparent lookup with Hasher functor
	//=====================================================================
	{
		std::cout << "7. Transparent lookup with Hasher functor\n";
		std::cout << "------------------------------------------\n";

		std::unordered_set<std::string, Hasher<>, std::equal_to<>> names;
		names.insert( "Alice" );
		names.insert( "Bob" );
		names.insert( "Charlie" );

		std::string_view searchName = "Bob";
		bool found = names.contains( searchName );

		std::cout << "Names in set: ";
		for ( const auto& name : names )
		{
			std::cout << name << " ";
		}
		std::cout << "\n";

		std::cout << "Searching for \"" << searchName << "\": " << ( found ? "FOUND" : "NOT FOUND" ) << "\n";
		std::cout << "Note: Heterogeneous lookup with string_view (no temporary string)\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 8. Cache key generation
	//=====================================================================
	{
		std::cout << "8. Cache key generation\n";
		std::cout << "-----------------------\n";

		std::string username = "alice";
		std::string resource = "profile";
		int version = 2;

		uint32_t cacheKey = hash<std::string, uint32_t>( username );
		cacheKey = combine<uint32_t>( cacheKey, hash<std::string, uint32_t>( resource ) );
		cacheKey = combine<uint32_t>( cacheKey, hash<int, uint32_t>( version ) );

		std::cout << "Cache key components:\n";
		std::cout << "  Username: \"" << username << "\"\n";
		std::cout << "  Resource: \"" << resource << "\"\n";
		std::cout << "  Version:  " << version << "\n";
		std::cout << "Generated cache key: 0x" << std::hex << cacheKey << std::dec << "\n";
		std::cout << "\n";
	}

	//=====================================================================
	// 9. Hash distribution analysis
	//=====================================================================
	{
		std::cout << "9. Hash distribution analysis\n";
		std::cout << "------------------------------\n";

		std::string_view similarStrings[] = {
			"user_001",
			"user_002",
			"user_003",
			"user_010",
			"User_001" };

		std::cout << "Hash values for similar strings (avalanche effect):\n";
		for ( const auto& str : similarStrings )
		{
			uint32_t h = hash<std::string_view, uint32_t>( str );
			std::cout << "  \"" << str << "\" -> 0x" << std::hex << h << std::dec << "\n";
		}
		std::cout << "\nNote: Small input changes produce very different hashes\n";
		std::cout << "\n";
	}

	return 0;
}
