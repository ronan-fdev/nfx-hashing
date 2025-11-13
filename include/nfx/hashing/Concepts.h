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
 * @file Concepts.h
 * @brief C++20 concepts and type traits for nfx-hashing library
 * @details Declares Hash32or64 concept and related type constraints for hash function templates.
 */

#pragma once

#include <concepts>

namespace nfx::hashing
{
	//=====================================================================
	// C++20 Concepts for type constraints
	//=====================================================================

	/**
	 * @brief Concept requiring type to be either uint32_t or uint64_t
	 * @details Used to constrain hash functions to supported integer sizes
	 */
	template <typename T>
	concept Hash32or64 = std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;
} // namespace nfx::hashing
