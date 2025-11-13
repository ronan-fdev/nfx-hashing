/**
 * @file TESTS_HashQuality.cpp
 * @brief Hash quality analysis tests
 * @details Advanced tests for hash algorithm quality including avalanche effect, chi-squared distribution,
 *          bit independence, and collision rates under adversarial inputs
 */

#include <bit>
#include <unordered_set>

#include <gtest/gtest.h>

#include <nfx/Hashing.h>

namespace nfx::hashing::test
{
	using namespace nfx::hashing;
	using namespace nfx::hashing::constants;

	//=====================================================================
	// Hash quality analysis
	//=====================================================================

	//----------------------------------------------
	// Distribution quality
	//----------------------------------------------

	TEST( HashQuality, StringDistribution )
	{
		// Generate hashes for many strings
		std::vector<std::string> testStrings;
		for ( int i = 0; i < 1000; ++i )
		{
			testStrings.push_back( "test_string_" + std::to_string( i ) );
		}

		std::unordered_set<uint32_t> uniqueHashes;
		for ( const auto& str : testStrings )
		{
			uint32_t hashValue = hash<std::string>( str );
			uniqueHashes.insert( hashValue );
		}

		// Should have no collisions - perfect distribution for this set
		EXPECT_EQ( uniqueHashes.size(), 1000u );
	}

	TEST( HashQuality, IntegerDistribution )
	{
		std::unordered_set<size_t> uniqueHashes;

		for ( int32_t i = 0; i < 1000; ++i )
		{
			size_t hashValue = hash<int>( i );
			uniqueHashes.insert( hashValue );
		}

		EXPECT_EQ( uniqueHashes.size(), 1000u );
	}

	TEST( HashQuality, StringDistribution64 )
	{
		// Generate hashes for many strings using 64-bit
		std::vector<std::string> testStrings;
		for ( int i = 0; i < 1000; ++i )
		{
			testStrings.push_back( "test_string_64bit_" + std::to_string( i ) );
		}

		std::unordered_set<uint64_t> uniqueHashes;
		for ( const auto& str : testStrings )
		{
			uint64_t hashValue = hash<std::string, uint64_t>( str );
			uniqueHashes.insert( hashValue );
		}

		// Should have no collisions - perfect distribution
		EXPECT_EQ( uniqueHashes.size(), 1000u );
	}

	TEST( HashQuality, IntegerDistribution64 )
	{
		std::unordered_set<uint64_t> uniqueHashes;

		for ( int64_t i = 0; i < 1000; ++i )
		{
			uint64_t hashValue = hash<int64_t, uint64_t>( i );
			uniqueHashes.insert( hashValue );
		}

		// Should have no collisions
		EXPECT_EQ( uniqueHashes.size(), 1000u );
	}

	//----------------------------------------------
	// Avalanche effect - Flipping 1 input bit should flip ~50% output bits
	//----------------------------------------------

	TEST( HashQuality, AvalancheEffectString32 )
	{
		// Test avalanche effect for string hashing (32-bit)
		// Flipping a single input bit should cause approximately 50% of output bits to flip
		std::string baseString = "avalanche_test_string";

		uint32_t baseHash = hash<std::string>( baseString );

		int totalBitFlips = 0;
		int totalTests = 0;

		// Test flipping each bit in each character
		for ( size_t charIdx = 0; charIdx < baseString.size(); ++charIdx )
		{
			for ( int bitIdx = 0; bitIdx < 8; ++bitIdx )
			{
				std::string modifiedString = baseString;
				modifiedString[charIdx] ^= ( 1 << bitIdx ); // Flip one bit

				uint32_t modifiedHash = hash<std::string>( modifiedString );
				uint32_t diff = baseHash ^ modifiedHash;

				// Count flipped bits using popcount
				int flippedBits = std::popcount( diff );
				totalBitFlips += flippedBits;
				totalTests++;
			}
		}

		double avgFlippedBits = static_cast<double>( totalBitFlips ) / totalTests;

		// Ideal avalanche: 16 bits flipped (50% of 32 bits)
		// Accept range: 12-20 bits (37.5% - 62.5%)
		EXPECT_GE( avgFlippedBits, 12.0 ) << "Poor avalanche: too few bits flipped";
		EXPECT_LE( avgFlippedBits, 20.0 ) << "Poor avalanche: too many bits flipped";
		std::cout << "avgFlippedBits: " << avgFlippedBits << std::endl;
		std::cout << "avgFlippedBits: " << avgFlippedBits << std::endl;
	}

