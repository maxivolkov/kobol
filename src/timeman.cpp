#include <cstdint>
#include "input.h"
#include "search.h"
#include "uci.h"
#include "timeman.h"

bool check_time()
{
  if (std::string input; get_input(input))
    uci_command(input);
  if (sp.game_over)
    return true;
  sp.game_over = sp.flags & tf_movetime && sd.elapsed() > static_cast<uint64_t>(sp.movetime);
  return sp.game_over;
}
