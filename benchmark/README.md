# Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **Computer Model**       | Lenovo ThinkPad P15v Gen 3                                    |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×6 P-cores) + 32 KiB (×8 E-cores)                     |
| **L1 Instruction Cache** | 32 KiB (×6 P-cores) + 64 KiB (×2 E-core clusters)             |
| **L2 Unified Cache**     | 1.25 MiB (×6 P-cores) + 2 MiB (×2 E-core clusters)            |
| **L3 Unified Cache**     | 24 MiB (×1 shared)                                            |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

| Platform    | OS         | Benchmark Framework     | C++ Compiler              | nfx-hashing Version |
| ----------- | ---------- | ----------------------- | ------------------------- | ------------------- |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | GCC 14.2.0-x64            | v0.1.0              |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64          | v0.1.0              |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64      | v0.1.0              |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-GNU-CLI 19.1.5-x64  | v0.1.0              |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-MSVC-CLI 19.1.5-x64 | v0.1.0              |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64    | v0.1.0              |

---

# Performance Results

## String Hashing Operations

### Low-level hash building block

| Operation             | Linux GCC    | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------- | ------------ | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| FNV1a (single byte)   | **0.105 ns** | 0.106 ns     | 0.151 ns          | 0.238 ns              | 0.243 ns               | 0.698 ns     |
| CRC32C (single byte)  | **0.374 ns** | 0.376 ns     | 0.465 ns          | 0.665 ns              | 0.665 ns               | 1.05 ns      |
| CRC32C SW (single)    | **0.105 ns** | 0.107 ns     | 0.151 ns          | 2.31 ns               | 2.33 ns                | 7.44 ns      |
| Larson (single byte)  | **0.106 ns** | 0.107 ns     | 0.141 ns          | 0.298 ns              | 0.289 ns               | 0.698 ns     |
| Seed mix (32-bit)     | 0.108 ns     | **0.106 ns** | 0.141 ns          | 0.727 ns              | 0.718 ns               | 1.06 ns      |
| FNV1a64 (single byte) | **0.106 ns** | **0.106 ns** | 0.167 ns          | 0.283 ns              | 0.651 ns               | 0.773 ns     |
| Larson64 (single)     | 0.107 ns     | **0.106 ns** | 0.171 ns          | 0.340 ns              | 0.651 ns               | 0.785 ns     |
| Seed mix (64-bit)     | **0.106 ns** | **0.106 ns** | 0.171 ns          | 0.921 ns              | 1.92 ns                | 1.33 ns      |

### String hashing (32-bit)

| Operation                | Linux GCC   | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------ | ----------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| Hash short (5 chars)     | 350 ns      | **338 ns**  | 384 ns            | 417 ns                | 427 ns                 | 567 ns       |
| Hash medium (30 chars)   | 1006 ns     | **958 ns**  | 1074 ns           | 1107 ns               | 1090 ns                | 1509 ns      |
| Hash long (1000 chars)   | 7907 ns     | **7492 ns** | 8998 ns           | 8371 ns               | 8545 ns                | 8022 ns      |
| ByteByByte FNV1a short   | 207 ns      | **200 ns**  | 251 ns            | 279 ns                | 244 ns                 | 242 ns       |
| ByteByByte FNV1a medium  | 635 ns      | **625 ns**  | 767 ns            | 632 ns                | 609 ns                 | 952 ns       |
| ByteByByte FNV1a long    | 7585 ns     | **7444 ns** | 10045 ns          | 8487 ns               | 7906 ns                | 7615 ns      |
| ByteByByte CRC32C short  | 351 ns      | **332 ns**  | 366 ns            | 417 ns                | 653 ns                 | 450 ns       |
| ByteByByte CRC32C medium | 1000 ns     | **998 ns**  | 1569 ns           | 1123 ns               | 1980 ns                | 1535 ns      |
| ByteByByte CRC32C long   | 7390 ns     | **7374 ns** | 9208 ns           | 8859 ns               | 13939 ns               | 8487 ns      |
| Hash64 short (5 chars)   | 631 ns      | **613 ns**  | 1311 ns           | 928 ns                | 1688 ns                | 732 ns       |
| Hash64 medium (30 chars) | 1964 ns     | **1959 ns** | 4098 ns           | 2783 ns               | 5052 ns                | 2930 ns      |
| Hash64 long (1000 chars) | 9124 ns     | **8997 ns** | 14125 ns          | 17648 ns              | 27274 ns               | 14881 ns     |
| Single string (reuse)    | **19.6 ns** | 19.9 ns     | 24.6 ns           | 37.5 ns               | 51.6 ns                | 32.6 ns      |

