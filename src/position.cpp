#include <sstream>
#include "bitboard.h"
#include "move.h"
#include "main.h"
#include "position.h"

std::ostream& operator<<(std::ostream& os, const position& p) {
  for (int i = 56; i >= 0; i -= 8) {
    for (int j = 0; j < 8; j++) {
      const piece piece = p.board[i + j];
      os << piece_str[piece] << " ";
    }
    os << '\n';
  }
  os << '\n';
  os << "FEN " << p.get_fen() << '\n';
  os << "Hash 0x" << std::hex << p.hash_ << std::dec << '\n';
  os << (p.side_to_play == white ? "White to move" : "Black to move");
  os << std::endl;
  return os;
}

void position::move_list(const color color, move* list, int& count) {
  const move* last = generate_moves(color, list);
  count = static_cast<int>(last - list);
}

void position::move_list_q(const color color, move* list, int& count) {
  const move* last = generate_moves(color, list, false);
  count = static_cast<int>(last - list);
}

void position::move_list(move* list, int& count) {
  const move* last = generate_moves(color_us(), list);
  count = static_cast<int>(last - list);
}

void position::move_list_q(move* list, int& count) {
  const move* last = generate_moves(color_us(), list, false);
  count = static_cast<int>(last - list);
}

bool position::is_legal(const move& mv) {
  move list[218];
  generate_moves(side_to_play, list);
  for (move m : list) {
    if (m == mv)
      return true;
  }
  return false;
}

bool position::in_check(const color color) const {
  return attackers_from(~color, lsb(bitboard_of(color, king)), all_pieces());
}

bool position::in_check() const {
  return attackers_from(~side_to_play, lsb(bitboard_of(side_to_play, king)), all_pieces());
}

void position::make_null() {
  hash_ ^= hash_color;
  side_to_play = ~side_to_play;
  ++history_index;
  history[history_index] = undo_info(history[history_index - 1]);
  history[history_index].epsq = no_square;
}

void position::unmake_null() {
  hash_ ^= hash_color;
  side_to_play = ~side_to_play;
  --history_index;
}

void position::make_move(const move& m) {
  hash_ ^= hash_color;
  const color c = side_to_play;
  side_to_play = ~side_to_play;
  ++history_index;
  history[history_index] = undo_info(history[history_index - 1]);
  const square fr = m.from();
  const square to = m.to();
  const move_flags type = m.flags();
  history[history_index].entry |= square_bb[to] | square_bb[fr];
  move50 = type == quiet && type_of(board[fr]) != pawn ? ++move50 : 0;
  switch (type) {
  case quiet:
    move_piece_quiet(m.from(), m.to());
    break;
  case double_push:
    move_piece_quiet(m.from(), m.to());

    history[history_index].epsq = m.from() + relative_dir(c, north);
    break;
  case oo:
    if (c == white) {
      move_piece_quiet(e1, g1);
      move_piece_quiet(h1, f1);
    }
    else {
      move_piece_quiet(e8, g8);
      move_piece_quiet(h8, f8);
    }
    break;
  case ooo:
    if (c == white) {
      move_piece_quiet(e1, c1);
      move_piece_quiet(a1, d1);
    }
    else {
      move_piece_quiet(e8, c8);
      move_piece_quiet(a8, d8);
    }
    break;
  case en_passant:
    move_piece_quiet(m.from(), m.to());
    remove_piece(m.to() + relative_dir(c, south));
    break;
  case promo_knight:
    remove_piece(m.from());
    put_piece(make_piece(c, knight), m.to());
    break;
  case promo_bishop:
    remove_piece(m.from());
    put_piece(make_piece(c, bishop), m.to());
    break;
  case promo_rook:
    move50 = 0;
    remove_piece(m.from());
    put_piece(make_piece(c, rook), m.to());
    break;
  case promo_queen:
    remove_piece(m.from());
    put_piece(make_piece(c, queen), m.to());
    break;
  case promo_cap_knight:
    remove_piece(m.from());
    history[history_index].captured = board[m.to()];
    remove_piece(m.to());
    put_piece(make_piece(c, knight), m.to());
    break;
  case promo_cap_bishop:
    remove_piece(m.from());
    history[history_index].captured = board[m.to()];
    remove_piece(m.to());
    put_piece(make_piece(c, bishop), m.to());
    break;
  case promo_cap_rook:
    remove_piece(m.from());
    history[history_index].captured = board[m.to()];
    remove_piece(m.to());
    put_piece(make_piece(c, rook), m.to());
    break;
  case promo_cap_queen:
    remove_piece(m.from());
    history[history_index].captured = board[m.to()];
    remove_piece(m.to());
    put_piece(make_piece(c, queen), m.to());
    break;
  case capture:
    history[history_index].captured = board[m.to()];
    move_piece(m.from(), m.to());
    break;
  }
  is_in_check = attackers_from(c, lsb(bitboard_of(~c, king)), all_pieces());
  history[history_index].in_check = is_in_check;
  history[history_index].hash = hash_;
  history[history_index].move50 = move50;
}

