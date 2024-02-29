#include "main.h"
#include "bitboard.h"

uint64_t reverse(uint64_t b) {
  b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
  b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
  b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
  b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

  return (b << 48) | (b & 0xffff0000) << 16 | ((b >> 16) & 0xffff0000) | b >> 48;
}

uint64_t sliding_attacks(const square sq, const uint64_t occ, const uint64_t mask) {
  return ((mask & occ) - square_bb[sq] * 2 ^
    reverse(reverse(mask & occ) - reverse(square_bb[sq]) * 2)) & mask;
}

uint64_t get_rook_attacks_for_init(const square sq, const uint64_t occ) {
  return sliding_attacks(sq, occ, mask_file[file_of(sq)]) |
    sliding_attacks(sq, occ, mask_rank[rank_of(sq)]);
}

void init_rook_attacks() {
  for (square sq = a1; sq <= h8; ++sq) {
    const uint64_t edges = ((mask_rank[a_file] | mask_rank[h_file]) & ~mask_rank[rank_of(sq)]) |
      ((mask_file[a_file] | mask_file[h_file]) & ~mask_file[file_of(sq)]);
    rook_attack_masks[sq] = (mask_rank[rank_of(sq)]
      ^ mask_file[file_of(sq)]) & ~edges;
    rook_attack_shifts[sq] = 64 - popcnt(rook_attack_masks[sq]);

    uint64_t subset = 0;
    do {
      uint64_t index = subset;
      index = index * rook_magics[sq];
      index = index >> rook_attack_shifts[sq];
      rook_attacks[sq][index] = get_rook_attacks_for_init(sq, subset);
      subset = subset - rook_attack_masks[sq] & rook_attack_masks[sq];
    }
    while (subset);
  }
}

uint64_t get_rook_attacks(const square sq, const uint64_t occ) {
  return rook_attacks[sq][(occ & rook_attack_masks[sq]) * rook_magics[sq]
    >> rook_attack_shifts[sq]];
}

uint64_t get_xray_rook_attacks(const square sq, const uint64_t occ, uint64_t blockers) {
  const uint64_t attacks = get_rook_attacks(sq, occ);
  blockers &= attacks;
  return attacks ^ get_rook_attacks(sq, occ ^ blockers);
}

uint64_t get_bishop_attacks_for_init(const square sq, const uint64_t occ) {
  return sliding_attacks(sq, occ, mask_diagonal[diagonal_of(sq)]) |
    sliding_attacks(sq, occ, mask_anti_diagonal[anti_diagonal_of(sq)]);
}

void init_bishop_attacks() {
  for (square sq = a1; sq <= h8; ++sq) {
    const uint64_t edges = ((mask_rank[a_file] | mask_rank[h_file]) & ~mask_rank[rank_of(sq)]) |
      ((mask_file[a_file] | mask_file[h_file]) & ~mask_file[file_of(sq)]);
    bishop_attack_masks[sq] = (mask_diagonal[diagonal_of(sq)]
      ^ mask_anti_diagonal[anti_diagonal_of(sq)]) & ~edges;
    bishop_attack_shifts[sq] = 64 - popcnt(bishop_attack_masks[sq]);

    uint64_t subset = 0;
    do {
      uint64_t index = subset;
      index = index * bishop_magics[sq];
      index = index >> bishop_attack_shifts[sq];
      bishop_attacks[sq][index] = get_bishop_attacks_for_init(sq, subset);
      subset = subset - bishop_attack_masks[sq] & bishop_attack_masks[sq];
    }
    while (subset);
  }
}

uint64_t get_bishop_attacks(const square sq, const uint64_t occ) {
  return bishop_attacks[sq][(occ & bishop_attack_masks[sq]) * bishop_magics[sq]
    >> bishop_attack_shifts[sq]];
}

uint64_t get_xray_bishop_attacks(const square sq, const uint64_t occ, uint64_t blockers) {
  const uint64_t attacks = get_bishop_attacks(sq, occ);
  blockers &= attacks;
  return attacks ^ get_bishop_attacks(sq, occ ^ blockers);
}

void init_squares_between() {
  for (square sq1 = a1; sq1 <= h8; ++sq1)
    for (square sq2 = a1; sq2 <= h8; ++sq2) {
      const uint64_t sqs = square_bb[sq1] | square_bb[sq2];
      if (file_of(sq1) == file_of(sq2) || rank_of(sq1) == rank_of(sq2))
        squares_between_bb[sq1][sq2] =
          get_rook_attacks_for_init(sq1, sqs) & get_rook_attacks_for_init(sq2, sqs);
      else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
        squares_between_bb[sq1][sq2] =
          get_bishop_attacks_for_init(sq1, sqs) & get_bishop_attacks_for_init(sq2, sqs);
    }
}

void init_line() {
  for (square sq1 = a1; sq1 <= h8; ++sq1)
    for (square sq2 = a1; sq2 <= h8; ++sq2) {
      if (file_of(sq1) == file_of(sq2) || rank_of(sq1) == rank_of(sq2))
        line[sq1][sq2] = (get_rook_attacks_for_init(sq1, 0) & get_rook_attacks_for_init(sq2, 0))
          | square_bb[sq1] | square_bb[sq2];
      else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
        line[sq1][sq2] = (get_bishop_attacks_for_init(sq1, 0) & get_bishop_attacks_for_init(sq2, 0))
          | square_bb[sq1] | square_bb[sq2];
    }
}

void init_pseudo_legal() {
  memcpy(pawn_attacks[white], white_pawn_attacks, sizeof white_pawn_attacks);
  memcpy(pawn_attacks[black], black_pawn_attacks, sizeof black_pawn_attacks);
  memcpy(pseudo_legal_attacks[knight], knight_attacks, sizeof knight_attacks);
  memcpy(pseudo_legal_attacks[king], king_attacks, sizeof king_attacks);
  for (square s = a1; s <= h8; ++s) {
    pseudo_legal_attacks[rook][s] = get_rook_attacks_for_init(s, 0);
    pseudo_legal_attacks[bishop][s] = get_bishop_attacks_for_init(s, 0);
    pseudo_legal_attacks[queen][s] = pseudo_legal_attacks[rook][s] |
      pseudo_legal_attacks[bishop][s];
  }
}

void init_tables() {
  init_rook_attacks();
  init_bishop_attacks();
  init_squares_between();
  init_line();
  init_pseudo_legal();
}
