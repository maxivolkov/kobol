#pragma once

#include <string>
#include "bitboard.h"
#include "move.h"
#include "zobrist.h"
#include "main.h"

struct undo_info
{
	bool in_check;
	uint64_t entry;
	piece captured;
	square epsq;
	uint64_t hash;
	int move50;

	constexpr undo_info() : in_check(false), entry(0), captured(no_piece), epsq(no_square), hash(0), move50(0)
	{
	}

	undo_info(const undo_info& prev) :
		in_check(false), entry(prev.entry), captured(no_piece), epsq(no_square), hash(0), move50(0)
	{
	}
};

class position
{

	uint64_t hash_;

public:
	color side_to_play;
	bool is_in_check = false;
	int move50{};
	int history_index;
	int phase = 28;
	piece board[nsquares];

	uint64_t piece_bb[npieces];
	undo_info history[512];
	uint64_t checkers;
	uint64_t pinned;

	position() : hash_(0), side_to_play(white), history_index(0), board{},
		piece_bb{ 0 }, checkers(0), pinned(0)
	{
		for (auto& i : board) i = no_piece;
		history[0] = undo_info();
	}

	void put_piece(const piece pc, const square s)
	{
		board[s] = pc;
		piece_bb[pc] |= square_bb[s];
		hash_ ^= zobrist_table[pc][s];
	}

	void remove_piece(const square s)
	{
		hash_ ^= zobrist_table[board[s]][s];
		piece_bb[board[s]] &= ~square_bb[s];
		board[s] = no_piece;
	}

	void move_piece(square from, square to);
	void move_piece_quiet(square from, square to);

	friend std::ostream& operator<<(std::ostream& os, const position& p);
	void set_fen(const std::string& fen = start_fen);
   [[nodiscard]] std::string get_fen() const;

	void clear()
	{
		side_to_play = white;
		history_index = 0;
		hash_ = 0;
		pinned = 0;
		checkers = 0;
		for (auto& n : board)
			n = no_piece;
		for (unsigned long long& n : piece_bb)
			n = 0;
		history[0] = undo_info();
	}

	position& operator=(const position&) = delete;
	bool operator==(const position& other) const { return hash_ == other.hash_; }

   [[nodiscard]] uint64_t bitboard_of(const piece pc) const { return piece_bb[pc]; }
   [[nodiscard]] uint64_t bitboard_of(const color c, const piece_type pt) const { return piece_bb[make_piece(c, pt)]; }

   [[nodiscard]] uint64_t all_pieces() const
	{
		return
			piece_bb[white_pawn] | piece_bb[white_knight] | piece_bb[white_bishop] | piece_bb[white_rook] | piece_bb[
				white_queen] | piece_bb[white_king] |
					piece_bb[black_pawn] | piece_bb[black_knight] | piece_bb[black_bishop] | piece_bb[black_rook] | piece_bb[
						black_queen] | piece_bb[black_king];
	}

   [[nodiscard]] piece at(const square sq) const { return board[sq]; }
   [[nodiscard]] color color_us() const { return side_to_play; }
   [[nodiscard]] color color_en() const { return ~side_to_play; }
	void move_list(color color, move* list, int& count);
	void move_list_q(color color, move* list, int& count);
	void move_list(move* list, int& count);
	void move_list_q(move* list, int& count);
   [[nodiscard]] bool in_check(color color) const;
   [[nodiscard]] bool in_check() const;
	bool is_legal(const move& mv);

   [[nodiscard]] int game_phase() const
	{
		const int result = popcnt(all_pieces()) - 4;
		return result < 0 ? 0 : result;
	}

   [[nodiscard]] bool is_repetition() const
	{
		for (int n = history_index - 2; n >= history_index - move50; n -= 2)
		{
			if (n < 0)
				return false;
			if (history[n].hash == hash_)
				return true;
		}
		return false;
	}

   [[nodiscard]] int ply() const { return history_index; }
   [[nodiscard]] uint64_t get_hash() const { return hash_; }

