#include "position.h"
#include "zobrist.h"

void init_zobrist() {
  hash_color = prng::rand64();
  for (auto& i : zobrist_table)
    for (unsigned long long& j : i)
      j = prng::rand64();
}
