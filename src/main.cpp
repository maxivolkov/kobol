#include "bitboard.h"
#include "eval.h"
#include "input.h"
#include "uci.h"
#include "util.h"
#include "zobrist.h"

int main() {
  engine_info();
  init_eval();
  init_tables();
  init_zobrist();
  init_input();
  uci_loop();
}
