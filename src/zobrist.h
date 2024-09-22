#pragma once
#include <random>
#include "main.h"

class prng {
public:
  uint64_t s;

  static uint64_t rand64() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    return dis(gen);
  }

  explicit prng(const uint64_t seed) : s(seed) {}

  template <typename T>
  static T rand() { return T(rand64()); }

  template <typename T>
  static T sparse_rand() {
    return T(rand64() & rand64() & rand64());
  }
};

void init_zobrist();
inline uint64_t hash_color;
inline uint64_t zobrist_table[npieces][nsquares];
