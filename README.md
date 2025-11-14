# nfx-hashing

<!-- Project Info -->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://github.com/nfx-libs/nfx-hashing/blob/main/LICENSE.txt) [![GitHub release (latest by date)](https://img.shields.io/github/v/release/nfx-libs/nfx-hashing?style=flat-square)](https://github.com/nfx-libs/nfx-hashing/releases) [![GitHub tag (latest by date)](https://img.shields.io/github/tag/nfx-libs/nfx-hashing?style=flat-square)](https://github.com/nfx-libs/nfx-hashing/tags)<br/>

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square) ![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg?style=flat-square) ![Cross Platform](https://img.shields.io/badge/Platform-Linux_Windows-lightgrey?style=flat-square)

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-hashing/build-linux-gcc.yml?branch=main&label=Linux%20GCC&style=flat-square)](https://github.com/nfx-libs/nfx-hashing/actions/workflows/build-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-hashing/build-linux-clang.yml?branch=main&label=Linux%20Clang&style=flat-square)](https://github.com/nfx-libs/nfx-hashing/actions/workflows/build-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-hashing/build-windows-mingw.yml?branch=main&label=Windows%20MinGW&style=flat-square)](https://github.com/nfx-libs/nfx-hashing/actions/workflows/build-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-hashing/build-windows-msvc.yml?branch=main&label=Windows%20MSVC&style=flat-square)](https://github.com/nfx-libs/nfx-hashing/actions/workflows/build-windows-msvc.yml) [![CodeQL](https://img.shields.io/github/actions/workflow/status/nfx-libs/nfx-hashing/codeql.yml?branch=main&label=CodeQL&style=flat-square)](https://github.com/nfx-libs/nfx-hashing/actions/workflows/codeql.yml)

> A modern C++20 header-only library providing high-performance hashing algorithms

## Overview

nfx-hashing is a modern C++20 header-only library that provides a collection of efficient hashing algorithms and utilities optimized for use in hash-based containers. The library includes hardware-accelerated implementations where available, with automatic fallback to software implementations.

## Key Features

### ⚡ High-Performance Hashing Algorithms

- **Hardware Acceleration**: SSE4.2 CRC32-C intrinsics for faster hashing
- **Software Fallback**: CRC32-C software implementation for systems without SSE4.2
- **Multiple Algorithms**: CRC32-C (Castagnoli), FNV-1a, Larson, integer hashing (32/64-bit)
- **Hash Combining**: Boost-style + MurmurHash3 finalizer for composite keys
- **Seed Mixing**: Utilities for hash table probing and collision resolution
- **Constexpr Support**: Compile-time hash computation where possible
- **Hardware Acceleration**: Automatic use of SSE4.2 CRC32-C instructions when available, with software fallback

### 📊 Real-World Applications

- **Hash Tables**: Foundation for custom hash table implementations
- **Data Structures**: High-performance key hashing for maps and sets
- **Checksums**: Fast data integrity verification
- **Fingerprinting**: Quick data fingerprint generation

### 🎯 Algorithm Selection

- **Manual Selection**: Choose between CRC32-C (default) or FNV-1a algorithms
- **Type-Safe**: Template-based integer hashing with compile-time type checking
- **Optimized**: Knuth's multiplicative hashing (32-bit), Wang's avalanche (64-bit)
- **Avalanche**: Excellent bit distribution for uniform hash values

### ⚡ Performance Optimized

- Header-only library with zero runtime dependencies
- Hardware-accelerated hashing with SSE4.2 intrinsics when available
- Software fallback for older CPUs
- Efficient hash combining with excellent avalanche properties
- Zero-cost abstractions with constexpr support
- Compiler-optimized inline implementations
- Runtime CPU feature detection with cached results

### 🌍 Cross-Platform Support

- **Platforms**: Linux, Windows
- **Architecture**: x86-64 (x86 SIMD features: SSE4.2, AVX, AVX2)
- **Compilers**: GCC 14+, Clang 19+, MSVC 2022+
- Thread-safe operations
- Consistent behavior across platforms
- CI/CD testing on multiple compilers

## Quick Start

### Requirements

- C++20 compatible compiler:
  - **GCC 14+** (14.2.0 tested)
  - **Clang 18+** (19.1.7 tested)
  - **MSVC 2022+** (19.44+ tested)
- CMake 3.20 or higher

### CMake Integration

```cmake
# Development options
option(NFX_HASHING_BUILD_TESTS          "Build tests"                        OFF )
option(NFX_HASHING_BUILD_SAMPLES        "Build samples"                      OFF )
option(NFX_HASHING_BUILD_BENCHMARKS     "Build benchmarks"                   OFF )
option(NFX_HASHING_BUILD_DOCUMENTATION  "Build Doxygen documentation"        OFF )

# Installation
option(NFX_HASHING_INSTALL_PROJECT      "Install project"                    OFF )

# Packaging
option(NFX_HASHING_PACKAGE_SOURCE       "Enable source package generation"   OFF )
option(NFX_HASHING_PACKAGE_ARCHIVE      "Enable TGZ/ZIP package generation"  OFF )
option(NFX_HASHING_PACKAGE_DEB          "Enable DEB package generation"      OFF )
option(NFX_HASHING_PACKAGE_RPM          "Enable RPM package generation"      OFF )
option(NFX_HASHING_PACKAGE_WIX          "Enable WiX MSI installer"           OFF )
```

### Using in Your Project

#### Option 1: Using FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
  nfx-hashing
  GIT_REPOSITORY https://github.com/nfx-libs/nfx-hashing.git
  GIT_TAG        main  # or use specific version tag like "0.1.0"
)
FetchContent_MakeAvailable(nfx-hashing)

