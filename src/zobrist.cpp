#include "position.h"
#include "zobrist.h"

void init_zobrist() {
  hash_color = rand_u64();
  for (auto& i : zobrist_table)
    for (unsigned long long& j : i)
      j = rand_u64();
}
