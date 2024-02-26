#include "eval.h"
#include "search.h"
#include "movepick.h"

void movepick::fill(const bool q)
{
  for (int n = 0; n < count; n++)
  {
    const move m = list[n];
    scores[n].mv = m;
    scores[n].score = static_cast<short>(eval(m, q));
  }
}

picker movepick::pick(const int index)
{
  int bst_i = index;
  int16_t bst_s = scores[bst_i].score;
  for (int n = index + 1; n < count; n++)
  {
    if (const int16_t cur_s = scores[n].score; bst_s < cur_s)
    {
      bst_s = cur_s;
      bst_i = n;
    }
  }
  if (index != bst_i)
  {
    const picker e = scores[index];
    scores[index] = scores[bst_i];
    scores[bst_i] = e;
  }
  return scores[index];
}

void movepick::set_best(const int index)
{
  const picker e = scores[index];
  for (int i = index; i > 0; i--)
    scores[i] = scores[i - 1];
  scores[0] = e;
}

void movepick::sort()
{
  for (int n = 0; n < count - 1; n++)
    pick(n);
}

int movepick::get_index(const move& m) const
{
  for (int n = 0; n < count; n++)
    if (list[n] == m)
      return n;
  return -1;
}

bool movepick::set_move(const move& m)
{
  const int index = get_index(m);
  sd.move_set++;
  if (index < 0)
    return false;
  sd.move_ok++;
  for (int n = index; n > 0; n--)
    list[n] = list[n - 1];
  list[0] = m;
  return true;
}
