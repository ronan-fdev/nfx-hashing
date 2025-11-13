# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- NIL

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL

## [0.1.0] - 2025-11-13 - Initial Release

### Added

- **`hash<T>`**: Template-based hash function for clean, STL-style hashing

- **`Hasher<HashType, Seed>`**: General-purpose STL-compatible hash functor

- **CRC32-C (Castagnoli)**: Hardware-accelerated hash using SSE4.2 intrinsics with software fallback

  - Polynomial: 0x1EDC6F41 (Castagnoli polynomial)
  - Runtime CPU detection: checks SSE4.2 support at first call, caches result
  - Hardware: uses `_mm_crc32_u8` (MSVC) or `__builtin_ia32_crc32qi` (GCC/Clang)
  - Software: pure C++ implementation matching SSE4.2 behavior
  - Single binary deployment: optimally runs on both old and new CPUs
  - Dual-stream 64-bit implementation: low 32 bits (normal bytes), high 32 bits (inverted bytes `byte ^ 0xFF`)

- **FNV-1a**: Fast non-cryptographic hash (32-bit and 64-bit variants)

  - XOR-multiply algorithm with prime constants
  - Fully `constexpr` for compile-time hashing

- **Larson Hash**: Simple multiplicative hash (`37 * hash + ch`)

  - Provided for benchmarking comparisons
  - Supports 32-bit and 64-bit variants

- **Integer Hashing**: Optimized multiplicative hashing for integral types

  - 32-bit: Knuth's multiplicative constant (`0x45d9f3b`)
  - 64-bit: Wang's avalanche mixing (`0xbf58476d1ce4e5b9`, `0x94d049bb133111eb`)

- **Hash Combining**: Two combination strategies

  - FNV-1a style: XOR then multiply by prime
  - Boost + MurmurHash3 finalizer: golden ratio mixing with avalanche finalization

- **Seed Mixing**: Hash table probing utilities

  - 32-bit: Thomas Wang's bit-mixing algorithm
  - 64-bit: MurmurHash3 avalanche mixing

- **Samples**

  - `Sample_Hash.cpp` - Unified hash<T> API demonstration
  - `Sample_Hasher.cpp` - STL container integration with `Hasher<>` functor
  - `Sample_LowLevelHashing.cpp` - Direct use of low-level hash primitives
  - `Sample_AdvancedHashing.cpp` - Advanced techniques (custom seeds, transparent lookup)

- **Build System**

  - CMake 3.20+ configuration with modular structure
  - Header-only interface library target
  - Cross-platform support (Linux GCC/Clang, Windows MinGW/Clang/MSVC)
  - Google Test integration for unit testing
  - Google Benchmark integration for performance benchmarking
  - Doxygen documentation generation with GitHub Pages deployment
  - CPack packaging for distribution (DEB, RPM, TGZ, ZIP, WiX MSI)
  - Installation support with CMake package config files

- **Documentation**

  - README with feature overview and usage examples
  - Detailed API documentation with Doxygen comments and RFC references
  - Sample applications
  - Build and installation instructions

- **Testing & Benchmarking**

  - Unit test suite
  - Performance benchmarks for all operations
  - Cross-compiler performance validation

- **CI/CD**: GitHub Actions workflows for automated testing, documentation and release deployment
