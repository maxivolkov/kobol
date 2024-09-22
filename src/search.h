#pragma once

#include <chrono>

#include "move.h"

constexpr auto max_depth = 100;
constexpr int32_t max_checkmate = 0x7ff0;
constexpr int32_t near_checkmate = 0x7000;

struct search_driver {
  int move_set = 0;
  int move_ok = 0;
  int32_t depth = 0;
  int32_t bscore = 0;
  uint64_t nodes = 0;
  uint64_t nodesq = 0;
  move bmove = 0;
  move pmove = 0;
  std::chrono::steady_clock::time_point time_start{};

  void restart() {
    move_set = 0;
    move_ok = 0;
    depth = 0;
    bscore = 0;
    nodes = 0;
    nodesq = 0;
    bmove = 0;
    pmove = 0;
    time_start = std::chrono::steady_clock::now();
  }

  [[nodiscard]] uint64_t elapsed() const {
    const std::chrono::steady_clock::time_point time_now = std::chrono::steady_clock::now();
    const std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>
	  (time_now - time_start);
    return duration.count();
  }
};

inline search_driver sd;

void search_iterate();
