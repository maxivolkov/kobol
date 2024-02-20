#pragma once
#include <cstdint>
#include <string>

enum square : int
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8,
	no_square
};

enum color : int
{
	white,
	black
};

enum piece_type : int
{
	pawn,
	knight,
	bishop,
	rook,
	queen,
	king
};

enum piece : int
{
	white_pawn,
	white_knight,
	white_bishop,
	white_rook,
	white_queen,
	white_king,
	black_pawn = 8,
	black_knight,
	black_bishop,
	black_rook,
	black_queen,
	black_king,
	no_piece
};

enum file : int
{
	a_file,
	b_file,
	c_file,
	d_file,
	e_file,
	f_file,
	g_file,
	h_file
};

enum rank : int
{
	rank_1,
	rank_2,
	rank_3,
	rank_4,
	rank_5,
	rank_6,
	rank_7,
	rank_8
};

enum move_flags : int
{
	quiet = 0b0000,
	double_push = 0b0001,
	oo = 0b0010,
	ooo = 0b0011,
	capture = 0b1000,
	en_passant = 0b1010,
	promo = 0b0100,
	promos = 0b0111,
	promo_caps = 0b1100,
	promo_knight = 0b0100,
	promo_bishop = 0b0101,
	promo_rook = 0b0110,
	promo_queen = 0b0111,
	promo_cap_knight = 0b1100,
	promo_cap_bishop = 0b1101,
	promo_cap_rook = 0b1110,
	promo_cap_queen = 0b1111
};

enum direction : int
{
	north = 8,
	northeast = 9,
	east = 1,
	southeast = -7,
	south = -8,
	southwest = -9,
	west = -1,
	northwest = 7,
	northnorth = 16,
	southsouth = -16
};

constexpr size_t nsquares = 64;
constexpr size_t ncolors = 2;
constexpr size_t ndirs = 8;
constexpr size_t npiece_types = 6;
const std::string piece_str = "PNBRQK~>pnbrqk.";
const std::string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
constexpr size_t npieces = 15;

constexpr color operator~(const color c)
{
	return static_cast<color>(c ^ black);
}

constexpr piece make_piece(const color c, const piece_type pt)
{
	return static_cast<piece>((c << 3) + pt);
}

constexpr piece_type type_of(const piece pc)
{
	return static_cast<piece_type>(pc & 0b111);
}

constexpr color color_of(const piece pc)
{
	return static_cast<color>((pc & 0b1000) >> 3);
}

constexpr square operator+(const square s, const direction d)
{
	return static_cast<square>(static_cast<int>(s) + static_cast<int>(d));
}

constexpr square operator-(const square s, const direction d)
{
	return static_cast<square>(static_cast<int>(s) - static_cast<int>(d));
}

inline square& operator+=(square& s, const direction d) { return s = s + d; }
inline square& operator-=(square& s, const direction d) { return s = s - d; }
inline square& operator++(square& s) { return s = static_cast<square>(static_cast<int>(s) + 1); }

constexpr rank rank_of(const square s) { return static_cast<rank>(s >> 3); }
constexpr file file_of(const square s) { return static_cast<file>(s & 0b111); }
constexpr int diagonal_of(const square s) { return 7 + rank_of(s) - file_of(s); }
constexpr int anti_diagonal_of(const square s) { return rank_of(s) + file_of(s); }
constexpr square create_square(const file f, const rank r) { return static_cast<square>(r << 3 | f); }

constexpr rank relative_rank(const color c, const rank r)
{
	return c == white ? r : static_cast<rank>(rank_8 - r);
}

constexpr direction relative_dir(const color c, direction d)
{
	return static_cast<direction>(c == white ? d : -d);
}

inline const std::string sqstr[65] = {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
  "None"
};

inline const std::string move_typestr_uci[8] = {
  "", "", "", "", "n", "b", "r", "q"
};

inline const char* move_typestr[16] = {
  "", "", " O-O", " O-O-O", "N", "B", "R", "Q", " (capture)", "", " e.p.", "", "N", "B", "R", "Q"
};

#if defined(_MSC_VER)
#include <bit>
inline int popcnt(const uint64_t b) { return std::popcount(b); }
inline square lsb(const uint64_t b) { return static_cast<square>(std::countr_zero(b)); }
#elif defined(__GNUC__)
inline int popcnt(const uint64_t b) { return __builtin_popcountll(b); }
inline Square lsb(const uint64_t b) { return static_cast<Square>(__builtin_ctzll(b)); }
#endif

inline square sparse_popcnt(uint64_t b)
{
	int count = 0;
	while (b)
	{
		count++;
		b &= b - 1;
	}
	return static_cast<square>(count);
}

inline square pop_lsb(uint64_t* b)
{
	int bsf = lsb(*b);
	*b &= *b - 1;
	return static_cast<square>(bsf);
}