# Link with header-only interface library
target_link_libraries(your_target PRIVATE nfx-hashing::nfx-hashing)
```

#### Option 2: As a Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/nfx-libs/nfx-hashing.git third-party/nfx-hashing
```

```cmake
# In your CMakeLists.txt
add_subdirectory(third-party/nfx-hashing)
target_link_libraries(your_target PRIVATE nfx-hashing::nfx-hashing)
```

#### Option 3: Using find_package (After Installation)

```cmake
find_package(nfx-hashing REQUIRED)
target_link_libraries(your_target PRIVATE nfx-hashing::nfx-hashing)
```

### Building

> ⚠️ **Important**: The library uses runtime CPU detection for SSE4.2 support, but **compiler flags are still required** to emit SIMD instructions. Without proper flags, the library **silently falls back** to software implementation even if your CPU supports SSE4.2.

**Compiler Flags for SIMD:**

- **GCC/Clang**: `-march=native` (auto-detect) or specific flags like `-msse4.2`, `-mavx`, `-mavx2`
- **MSVC**: `/arch:AVX` or `/arch:AVX2`

**CMake Example:**

```cmake
target_compile_options(your_target PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
)
```

**Build Commands:**

```bash
# Clone the repository
git clone https://github.com/nfx-libs/nfx-hashing.git
cd nfx-hashing

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release --parallel

# Run tests (optional)
ctest -C Release --output-on-failure

# Run benchmarks (optional)
./bin/benchmarks/BM_Hashing
```

### Documentation

nfx-hashing includes API documentation generated with Doxygen.

#### 📚 Online Documentation

The complete API documentation is available online at:
**https://nfx-libs.github.io/nfx-hashing**

#### Building Documentation Locally

```bash
# Configure with documentation enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DNFX_HASHING_BUILD_DOCUMENTATION=ON

# Build the documentation
cmake --build . --target documentation
```

#### Requirements

- **Doxygen** - Documentation generation tool
- **Graphviz Dot** (optional) - For generating class diagrams

