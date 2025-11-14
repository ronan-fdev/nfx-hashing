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
 * @file Hasher.h
 * @brief STL-compatible hash functor for use with unordered containers
 * @details Provides Hasher<HashType, Seed> - a general-purpose hash functor supporting
 *          strings, integers, floats, pointers, enums, pairs, tuples, arrays, and custom types
 */

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <tuple>
#include <variant>
#include <vector>

#include "Concepts.h"

#include "Constants.h"

namespace nfx::hashing
{
	//=====================================================================
	// Type trait helpers for SFINAE
	//=====================================================================

	template <typename T>
	struct is_std_pair : std::false_type
	{
	};

	template <typename T1, typename T2>
	struct is_std_pair<std::pair<T1, T2>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_tuple : std::false_type
	{
	};

	template <typename... Ts>
	struct is_std_tuple<std::tuple<Ts...>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_array : std::false_type
	{
	};

	template <typename T, std::size_t N>
	struct is_std_array<std::array<T, N>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_vector : std::false_type
	{
	};

	template <typename T>
	struct is_std_vector<std::vector<T>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_span : std::false_type
	{
	};

	template <typename T, std::size_t Extent>
	struct is_std_span<std::span<T, Extent>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_optional : std::false_type
	{
	};

	template <typename T>
	struct is_std_optional<std::optional<T>> : std::true_type
	{
	};

	template <typename T>
	struct is_std_variant : std::false_type
	{
	};

	template <typename... Ts>
	struct is_std_variant<std::variant<Ts...>> : std::true_type
	{
	};

	//=====================================================================
	// General-purpose STL-compatible hash functor
	//=====================================================================

	/**
	 * @brief General-purpose STL-compatible hash functor supporting multiple types
	 * @tparam HashType Hash value type - must be uint32_t or uint64_t (default: uint32_t)
	 * @tparam Seed Initial seed value for hash calculation (default: FNV_OFFSET_BASIS_32 for 32-bit, FNV_OFFSET_BASIS_64 for 64-bit)
	 *
	 * @details This functor provides a unified hashing interface compatible with STL containers
	 *          like std::unordered_map and std::unordered_set. It supports transparent lookup
	 *          via the `is_transparent` type alias, allowing heterogeneous key comparisons.
	 *
	 *          **Supported Types:**
	 *          - **Strings**: std::string, std::string_view, const char* → CRC32-C with SSE4.2 hardware acceleration (requires `-march=native`/`-msse4.2` or `/arch:AVX`)
	 *          - **Integers**: All integral types → uses multiplicative hashing (Knuth/Wang)
	 *          - **Pointers**: Generic pointers → hashes the address as uintptr_t
	 *          - **Floating-point**: float, double → normalizes special values (+0/-0, NaN) and hashes bit representation
	 *          - **Enums**: Converts to underlying integral type and hashes
	 *          - **Pairs**: std::pair<T1, T2> → hashes both elements and combines them
	 *          - **Tuples**: std::tuple<Ts...> → hashes all elements and combines them
	 *          - **Arrays**: std::array<T, N> → hashes all elements and combines them
	 *          - **Vectors**: std::vector<T> → hashes size and all elements
	 *          - **Spans**: std::span<T> → hashes all elements in the contiguous view
	 *          - **Optionals**: std::optional<T> → hashes nullopt state or contained value
	 *          - **Variants**: std::variant<Ts...> → hashes index and active alternative
	 *          - **Custom types**: Falls back to std::hash<T> for unknown types
	 *
	 *          **Usage Example:**
	 *          @code
	 *          // 32-bit hash with default seed
	 *          std::unordered_map<std::string, int, nfx::hashing::Hasher<>> map32;
	 *
	 *          // 64-bit hash with default seed (automatically uses FNV_OFFSET_BASIS_64)
	 *          std::unordered_map<std::string, int, nfx::hashing::Hasher<uint64_t>> map64;
	 *
	 *          // Transparent lookup (find with string_view in string-keyed map)
	 *          std::unordered_map<std::string, int, nfx::hashing::Hasher<>, std::equal_to<>> transparentMap;
	 *          std::string_view key = "lookup";
	 *          auto it = transparentMap.find(key); // No temporary string allocation
	 *          @endcode
	 */
	template <Hash32or64 HashType = uint32_t, HashType Seed = ( sizeof( HashType ) == 4 ? constants::FNV_OFFSET_BASIS_32 : constants::FNV_OFFSET_BASIS_64 )>
	struct Hasher final
	{
		//----------------------------------------------
		// Transparent lookup support
		//----------------------------------------------

		/**
		 * @brief Enables transparent lookup in STL containers
		 * @details Allows heterogeneous lookup without temporary object creation.
		 */
		using is_transparent = void;