	TEST( HashQuality, AvalancheEffectString64 )
	{
		// Test avalanche effect for 64-bit string hashing
		std::string baseString = "avalanche_test_64bit";

		uint64_t baseHash = hash<std::string, uint64_t>( baseString );

		int totalBitFlips = 0;
		int totalTests = 0;

		for ( size_t charIdx = 0; charIdx < baseString.size(); ++charIdx )
		{
			for ( int bitIdx = 0; bitIdx < 8; ++bitIdx )
			{
				std::string modifiedString = baseString;
				modifiedString[charIdx] ^= ( 1 << bitIdx );

				uint64_t modifiedHash = hash<std::string, uint64_t>( modifiedString );
				uint64_t diff = baseHash ^ modifiedHash;

				int flippedBits = std::popcount( diff );
				totalBitFlips += flippedBits;
				totalTests++;
			}
		}

		double avgFlippedBits = static_cast<double>( totalBitFlips ) / totalTests;

		// Ideal avalanche: 32 bits flipped (50% of 64 bits)
		// Accept range: 24-40 bits (37.5% - 62.5%)
		EXPECT_GE( avgFlippedBits, 24.0 ) << "Poor avalanche: too few bits flipped";
		EXPECT_LE( avgFlippedBits, 40.0 ) << "Poor avalanche: too many bits flipped";
	}

	TEST( HashQuality, AvalancheEffectInteger32 )
	{
		// Test avalanche effect for 32-bit integer hashing
		uint32_t baseValue = 0x12345678;
		uint32_t baseHash = hash<uint32_t>( baseValue );

		int totalBitFlips = 0;
		int totalTests = 0;

		// Test flipping each bit in the input
		for ( int bitIdx = 0; bitIdx < 32; ++bitIdx )
		{
			uint32_t modifiedValue = baseValue ^ ( 1u << bitIdx );
			uint32_t modifiedHash = hash<uint32_t>( modifiedValue );
			uint32_t diff = baseHash ^ modifiedHash;

			int flippedBits = std::popcount( diff );
			totalBitFlips += flippedBits;
			totalTests++;
		}

		double avgFlippedBits = static_cast<double>( totalBitFlips ) / totalTests;

		// Ideal: 16 bits (50% of 32)
		// Accept: 12-20 bits (37.5% - 62.5%)
		EXPECT_GE( avgFlippedBits, 12.0 );
		EXPECT_LE( avgFlippedBits, 20.0 );
	}

	TEST( HashQuality, AvalancheEffectInteger64 )
	{
		// Test avalanche effect for 64-bit integer hashing
		uint64_t baseValue = 0xCAFEBABEDEADC0DEULL;
		uint64_t baseHash = hash<uint64_t, uint64_t>( baseValue );

		int totalBitFlips = 0;
		int totalTests = 0;

		for ( int bitIdx = 0; bitIdx < 64; ++bitIdx )
		{
			uint64_t modifiedValue = baseValue ^ ( 1ULL << bitIdx );
			uint64_t modifiedHash = hash<uint64_t, uint64_t>( modifiedValue );
			uint64_t diff = baseHash ^ modifiedHash;

			int flippedBits = std::popcount( diff );
			totalBitFlips += flippedBits;
			totalTests++;
		}

		double avgFlippedBits = static_cast<double>( totalBitFlips ) / totalTests;

		// Ideal: 32 bits (50% of 64)
		// Accept: 24-40 bits (37.5% - 62.5%)
		EXPECT_GE( avgFlippedBits, 24.0 );
		EXPECT_LE( avgFlippedBits, 40.0 );
	}

	//----------------------------------------------
	// Chi-Squared distribution test - Hash outputs should be uniformly distributed
	//----------------------------------------------

