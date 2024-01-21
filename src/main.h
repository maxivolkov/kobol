#pragma once
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
	black_pawn,
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