		//----------------------------------------------
		// String type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::string_view using CRC32-C algorithm
		 * @param key String view to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline HashType operator()( std::string_view key ) const noexcept;

		/**
		 * @brief Hashes a std::string using CRC32-C algorithm
		 * @param key String to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline HashType operator()( const std::string& key ) const noexcept;

		/**
		 * @brief Hashes a C-style string using CRC32-C algorithm
		 * @param key Null-terminated string to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline HashType operator()( const char* key ) const noexcept;

		//----------------------------------------------
		// Integer type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes an integral type using multiplicative hashing
		 * @tparam TKey Integral type
		 * @param key Integer value to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename TKey>
		[[nodiscard]] inline std::enable_if_t<std::is_integral_v<TKey>, HashType> operator()( const TKey& key ) const noexcept;

		//----------------------------------------------
		// Floating-point type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a floating-point value with normalization
		 * @tparam T Floating-point type
		 * @param value Floating-point value to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] inline std::enable_if_t<std::is_floating_point_v<T>, HashType> operator()( T value ) const noexcept;

		//----------------------------------------------
		// Pointer type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a pointer by its address
		 * @tparam T Pointer type
		 * @param ptr Pointer to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] inline std::enable_if_t<std::is_pointer_v<T> && !std::is_same_v<T, const char*> && !std::is_same_v<T, char*>, HashType> operator()( T ptr ) const noexcept;

		//----------------------------------------------
		// Enum type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes an enum by its underlying integral value
		 * @tparam TKey Enum type
		 * @param key Enum value to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename TKey>
		[[nodiscard]] inline std::enable_if_t<std::is_enum_v<TKey>, HashType> operator()( const TKey& key ) const noexcept;

		//----------------------------------------------
		// std::array type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::array by combining hashes of all elements
		 * @tparam T Element type
		 * @tparam N Array size
		 * @param arr Array to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T, size_t N>
		[[nodiscard]] inline HashType operator()( const std::array<T, N>& arr ) const noexcept;

		//----------------------------------------------
		// std::optional type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::optional - nullopt has distinct hash from any value
		 * @tparam T Contained type
		 * @param opt Optional to hash
		 * @return Hash value (distinct for nullopt vs any contained value)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] inline HashType operator()( const std::optional<T>& opt ) const noexcept;

		//----------------------------------------------
		// std::pair type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::pair by combining hashes of both elements
		 * @tparam T1 Type of first element
		 * @tparam T2 Type of second element
		 * @param p Pair to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T1, typename T2>
		[[nodiscard]] inline HashType operator()( const std::pair<T1, T2>& p ) const noexcept;

		//----------------------------------------------
		// std::span type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::span by combining hashes of all elements in the view
		 * @tparam T Element type
		 * @tparam Extent Static extent (std::dynamic_extent for dynamic spans)
		 * @param sp Span to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T, std::size_t Extent = std::dynamic_extent>
		[[nodiscard]] inline HashType operator()( std::span<T, Extent> sp ) const noexcept;

		//----------------------------------------------
		// std::tuple type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::tuple by combining hashes of all elements
		 * @tparam Ts Types of tuple elements
		 * @param t Tuple to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename... Ts>
		[[nodiscard]] inline HashType operator()( const std::tuple<Ts...>& t ) const noexcept;

		//----------------------------------------------
		// std::variant type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::variant by combining index and active alternative's hash
		 * @tparam Ts Alternative types
		 * @param var Variant to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename... Ts>
		[[nodiscard]] inline HashType operator()( const std::variant<Ts...>& var ) const noexcept;

		//----------------------------------------------
		// std::vector type overloads
		//----------------------------------------------

		/**
		 * @brief Hashes a std::vector by combining size and hashes of all elements
		 * @tparam T Element type
		 * @param vec Vector to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T>
		[[nodiscard]] inline HashType operator()( const std::vector<T>& vec ) const noexcept;

		//----------------------------------------------
		// Custom type fallback
		//----------------------------------------------

		/**
		 * @brief Hashes custom types using std::hash fallback
		 * @tparam TKey Custom type
		 * @param key Custom object to hash
		 * @return Hash value
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 * @warning Hash quality depends on std::hash<TKey> implementation quality.
		 *          Some STL implementations have poor std::hash (e.g., identity function for pointers).
		 *          For critical custom types, consider specializing Hasher<> or providing a custom hash function.
		 */
		template <typename TKey>
		[[nodiscard]] inline std::enable_if_t<!std::is_same_v<std::decay_t<TKey>, std::string_view> &&
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
		operator()( const TKey& key ) const noexcept;
	};
} // namespace nfx::hashing

#include "nfx/detail/hashing/Hasher.inl"