### Integer hashing

| Operation             | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------- | ---------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| Hash uint32 (1000×)   | 363 ns     | **357 ns**  | 465 ns            | 628 ns                | 901 ns                 | 665 ns       |
| Hash uint64 (1000×)   | 612 ns     | **593 ns**  | 820 ns            | 274 ns                | 326 ns                 | 698 ns       |
| Hash int32 (1000×)    | 362 ns     | **358 ns**  | 476 ns            | 646 ns                | 889 ns                 | 670 ns       |
| Hash64 uint64 (1000×) | **727 ns** | 734 ns      | 1099 ns           | 1358 ns               | 2100 ns                | 956 ns       |

### Algorithm comparison

| Operation                       | Linux GCC    | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------- | ------------ | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| CRC32C HW short (5 chars)       | 2.13 ns      | **2.11 ns**  | 2.61 ns           | 3.77 ns               | 3.75 ns                | 3.64 ns      |
| CRC32C SW short (5 chars)       | 18.8 ns      | **18.6 ns**  | 25.1 ns           | 21.5 ns               | 21.8 ns                | 43.0 ns      |
| CRC32C HW medium (30 chars)     | **22.4 ns**  | 23.6 ns      | 28.9 ns           | 29.1 ns               | 31.4 ns                | 34.5 ns      |
| CRC32C SW medium (30 chars)     | **206 ns**   | 208 ns       | 283 ns            | 259 ns                | 259 ns                 | 372 ns       |
| std::hash short (5 chars)       | 363 ns       | **360 ns**   | 328 ns            | 247 ns                | 436 ns                 | 353 ns       |
| std::hash medium (30 chars)     | 405 ns       | **391 ns**   | 445 ns            | 670 ns                | 1413 ns                | 628 ns       |
| std::hash long (1000 chars)     | 1147 ns      | **1116 ns**  | 1444 ns           | 8859 ns               | 19950 ns               | 7789 ns      |
| std::hash uint32 (1000×)        | 109 ns       | **106 ns**   | 141 ns            | 1883 ns               | 3023 ns                | 1025 ns      |
| std::hash uint64 (1000×)        | 300 ns       | **294 ns**   | 401 ns            | 2493 ns               | 4050 ns                | 1904 ns      |
| std::hash single string (reuse) | 4.49 ns      | **4.43 ns**  | 6.42 ns           | 15.7 ns               | 35.0 ns                | 16.2 ns      |
| Combine64 FNV                   | 0.107 ns     | **0.106 ns** | 0.173 ns          | 0.319 ns              | 0.505 ns               | 0.866 ns     |
| Combine64 BoostMurmur           | **0.106 ns** | **0.106 ns** | 0.176 ns          | 1.07 ns               | 1.87 ns                | 1.59 ns      |

### Memory access patterns

| Operation                   | Linux GCC | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | --------- | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| Cache test (100 strings)    | 3350 ns   | **3274 ns**  | 4143 ns           | 4202 ns               | 7580 ns                | 5677 ns      |
| Sequential (100 strings)    | 4333 ns   | **4262 ns**  | 5781 ns           | 6789 ns               | 11626 ns               | 6278 ns      |
| Random access (100 strings) | 12838 ns  | **12520 ns** | 20996 ns          | 15381 ns              | 37202 ns               | 18892 ns     |

---

_Benchmarks executed on November 13, 2025_