#### Accessing Local Documentation

After building, open `./build/doc/html/index.html` in your web browser.

## Usage Example

```cpp
#include <nfx/Hashing.h>

using namespace nfx::hashing;

// Hash a string with the unified hash<T> API
// Uses CRC32-C (hardware-accelerated when available) with default FNV seed
uint32_t hash_value = hash<std::string>("example_key");

// Hash with 64-bit output
uint64_t hash64 = hash<std::string, uint64_t>("example_key");

// Hash with custom seed (compile-time)
uint32_t custom_hash = hash<std::string, uint32_t, 0xDEADBEEF>("example_key");

// Hash integers
uint32_t int_hash = hash<int>(42);

// Hash complex types (pairs, tuples, vectors, etc.)
std::pair<int, std::string> p = {42, "answer"};
uint32_t pair_hash = hash<std::pair<int, std::string>>(p);

// Low-level: Manual algorithm use (for advanced users)
uint32_t fnvHash = constants::FNV_OFFSET_BASIS_32;
for (char ch : std::string_view("example_key"))
{
    fnvHash = fnv1a(fnvHash, static_cast<uint8_t>(ch));
}
```

## Installation & Packaging

nfx-hashing provides packaging options for distribution.

### Package Generation

```bash
# Configure with packaging options
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNFX_HASHING_PACKAGE_ARCHIVE=ON \
         -DNFX_HASHING_PACKAGE_DEB=ON \
         -DNFX_HASHING_PACKAGE_RPM=ON

# Generate binary packages
cmake --build . --target package
# or
cd build && cpack

# Generate source packages
cd build && cpack --config CPackSourceConfig.cmake
```

### Supported Package Formats

| Format      | Platform       | Description                        | Requirements |
| ----------- | -------------- | ---------------------------------- | ------------ |
| **TGZ/ZIP** | Cross-platform | Compressed archive packages        | None         |
| **DEB**     | Debian/Ubuntu  | Native Debian packages             | `dpkg-dev`   |
| **RPM**     | RedHat/SUSE    | Native RPM packages                | `rpm-build`  |
| **WiX**     | Windows        | Professional MSI installer         | `WiX 3.11+`  |
| **Source**  | Cross-platform | Source code distribution (TGZ+ZIP) | None         |

### Installation

```bash
# Linux (DEB-based systems)
sudo dpkg -i nfx-hashing_*_amd64.deb

# Linux (RPM-based systems)
sudo rpm -ivh nfx-hashing-*-Linux.rpm

# Windows
# Run the .exe installer with administrator privileges
nfx-hashing-*-win64.exe

# Manual installation (extract archive)
tar -xzf nfx-hashing-*-Linux.tar.gz -C /usr/local/
```

## Project Structure

```
nfx-hashing/
├── benchmark/             # Benchmarks with Google Benchmark
├── cmake/                 # CMake modules and configuration
├── include/nfx/           # Public headers: hashing algorithms
├── samples/               # Example usage and demonstrations
└── test/                  # Unit tests with GoogleTest
```

## Performance

nfx-hashing is optimized for high performance with:

- **Hardware-accelerated hashing** - SSE4.2 CRC32-C intrinsics
- **Software fallback** - Pure C++ CRC32-C implementation when SSE4.2 unavailable
- **Constexpr support** - Compile-time hash computation where possible
- **Zero-cost abstractions** - No runtime overhead for type safety
- **Optimized avalanche** - Excellent bit distribution for uniform hash values
- **Runtime detection with static caching** - CPU feature detection at startup, cached for zero overhead thereafter
- **Cache-friendly** - Optimized memory access patterns for large datasets

For detailed performance metrics and benchmarks, see the [benchmark documentation](benchmark/README.md).

## Roadmap

See [TODO.md](TODO.md) for upcoming features and project roadmap.

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Dependencies

- **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
- **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

---

_Updated on November 12, 2025_