   [[nodiscard]] inline uint64_t diagonal_sliders(color c) const;
   [[nodiscard]] inline uint64_t orthogonal_sliders(color c) const;
   [[nodiscard]] inline uint64_t all_pieces(color c) const;
   [[nodiscard]] inline uint64_t attackers_from(color c, square s, uint64_t occ) const;
   [[nodiscard]] inline uint64_t attackers(square s) const;
	void make_null();
	void unmake_null();
	void make_move(const move& m);
	void unmake_move(const move& m);
   [[nodiscard]] inline bool not_only_pawns() const;
	move* generate_moves(color us, move* list, bool quietmove = true);
};

inline uint64_t position::diagonal_sliders(const color c) const
{
	return c == white ? piece_bb[white_bishop] | piece_bb[white_queen] : piece_bb[black_bishop] | piece_bb[black_queen];
}

inline uint64_t position::orthogonal_sliders(const color c) const
{
	return c == white ? piece_bb[white_rook] | piece_bb[white_queen] : piece_bb[black_rook] | piece_bb[black_queen];
}

inline bool position::not_only_pawns() const
{
	return side_to_play == white
		? piece_bb[white_knight] || piece_bb[white_bishop] || piece_bb[white_rook] || piece_bb[white_queen]
		: piece_bb[black_knight] || piece_bb[black_bishop] || piece_bb[black_rook] || piece_bb[black_queen];
}

inline uint64_t position::all_pieces(const color c) const
{
	return c == white
		? piece_bb[white_pawn] | piece_bb[white_knight] | piece_bb[white_bishop] |
		piece_bb[white_rook] | piece_bb[white_queen] | piece_bb[white_king]
		: piece_bb[black_pawn] | piece_bb[black_knight] | piece_bb[black_bishop] |
		piece_bb[black_rook] | piece_bb[black_queen] | piece_bb[black_king];
}

inline uint64_t position::attackers_from(const color c, const square s, const uint64_t occ) const
{
	return c == white
		? (pawnattacks(black, s) & piece_bb[white_pawn]) |
		(attacks<knight>(s, occ) & piece_bb[white_knight]) |
		(attacks<bishop>(s, occ) & (piece_bb[white_bishop] | piece_bb[white_queen])) |
		(attacks<rook>(s, occ) & (piece_bb[white_rook] | piece_bb[white_queen]))
		: (pawnattacks(white, s) & piece_bb[black_pawn]) |
		(attacks<knight>(s, occ) & piece_bb[black_knight]) |
		(attacks<bishop>(s, occ) & (piece_bb[black_bishop] | piece_bb[black_queen])) |
		(attacks<rook>(s, occ) & (piece_bb[black_rook] | piece_bb[black_queen]));
}

inline uint64_t position::attackers(const square s) const
{
	const uint64_t occ = all_pieces();
	return side_to_play == black
		? (pawnattacks(black, s) & piece_bb[white_pawn]) |
		(attacks<knight>(s, occ) & piece_bb[white_knight]) |
		(attacks<bishop>(s, occ) & (piece_bb[white_bishop] | piece_bb[white_queen])) |
		(attacks<rook>(s, occ) & (piece_bb[white_rook] | piece_bb[white_queen]))
		: (pawnattacks(white, s) & piece_bb[black_pawn]) |
		(attacks<knight>(s, occ) & piece_bb[black_knight]) |
		(attacks<bishop>(s, occ) & (piece_bb[black_bishop] | piece_bb[black_queen])) |
		(attacks<rook>(s, occ) & (piece_bb[black_rook] | piece_bb[black_queen]));
}

class move_list
{
public:
	explicit move_list(position& p, const color color) : last_(p.generate_moves(color, list))
	{
	}

	explicit move_list(position& p) : last_(p.generate_moves(p.color_us(), list))
	{
	}

   [[nodiscard]] const move* begin() const { return list; }
   [[nodiscard]] const move* end() const { return last_; }
   [[nodiscard]] size_t size() const { return last_ - list; }
	move list[218];

private:
	move* last_;
};

inline position pos;