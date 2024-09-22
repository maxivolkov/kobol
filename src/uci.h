#pragma once

#include "search.h"

struct s_options {
  int aspiration = 38;
  int contempt = 0;
  int hash = 64;
  int overhead = 10;
  bool ponder = false;
};

inline std::string engine = "kobol";
inline std::string version = "1.1";
inline std::string author = "maksim";

inline s_options options;
void uci_command(std::string str);
void uci_loop();