	TEST( HashQuality, ChiSquaredUniformityString )
	{
		// Chi-squared test to verify hash outputs are uniformly distributed
		constexpr size_t numBuckets = 256;	 // Divide hash space into 256 buckets
		constexpr size_t numSamples = 10000; // Hash 10,000 strings
		constexpr double expectedPerBucket = static_cast<double>( numSamples ) / numBuckets;

		std::vector<size_t> buckets( numBuckets, 0 );

		// Generate diverse test strings
		for ( size_t i = 0; i < numSamples; ++i )
		{
			std::string testStr = "chi_squared_test_" + std::to_string( i );
			uint32_t hashValue = hash<std::string>( testStr );

			// Map hash to bucket (use low 8 bits)
			size_t bucketIdx = hashValue % numBuckets;
			buckets[bucketIdx]++;
		}

		// Calculate chi-squared statistic
		double chiSquared = 0.0;
		for ( size_t count : buckets )
		{
			double deviation = static_cast<double>( count ) - expectedPerBucket;
			chiSquared += ( deviation * deviation ) / expectedPerBucket;
		}

		// Chi-squared critical value for 255 degrees of freedom at p=0.01 is ~310
		// Accept if chi-squared < 400 (generous threshold for hash quality)
		EXPECT_LT( chiSquared, 400.0 ) << "Poor distribution: chi-squared = " << chiSquared;

		// Also verify no bucket is severely over/under-populated
		for ( size_t count : buckets )
		{
			double ratio = static_cast<double>( count ) / expectedPerBucket;
			EXPECT_GT( ratio, 0.5 ) << "Bucket severely under-populated";
			EXPECT_LT( ratio, 2.0 ) << "Bucket severely over-populated";
		}
	}

	TEST( HashQuality, ChiSquaredUniformityInteger )
	{
		constexpr size_t numBuckets = 256;
		constexpr size_t numSamples = 10000;
		constexpr double expectedPerBucket = static_cast<double>( numSamples ) / numBuckets;

		std::vector<size_t> buckets( numBuckets, 0 );

		for ( int32_t i = 0; i < static_cast<int32_t>( numSamples ); ++i )
		{
			uint32_t hashValue = hash<int>( i );
			size_t bucketIdx = hashValue % numBuckets;
			buckets[bucketIdx]++;
		}

		double chiSquared = 0.0;
		for ( size_t count : buckets )
		{
			double deviation = static_cast<double>( count ) - expectedPerBucket;
			chiSquared += ( deviation * deviation ) / expectedPerBucket;
		}

		EXPECT_LT( chiSquared, 400.0 ) << "Poor distribution: chi-squared = " << chiSquared;

		for ( size_t count : buckets )
		{
			double ratio = static_cast<double>( count ) / expectedPerBucket;
			EXPECT_GT( ratio, 0.5 );
			EXPECT_LT( ratio, 2.0 );
		}
	}

	//----------------------------------------------
	// Bit Independence - Each output bit should depend evenly on input bits
	//----------------------------------------------

	TEST( HashQuality, BitIndependenceString )
	{
		// Test that each output bit position has approximately equal probability
		// of changing when input changes
		constexpr size_t numSamples = 1000;
		std::vector<int> bitFlipCounts( 32, 0 ); // Count flips for each of 32 output bits

		std::string baseString = "bit_independence_base";
		uint32_t baseHash = hash<std::string>( baseString );

		for ( size_t i = 0; i < numSamples; ++i )
		{
			std::string testString = "bit_independence_" + std::to_string( i );
			uint32_t testHash = hash<std::string>( testString );
			uint32_t diff = baseHash ^ testHash;

			// Count which bits are different
			for ( int bitIdx = 0; bitIdx < 32; ++bitIdx )
			{
				if ( diff & ( 1u << bitIdx ) )
				{
					bitFlipCounts[bitIdx]++;
				}
			}
		}

		// Each bit should flip approximately 50% of the time
		// Accept range: 35% - 65% (350-650 flips out of 1000 samples)
		for ( int bitIdx = 0; bitIdx < 32; ++bitIdx )
		{
			EXPECT_GE( bitFlipCounts[bitIdx], 350 ) << "Bit " << bitIdx << " flips too rarely";
			EXPECT_LE( bitFlipCounts[bitIdx], 650 ) << "Bit " << bitIdx << " flips too often";
		}
	}

	TEST( HashQuality, BitIndependenceInteger )
	{
		constexpr size_t numSamples = 1000;
		std::vector<int> bitFlipCounts( 32, 0 );

		uint32_t baseValue = 0xDEADBEEF;
		uint32_t baseHash = hash<uint32_t>( baseValue );

		for ( uint32_t i = 0; i < numSamples; ++i )
		{
			uint32_t testHash = hash<uint32_t>( i );
			uint32_t diff = baseHash ^ testHash;

			for ( int bitIdx = 0; bitIdx < 32; ++bitIdx )
			{
				if ( diff & ( 1u << bitIdx ) )
				{
					bitFlipCounts[bitIdx]++;
				}
			}
		}

		// Each bit should flip 35%-65% of the time
		for ( int bitIdx = 0; bitIdx < 32; ++bitIdx )
		{
			EXPECT_GE( bitFlipCounts[bitIdx], 350 ) << "Bit " << bitIdx << " flips too rarely";
			EXPECT_LE( bitFlipCounts[bitIdx], 650 ) << "Bit " << bitIdx << " flips too often";
		}
	}

