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

| Operation             | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| FNV1a (single byte)   | **0.838 ns** | 1.06 ns     | 0.849 ns          | 1.35 ns               | 1.49 ns                | 1.25 ns      |
| CRC32C (single byte)  | **0.629 ns** | 0.645 ns    | 0.632 ns          | 0.837 ns              | 0.907 ns               | 1.09 ns      |
| CRC32C SW (single)    | **5.12 ns**  | 5.77 ns     | 5.21 ns           | 7.61 ns               | 8.44 ns                | 14.4 ns      |
| Larson (single byte)  | **1.05 ns**  | 1.06 ns     | **1.05 ns**       | 1.55 ns               | 1.72 ns                | 2.13 ns      |
| Seed mix (32-bit)     | **2.31 ns**  | 2.33 ns     | 2.44 ns           | 3.69 ns               | 3.85 ns                | 4.59 ns      |
| FNV1a64 (single byte) | 0.839 ns     | 1.06 ns     | **0.837 ns**      | 1.40 ns               | 1.26 ns                | 1.41 ns      |
| Larson64 (single)     | **1.05 ns**  | 1.07 ns     | 1.06 ns           | 1.34 ns               | 1.22 ns                | 1.37 ns      |
| Seed mix (64-bit)     | **3.56 ns**  | 3.60 ns     | 3.58 ns           | 4.45 ns               | 3.99 ns                | 5.77 ns      |

### String hashing (32-bit)

| Operation                | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------ | --------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| Hash short (5 chars)     | 208 ns    | **195 ns**  | 221 ns            | 344 ns                | 348 ns                 | 433 ns       |
| Hash medium (30 chars)   | 453 ns    | **378 ns**  | 474 ns            | 593 ns                | 632 ns                 | 993 ns       |
| Hash long (1000 chars)   | 4837 ns   | **3638 ns** | 5052 ns           | 5781 ns               | 6042 ns                | 10463 ns     |
| ByteByByte FNV1a short   | 194 ns    | **192 ns**  | 207 ns            | 490 ns                | 510 ns                 | 423 ns       |
| ByteByByte FNV1a medium  | 591 ns    | **526 ns**  | 614 ns            | 830 ns                | 872 ns                 | 1256 ns      |
| ByteByByte FNV1a long    | 7334 ns   | **6959 ns** | 7533 ns           | 10882 ns              | 11347 ns               | 15276 ns     |
| ByteByByte CRC32C short  | 188 ns    | **184 ns**  | 220 ns            | 361 ns                | 361 ns                 | 446 ns       |
| ByteByByte CRC32C medium | 473 ns    | **380 ns**  | 465 ns            | 609 ns                | 625 ns                 | 1011 ns      |
| ByteByByte CRC32C long   | 4827 ns   | **3625 ns** | 4948 ns           | 5859 ns               | 5885 ns                | 10114 ns     |
| Hash64 short (5 chars)   | 259 ns    | **258 ns**  | 259 ns            | 289 ns                | 265 ns                 | 363 ns       |
| Hash64 medium (30 chars) | 775 ns    | **760 ns**  | 796 ns            | 889 ns                | 802 ns                 | 1246 ns      |
| Hash64 long (1000 chars) | 7133 ns   | **6405 ns** | 7022 ns           | 7440 ns               | 6743 ns                | 10742 ns     |
| Single string (reuse)    | 8.40 ns   | **7.96 ns** | 8.37 ns           | 11.9 ns               | 10.9 ns                | 18.7 ns      |

### Integer hashing

| Operation             | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------- | ---------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| Hash uint32 (1000×)   | **452 ns** | 771 ns      | 469 ns            | 1107 ns               | 1082 ns                | 1496 ns      |
| Hash uint64 (1000×)   | 706 ns     | **227 ns**  | 730 ns            | 332 ns                | 327 ns                 | 1911 ns      |
| Hash int32 (1000×)    | **453 ns** | 772 ns      | 469 ns            | 1081 ns               | 1042 ns                | 1442 ns      |
| Hash64 uint64 (1000×) | 842 ns     | 838 ns      | **825 ns**        | 872 ns                | 849 ns                 | 1172 ns      |

### Algorithm comparison

| Operation                       | Linux GCC   | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------- | ----------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| CRC32C HW short (5 chars)       | **1.07 ns** | 1.18 ns     | 1.11 ns           | 1.62 ns               | 1.79 ns                | 2.41 ns      |
| CRC32C SW short (5 chars)       | **18.3 ns** | 19.1 ns     | 18.4 ns           | 27.5 ns               | 29.8 ns                | 74.4 ns      |
| CRC32C HW medium (30 chars)     | 15.6 ns     | 11.5 ns     | **10.8 ns**       | 16.8 ns               | 19.1 ns                | 24.6 ns      |
| CRC32C SW medium (30 chars)     | **205 ns**  | 229 ns      | 212 ns            | 343 ns                | 361 ns                 | 688 ns       |
| std::hash short (5 chars)       | 350 ns      | 370 ns      | **273 ns**        | 253 ns                | 314 ns                 | 408 ns       |
| std::hash medium (30 chars)     | 396 ns      | 381 ns      | **374 ns**        | 849 ns                | 830 ns                 | 1180 ns      |
| std::hash long (1000 chars)     | **1088 ns** | 1095 ns     | 1144 ns           | 11149 ns              | 10905 ns               | 15230 ns     |
| std::hash uint32 (1000×)        | **101 ns**  | 103 ns      | 103 ns            | 2252 ns               | 2083 ns                | 1688 ns      |
| std::hash uint64 (1000×)        | 287 ns      | **159 ns**  | 294 ns            | 2849 ns               | 2679 ns                | 3248 ns      |
| std::hash single string (reuse) | 4.41 ns     | **4.24 ns** | 4.55 ns           | 17.4 ns               | 16.3 ns                | 25.7 ns      |
| Combine64 FNV                   | **1.05 ns** | 1.07 ns     | 1.08 ns           | 1.15 ns               | 1.15 ns                | 1.43 ns      |
| Combine64 BoostMurmur           | 3.67 ns     | **3.52 ns** | 3.77 ns           | 3.68 ns               | 3.68 ns                | 4.71 ns      |

### Memory access patterns

| Operation                   | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------- | --------- | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| Cache test (100 strings)    | 20.8 ns   | 21.4 ns     | **20.4 ns**       | 33.7 ns               | 27.6 ns                | 38.5 ns      |
| Sequential (100 strings)    | 31.2 ns   | **21.9 ns** | 23.7 ns           | 28.7 ns               | 26.4 ns                | 45.2 ns      |
| Random access (100 strings) | 94.2 ns   | **42.8 ns** | 100 ns            | 57.7 ns               | 52.1 ns                | 140 ns       |

---

_Benchmarks executed on November 15, 2025_