void position::unmake_move(const move& m) {
  hash_ ^= hash_color;
  side_to_play = ~side_to_play;
  const color c = side_to_play;
  switch (const move_flags type = m.flags()) {
  case quiet:
    move_piece_quiet(m.to(), m.from());
    break;
  case double_push:
    move_piece_quiet(m.to(), m.from());
    break;
  case oo:
    if (c == white) {
      move_piece_quiet(g1, e1);
      move_piece_quiet(f1, h1);
    }
    else {
      move_piece_quiet(g8, e8);
      move_piece_quiet(f8, h8);
    }
    break;
  case ooo:
    if (c == white) {
      move_piece_quiet(c1, e1);
      move_piece_quiet(d1, a1);
    }
    else {
      move_piece_quiet(c8, e8);
      move_piece_quiet(d8, a8);
    }
    break;
  case en_passant:
    move_piece_quiet(m.to(), m.from());
    put_piece(make_piece(~c, pawn), m.to() + relative_dir(c, south));
    break;
  case promo_knight:
  case promo_bishop:
  case promo_rook:
  case promo_queen:
    remove_piece(m.to());
    put_piece(make_piece(c, pawn), m.from());
    break;
  case promo_cap_knight:
  case promo_cap_bishop:
  case promo_cap_rook:
  case promo_cap_queen:
    remove_piece(m.to());
    put_piece(make_piece(c, pawn), m.from());
    put_piece(history[history_index].captured, m.to());
    break;
  case capture:
    move_piece_quiet(m.to(), m.from());
    put_piece(history[history_index].captured, m.to());
    break;
  }
  --history_index;
  is_in_check = history[history_index].in_check;
  move50 = history[history_index].move50;
}

void position::set_fen(const std::string& fen) {
  clear();
  int sq = a8;
  for (const char ch : fen.substr(0, fen.find(' '))) {
    if (isdigit(ch))
      sq += (ch - '0') * east;
    else if (ch == '/')
      sq += 2 * south;
    else
      put_piece(static_cast<piece>(piece_str.find(ch)), static_cast<square>(sq++));
  }

  std::istringstream ss(fen.substr(fen.find(' ')));
  unsigned char token;

  ss >> token;
  side_to_play = token == 'w' ? white : black;

  history[history_index].entry = all_castling_mask;
  while (ss >> token && !isspace(token)) {
    switch (token) {
    case 'K':
      history[history_index].entry &= ~white_oo_mask;
      break;
    case 'Q':
      history[history_index].entry &= ~white_ooo_mask;
      break;
    case 'k':
      history[history_index].entry &= ~black_oo_mask;
      break;
    case 'q':
      history[history_index].entry &= ~black_ooo_mask;
      break;
    default:;
    }
  }
  is_in_check = in_check();
}

std::string position::get_fen() const {
  std::ostringstream fen;

  for (int i = 56; i >= 0; i -= 8) {
    int empty = 0;
    for (int j = 0; j < 8; j++) {
      if (const piece p = board[i + j]; p == no_piece) empty++;
      else {
        fen << (empty == 0 ? "" : std::to_string(empty))
          << piece_str[p];
        empty = 0;
      }
    }

    if (empty != 0) fen << empty;
    if (i > 0) fen << '/';
  }

  fen << (side_to_play == white ? " w " : " b ")
    << (history[history_index].entry & white_oo_mask ? "" : "K")
    << (history[history_index].entry & white_ooo_mask ? "" : "Q")
    << (history[history_index].entry & black_oo_mask ? "" : "k")
    << (history[history_index].entry & black_ooo_mask ? "" : "q")
    << (history[history_index].entry & all_castling_mask ? "- " : "")
    << (history[history_index].epsq == no_square ? " -" : sqstr[history[history_index].epsq]);

  return fen.str();
}

void position::move_piece(const square from, const square to) {
  hash_ ^= zobrist_table[board[from]][from] ^ zobrist_table[board[from]][to]
    ^ zobrist_table[board[to]][to];
  const uint64_t mask = square_bb[from] | square_bb[to];
  piece_bb[board[from]] ^= mask;
  piece_bb[board[to]] &= ~mask;
  board[to] = board[from];
  board[from] = no_piece;
}

void position::move_piece_quiet(const square from, const square to) {
  hash_ ^= zobrist_table[board[from]][from] ^ zobrist_table[board[from]][to];
  piece_bb[board[from]] ^= square_bb[from] | square_bb[to];
  board[to] = board[from];
  board[from] = no_piece;
}

