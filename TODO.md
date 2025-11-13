# nfx-hashing TODO

Project roadmap and task tracking for the nfx-hashing library.

### Todo

- [ ] Add configurable hash algorithm selection for string hashing:
  - [ ] Template parameter or policy class to choose CRC32-C vs FNV-1a vs Larson
  - [ ] Current Seed parameter only controls seed value, not algorithm choice
  - [ ] Allow users to opt for FNV-1a (simpler/portable) vs CRC32-C (faster with SSE4.2)
- [ ] Performance optimizations:
  - [ ] SIMD-accelerated bulk hashing for large buffers (process 4-8 bytes at once)
  - [ ] Constexpr CRC32-C with lookup table for compile-time string hashing

### In Progress

- NIL

### Done ✓

- NIL