	//----------------------------------------------
	// Collision rate under adversarial inputs
	//----------------------------------------------

	TEST( HashQuality, CollisionRateSimilarStrings )
	{
		// Test collision rate for strings that differ by only 1 character
		std::unordered_set<uint32_t> uniqueHashes;
		constexpr size_t numStrings = 1000000;

		// Generate strings like "test_0", "test_1", ..., "test_999"
		for ( size_t i = 0; i < numStrings; ++i )
		{
			std::string str = "test_" + std::to_string( i );
			uint32_t hashValue = hash<std::string>( str );
			uniqueHashes.insert( hashValue );
		}

		// Collision rate should be very low (< 0.1%)
		size_t collisions = numStrings - uniqueHashes.size();
		double collisionRate = static_cast<double>( collisions ) / numStrings;

		EXPECT_LT( collisionRate, 0.001 ) << "High collision rate: " << ( collisionRate * 100.0 ) << "%";
	}

	TEST( HashQuality, CollisionRateSequentialIntegers )
	{
		// Test collision rate for sequential integers
		std::unordered_set<uint32_t> uniqueHashes;
		constexpr size_t numIntegers = 10000000;

		for ( int32_t i = 0; i < static_cast<int32_t>( numIntegers ); ++i )
		{
			uint32_t hashValue = hash<int>( i );
			uniqueHashes.insert( hashValue );
		}

		size_t collisions = numIntegers - uniqueHashes.size();
		double collisionRate = static_cast<double>( collisions ) / numIntegers;

		EXPECT_LT( collisionRate, 0.001 ) << "High collision rate: " << ( collisionRate * 100.0 ) << "%";
	}

	TEST( HashQuality, CollisionRateCommonPrefixes )
	{
		// Test collision rate for strings with common prefixes (adversarial pattern)
		std::unordered_set<uint32_t> uniqueHashes;
		std::string commonPrefix = "common_prefix_";
		constexpr size_t numStrings = 1000000;

		for ( size_t i = 0; i < numStrings; ++i )
		{
			std::string str = commonPrefix + std::to_string( i );
			uint32_t hashValue = hash<std::string>( str );
			uniqueHashes.insert( hashValue );
		}

		size_t collisions = numStrings - uniqueHashes.size();
		double collisionRate = static_cast<double>( collisions ) / numStrings;

		EXPECT_LT( collisionRate, 0.001 ) << "High collision rate with common prefixes: " << ( collisionRate * 100.0 ) << "%";
	}

	TEST( HashQuality, CollisionRateCommonSuffixes )
	{
		// Test collision rate for strings with common suffixes
		std::unordered_set<uint32_t> uniqueHashes;
		std::string commonSuffix = "_common_suffix";
		constexpr size_t numStrings = 1000000;

		for ( size_t i = 0; i < numStrings; ++i )
		{
			std::string str = std::to_string( i ) + commonSuffix;
			uint32_t hashValue = hash<std::string>( str );
			uniqueHashes.insert( hashValue );
		}

		size_t collisions = numStrings - uniqueHashes.size();
		double collisionRate = static_cast<double>( collisions ) / numStrings;

		EXPECT_LT( collisionRate, 0.001 ) << "High collision rate with common suffixes: " << ( collisionRate * 100.0 ) << "%";
	}

	TEST( HashQuality, CollisionRateSmallDifferences )
	{
		// Test strings that differ by single character substitution
		std::unordered_set<uint32_t> uniqueHashes;
		std::string base = "the_quick_brown_fox_jumps_over_the_lazy_dog";

		uniqueHashes.insert( hash<std::string>( base ) );

		size_t totalStrings = 1; // Start with base string

		// Create variations by changing one character at a time
		for ( size_t i = 0; i < base.size(); ++i )
		{
			for ( char c = 'a'; c <= 'z'; ++c )
			{
				if ( base[i] == c )
					continue;

				std::string modified = base;
				modified[i] = c;
				uint32_t hashValue = hash<std::string>( modified );
				uniqueHashes.insert( hashValue );
				totalStrings++;
			}
		}

		// Calculate actual collision count
		size_t collisions = totalStrings - uniqueHashes.size();
		double collisionRate = static_cast<double>( collisions ) / static_cast<double>( totalStrings );

		EXPECT_LT( collisionRate, 0.01 ) << "High collision rate for small differences: " << ( collisionRate * 100.0 ) << "%";
	}
} // namespace nfx::hashing::test