move* position::generate_moves(color us, move* list, bool quietmove) {
  color them = ~us;

  const uint64_t us_bb = all_pieces(us);
  const uint64_t them_bb = all_pieces(them);
  const uint64_t all = us_bb | them_bb;

  const square our_king = lsb(bitboard_of(us, king));
  const square their_king = lsb(bitboard_of(them, king));

  const uint64_t our_diag_sliders = diagonal_sliders(us);
  const uint64_t their_diag_sliders = diagonal_sliders(them);
  const uint64_t our_orth_sliders = orthogonal_sliders(us);
  const uint64_t their_orth_sliders = orthogonal_sliders(them);

  uint64_t b1, b2, b3;

  uint64_t danger = 0;

  danger |= pawnattacks(them, bitboard_of(them, pawn)) | attacks<king>(their_king, all);

  b1 = bitboard_of(them, knight);
  while (b1) danger |= attacks<knight>(pop_lsb(&b1), all);

  b1 = their_diag_sliders;
  while (b1) danger |= attacks<bishop>(pop_lsb(&b1), all ^ square_bb[our_king]);

  b1 = their_orth_sliders;
  while (b1) danger |= attacks<rook>(pop_lsb(&b1), all ^ square_bb[our_king]);

  b1 = attacks<king>(our_king, all) & ~(us_bb | danger);
  if (quietmove)
    list = make<quiet>(our_king, b1 & ~them_bb, list);
  list = make<capture>(our_king, b1 & them_bb, list);

  uint64_t capture_mask;

  uint64_t quiet_mask;

  square s;

  checkers = (attacks<knight>(our_king, all) & bitboard_of(them, knight))
    | (pawnattacks(us, our_king) & bitboard_of(them, pawn));

  uint64_t candidates = (attacks<rook>(our_king, them_bb) & their_orth_sliders)
    | (attacks<bishop>(our_king, them_bb) & their_diag_sliders);

  pinned = 0;
  while (candidates) {
    s = pop_lsb(&candidates);
    b1 = squares_between_bb[our_king][s] & us_bb;

    if (b1 == 0) checkers ^= square_bb[s];
    else if ((b1 & b1 - 1) == 0) pinned ^= b1;
  }

  const uint64_t not_pinned = ~pinned;

  switch (sparse_popcnt(checkers)) {
  case 2:
    return list;
  case 1: {
    square checker_square = lsb(checkers);
    piece piece = board[checker_square];
    piece_type pt = type_of(piece);
    color pc = color_of(piece);
    if (pc == them && pt == pawn) {
      if (checkers == shift(relative_dir(us, south), square_bb[history[history_index].epsq])) {
        b1 = pawnattacks(them, history[history_index].epsq) & bitboard_of(us, pawn) & not_pinned;
        while (b1) *list++ = move(pop_lsb(&b1), history[history_index].epsq, en_passant);
      }
    }
    if (pc == them && pt == knight) {
      b1 = attackers_from(us, checker_square, all) & not_pinned;
      while (b1) *list++ = move(pop_lsb(&b1), checker_square, capture);

      return list;
    }
    capture_mask = checkers;

    quiet_mask = squares_between_bb[our_king][checker_square];

    break;
  }

  default:
    capture_mask = them_bb;

    quiet_mask = ~all;

    if (history[history_index].epsq != no_square) {
      b2 = pawnattacks(them, history[history_index].epsq) & bitboard_of(us, pawn);
      b1 = b2 & not_pinned;
      while (b1) {
        s = pop_lsb(&b1);

        if ((sliding_attacks(our_king, all ^ square_bb[s]
          ^ shift(relative_dir(us, south), square_bb[history[history_index].epsq]),
          mask_rank[rank_of(our_king)]) &
          their_orth_sliders) == 0)
          *list++ = move(s, history[history_index].epsq, en_passant);
      }

      b1 = b2 & pinned & line[history[history_index].epsq][our_king];
      if (b1) {
        *list++ = move(lsb(b1), history[history_index].epsq, en_passant);
      }
    }

    if (quietmove) {
      if (!((history[history_index].entry & oo_mask(us)) | ((all | danger) & oo_blockers_mask(us)))) {
        *list++ = us == white ? move(e1, g1, oo) : move(e8, g8, oo);
      }
      if (!((history[history_index].entry & ooo_mask(us)) |
        ((all | (danger & ~ignore_ooo_danger(us))) & ooo_blockers_mask(us))))
        *list++ = us == white ? move(e1, c1, ooo) : move(e8, c8, ooo);
    }
    b1 = ~(not_pinned | bitboard_of(us, knight));
    while (b1) {
      s = pop_lsb(&b1);

      b2 = attacks(type_of(board[s]), s, all) & line[our_king][s];
      if (quietmove)
        list = make<quiet>(s, b2 & quiet_mask, list);
      list = make<capture>(s, b2 & capture_mask, list);
    }

    b1 = ~not_pinned & bitboard_of(us, pawn);
    while (b1) {
      s = pop_lsb(&b1);

      if (rank_of(s) == relative_rank(us, rank_7)) {
        b2 = pawnattacks(us, s) & capture_mask & line[our_king][s];
        list = make<promo_caps>(s, b2, list);
      }
      else {
        b2 = pawnattacks(us, s) & them_bb & line[s][our_king];
        list = make<capture>(s, b2, list);
        if (quietmove) {
          b2 = shift(relative_dir(us, north), square_bb[s]) & ~all & line[our_king][s];
          b3 = shift(relative_dir(us, north), b2 &
            mask_rank[relative_rank(us, rank_3)]) & ~all & line[our_king][s];
          list = make<quiet>(s, b2, list);
          list = make<double_push>(s, b3, list);
        }
      }
    }

    break;
  }

  b1 = bitboard_of(us, knight) & not_pinned;
  while (b1) {
    s = pop_lsb(&b1);
    b2 = attacks<knight>(s, all);
    if (quietmove)
      list = make<quiet>(s, b2 & quiet_mask, list);
    list = make<capture>(s, b2 & capture_mask, list);
  }

  b1 = our_diag_sliders & not_pinned;
  while (b1) {
    s = pop_lsb(&b1);
    b2 = attacks<bishop>(s, all);
    if (quietmove)
      list = make<quiet>(s, b2 & quiet_mask, list);
    list = make<capture>(s, b2 & capture_mask, list);
  }

  b1 = our_orth_sliders & not_pinned;
  while (b1) {
    s = pop_lsb(&b1);
    b2 = attacks<rook>(s, all);
    if (quietmove)
      list = make<quiet>(s, b2 & quiet_mask, list);
    list = make<capture>(s, b2 & capture_mask, list);
  }

  b1 = bitboard_of(us, pawn) & not_pinned & ~mask_rank[relative_rank(us, rank_7)];

  if (quietmove) {
    b2 = shift(relative_dir(us, north), b1) & ~all;

    b3 = shift(relative_dir(us, north), b2 & mask_rank[relative_rank(us, rank_3)]) & quiet_mask;

    b2 &= quiet_mask;

    while (b2) {
      s = pop_lsb(&b2);
      *list++ = move(s - relative_dir(us, north), s, quiet);
    }

    while (b3) {
      s = pop_lsb(&b3);
      *list++ = move(s - relative_dir(us, northnorth), s, double_push);
    }
  }
  b2 = shift(relative_dir(us, northwest), b1) & capture_mask;
  b3 = shift(relative_dir(us, northeast), b1) & capture_mask;

  while (b2) {
    s = pop_lsb(&b2);
    *list++ = move(s - relative_dir(us, northwest), s, capture);
  }

  while (b3) {
    s = pop_lsb(&b3);
    *list++ = move(s - relative_dir(us, northeast), s, capture);
  }

  b1 = bitboard_of(us, pawn) & not_pinned & mask_rank[relative_rank(us, rank_7)];
  if (b1) {
    b2 = shift(relative_dir(us, north), b1) & quiet_mask;
    while (b2) {
      s = pop_lsb(&b2);
      *list++ = move(s - relative_dir(us, north), s, promo_knight);
      *list++ = move(s - relative_dir(us, north), s, promo_bishop);
      *list++ = move(s - relative_dir(us, north), s, promo_rook);
      *list++ = move(s - relative_dir(us, north), s, promo_queen);
    }

    b2 = shift(relative_dir(us, northwest), b1) & capture_mask;
    b3 = shift(relative_dir(us, northeast), b1) & capture_mask;

    while (b2) {
      s = pop_lsb(&b2);
      *list++ = move(s - relative_dir(us, northwest), s, promo_cap_knight);
      *list++ = move(s - relative_dir(us, northwest), s, promo_cap_bishop);
      *list++ = move(s - relative_dir(us, northwest), s, promo_cap_rook);
      *list++ = move(s - relative_dir(us, northwest), s, promo_cap_queen);
    }

    while (b3) {
      s = pop_lsb(&b3);
      *list++ = move(s - relative_dir(us, northeast), s, promo_cap_knight);
      *list++ = move(s - relative_dir(us, northeast), s, promo_cap_bishop);
      *list++ = move(s - relative_dir(us, northeast), s, promo_cap_rook);
      *list++ = move(s - relative_dir(us, northeast), s, promo_cap_queen);
    }
  }

  return list;
}
