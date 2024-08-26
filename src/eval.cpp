#include "eval.h"
#include "position.h"

void init_eval() {
  for (int r = 0; r < 6; r++)
    for (int y = 0; y < 8; y++)
      for (int x = 0; x < 8; x++) {
        const int iw = (7 - y) * 8 + x;
        const int ib = y * 8 + x;
        const int shift = r * 64 + ib;
        pst_pos_mg[0][r][iw] = pst_mg[shift];
        pst_pos_eg[0][r][iw] = pst_eg[shift];
        pst_pos_mg[1][r][ib] = pst_mg[shift];
        pst_pos_eg[1][r][ib] = pst_eg[shift];
      }
  for (int p = 0; p < 29; p++)
    for (int c = 0; c < 2; c++)
      for (int r = 0; r < 6; r++)
        for (int s = 0; s < 64; s++) {
          const int32_t mg = pst_pos_mg[c][r][s];
          const int32_t eg = pst_pos_eg[c][r][s];
          pst_pos[p][c][r][s] = (mg * p + eg * (28 - p)) / 28;
        }
}

int32_t eval(const move& m, const bool q) {
  const square fr = m.from();
  const square to = m.to();
  const int flags = m.flags();
  const piece pfr = pos.board[fr];
  const int fr_col = color_of(pfr);
  int fr_rank = type_of(pfr);
  const int fr_sc = pst_pos[pos.phase][fr_col][fr_rank][fr];
  int32_t score = -fr_sc;
  if (flags & capture) {
    const piece pto = pos.board[to];
    const int to_rank = type_of(pto);
    score += pst_pos[pos.phase][fr_col ^ 1][to_rank][to];
    if (q && pos.attackers(to))
      score -= pst_pos[pos.phase][fr_col][fr_rank][to];
  }
  if (flags & promo)
    fr_rank = 1 + flags & 3;
  return score + pst_pos[pos.phase][fr_col][fr_rank][to];
}

int32_t eval() {
  int32_t score_w = 0;
  int32_t score_b = 0;
  int piece_w[6] = {};
  int piece_b[6] = {};
  for (int s = 0; s < 64; s++) {
    const piece p = pos.board[s];
    if (p == no_piece)
      continue;
    const int col = color_of(p);
    const int rank = type_of(p);
    const int32_t val = pst_pos[pos.phase][col][rank][s];
    if (col == 0) {
      score_w += val;
      piece_w[rank]++;
    }
    else {
      score_b += val;
      piece_b[rank]++;
    }
  }
  return pos.us() == white ? score_w - score_b : score_b - score_w;
}
