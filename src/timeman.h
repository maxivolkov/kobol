#pragma once

enum time_factor
{
  tf_time = 1,
  tf_inc = 2,
  tf_movestogo = 4,
  tf_depth = 8,
  tf_nodes = 16,
  tf_mate = 32,
  tf_movetime = 64,
  tf_infinite = 128
};

struct search_param
{
  bool game_over;
  bool ponder;
  bool post;
  int time[2];
  int inc[2];
  int movestogo;
  int depth;
  int nodes;
  int movetime;
  int flags;

  void reset()
  {
    game_over = false;
    ponder = false;
    post = true;
    movestogo = 0;
    depth = 0;
    nodes = 0;
    movetime = 0;
    flags = 0;
  }
};

inline search_param sp;
bool check_time();
