#include "eval.h"
#include "nnue.h"
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

/*
* Piece codes are
wking=1, wqueen=2, wrook=3, wbishop= 4, wknight= 5, wpawn= 6,
bking=7, bqueen=8, brook=9, bbishop=10, bknight=11, bpawn=12,

WHITE_PAWN=0, WHITE_KNIGHT=1, WHITE_BISHOP=2, WHITE_ROOK=3, WHITE_QUEEN=4, WHITE_KING=5,
BLACK_PAWN = 8, BLACK_KNIGHT=9, BLACK_BISHOP=10, BLACK_ROOK=11, BLACK_QUEEN=12, BLACK_KING=13
*/

int eval_nnue(const position& pos) {
  int pieces[33]{};
  int squares[33]{};
  int index = 2;
  for (uint8_t i = 0; i < 64; i++) {
    if (pos.at(static_cast<square>(i)) == 5) {
      pieces[0] = 1;
      squares[0] = i;
    }
    else if (pos.at(static_cast<square>(i)) == 13) {
      pieces[1] = 7;
      squares[1] = i;
    }
    else if (pos.at(static_cast<square>(i)) == 0) {
      pieces[index] = 6;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 1) {
      pieces[index] = 5;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 2) {
      pieces[index] = 4;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 3) {
      pieces[index] = 3;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 4) {
      pieces[index] = 2;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 8) {
      pieces[index] = 12;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 9) {
      pieces[index] = 11;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 10) {
      pieces[index] = 10;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 11) {
      pieces[index] = 9;
      squares[index] = i;
      index++;
    }
    else if (pos.at(static_cast<square>(i)) == 12) {
      pieces[index] = 8;
      squares[index] = i;
      index++;
    }
  }
  const int nnue_score = nnue_evaluate(pos.side_to_play, pieces, squares);
  return nnue_score;
}

int32_t eval(const position& pos) {
  const int nnue_score = eval_nnue(pos);
  return nnue_score;
}
