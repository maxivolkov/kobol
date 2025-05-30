#pragma once
#include <random>
#include "main.h"

inline uint64_t rand_u64() {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint64_t> dis;
  return dis(gen);
}

void init_zobrist();
inline uint64_t hash_color;
inline uint64_t zobrist_table[npieces][nsquares];
