#pragma once
#include "main.h"

class move {
public:
  uint16_t mv;

  move() : mv(0) {}
  move(const uint16_t m) { mv = m; }

  move(const square from, const square to) : mv(0) {
    mv = static_cast<uint16_t>(from << 6 | to);
  }

  move(const square from, const square to, const move_flag flags) : mv(0) {
    mv = static_cast<uint16_t>(flags << 12 | from << 6 | to);
  }

  explicit move(const std::string& mv) {
    const square fr = create_square(static_cast<file>(mv[0] - 'a'), static_cast<rank>(mv[1] - '1'));
    const square to = create_square(static_cast<file>(mv[2] - 'a'), static_cast<rank>(mv[3] - '1'));
    move_flag mf = quiet;
    if (mv.length() > 4) {
      if (mv[5] == 'q')
        mf = promo_cap_queen;
      else if (mv[5] == 'r')
        mf = promo_cap_rook;
      else if (mv[5] == 'b')
        mf = promo_cap_bishop;
      else if (mv[5] == 'n')
        mf = promo_cap_knight;
    }
    this->mv = static_cast<uint16_t>(mf << 12 | fr << 6 | to);
  }

  [[nodiscard]] square to() const { return static_cast<square>(mv & 0x3f); }
  [[nodiscard]] square from() const { return static_cast<square>(mv >> 6 & 0x3f); }
  [[nodiscard]] int to_from() const { return mv & 0xffff; }
  [[nodiscard]] move_flag flags() const { return static_cast<move_flag>(mv >> 12 & 0xf); }

  [[nodiscard]] bool is_capture() const {
    return mv >> 12 & capture;
  }

  [[nodiscard]] std::string to_uci() const {
    std::string uci = sq_str[from()] + sq_str[to()];
    if (flags() & promo)
      return uci + move_typestr_uci[flags() & 7];
    return uci;
  }

  bool operator==(const move a) const { return to_from() == a.to_from(); }
  bool operator!=(const move a) const { return to_from() != a.to_from(); }
};

template <move_flag F = quiet>
move* make(const square from, uint64_t to, move* list) {
  while (to) *list++ = move(from, pop_lsb(&to), F);
  return list;
}
