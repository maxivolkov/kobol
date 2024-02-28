#pragma once

#include "move.h"
#include "position.h"

inline int32_t pst_mg[6 * 64]
{
  //PAWN MG
50, 50, 50, 50, 50, 50, 50, 50,
88, 107, 73, 97, 94, 107, 66, 38,
41, 44, 53, 56, 75, 73, 55, 36,
33, 46, 41, 47, 48, 46, 49, 31,
27, 37, 40, 44, 47, 43, 45, 27,
27, 35, 34, 34, 38, 40, 50, 33,
26, 42, 33, 30, 34, 49, 58, 30,
50, 50, 50, 50, 50, 50, 50, 50,
//KNIGHT MG
58, 114, 135, 135, 169, 106, 139, 95,
112, 123, 176, 165, 160, 180, 150, 140,
129, 177, 171, 181, 194, 214, 187, 173,
150, 166, 162, 180, 174, 189, 169, 166,
149, 161, 162, 160, 168, 166, 166, 151,
143, 148, 158, 159, 163, 160, 163, 147,
138, 129, 150, 152, 154, 161, 148, 146,
104, 145, 128, 137, 148, 142, 146, 142,
//BISHOP MG
146, 169, 127, 141, 149, 147, 168, 161,
158, 171, 159, 159, 180, 192, 171, 147,
171, 188, 186, 187, 184, 196, 192, 181,
166, 169, 178, 192, 185, 190, 168, 170,
163, 177, 176, 183, 186, 173, 172, 170,
167, 175, 175, 173, 176, 180, 175, 172,
166, 177, 177, 169, 172, 176, 183, 166,
154, 170, 171, 162, 167, 166, 151, 156,
//ROOK MG
246, 255, 243, 257, 257, 241, 242, 247,
246, 249, 264, 267, 273, 272, 241, 254,
232, 245, 249, 248, 241, 259, 265, 240,
224, 232, 238, 247, 242, 253, 233, 227,
221, 225, 234, 235, 238, 236, 249, 227,
220, 230, 234, 232, 239, 240, 239, 226,
221, 236, 233, 238, 241, 250, 243, 211,
229, 231, 235, 239, 240, 240, 223, 229,
//QUEEN MG
432, 451, 461, 455, 482, 474, 466, 464,
443, 432, 451, 460, 444, 475, 461, 470,
451, 450, 453, 459, 468, 489, 482, 483,
440, 442, 448, 447, 449, 464, 453, 457,
453, 442, 449, 448, 452, 453, 456, 455,
447, 458, 450, 451, 452, 456, 462, 458,
437, 449, 459, 454, 457, 462, 455, 453,
453, 449, 453, 459, 449, 445, 439, 429,
//KING MG
-5, 35, 27, 15, -18, -8, 11, 11,
18, 12, 12, 18, 8, 4, -6, -15,
16, 13, 21, 5, 5, 20, 17, -1,
0, -4, 0, -10, -10, -11, -7, -30,
-12, 8, -13, -33, -40, -29, -20, -31,
3, -1, -18, -38, -39, -30, -11, -13,
6, 7, -6, -36, -28, -14, 7, 8,
-3, 22, 14, -29, 4, -12, 17, 14,
};

inline int32_t pst_eg[6 * 64]
{
  //PAWN EG
50, 50, 50, 50, 50, 50, 50, 50,
138, 135, 126, 114, 120, 116, 132, 142,
95, 98, 91, 84, 77, 75, 90, 90,
64, 58, 54, 51, 46, 50, 55, 55,
53, 50, 44, 42, 43, 41, 46, 45,
48, 48, 42, 46, 44, 41, 42, 41,
53, 49, 48, 48, 50, 44, 44, 42,
50, 50, 50, 50, 50, 50, 50, 50,
//KNIGHT EG
114, 118, 134, 125, 128, 124, 109, 94,
126, 137, 131, 140, 136, 129, 130, 114,
126, 132, 145, 144, 139, 137, 131, 121,
133, 140, 149, 150, 149, 146, 142, 132,
131, 136, 146, 150, 148, 146, 142, 130,
129, 138, 139, 145, 143, 137, 130, 127,
120, 129, 135, 138, 138, 131, 130, 118,
126, 116, 129, 132, 130, 130, 117, 107,
//BISHOP EG
144, 139, 143, 146, 146, 145, 143, 138,
144, 147, 150, 144, 148, 144, 147, 140,
146, 144, 148, 146, 148, 150, 148, 146,
146, 151, 152, 152, 153, 151, 148, 148,
144, 146, 152, 154, 149, 150, 145, 144,
142, 147, 152, 151, 153, 147, 145, 140,
142, 142, 145, 149, 150, 145, 142, 135,
138, 146, 143, 147, 147, 144, 145, 142,
//ROOK EG
253, 250, 254, 251, 252, 253, 252, 251,
252, 253, 251, 251, 245, 248, 253, 250,
252, 251, 249, 250, 250, 247, 248, 248,
251, 251, 255, 250, 251, 252, 250, 252,
252, 254, 254, 253, 252, 251, 248, 247,
250, 251, 250, 252, 249, 249, 249, 244,
248, 247, 251, 252, 249, 249, 245, 249,
246, 248, 249, 248, 246, 246, 248, 240,
//QUEEN EG
459, 468, 471, 472, 467, 463, 462, 471,
453, 472, 473, 475, 491, 466, 464, 456,
448, 460, 463, 483, 481, 468, 462, 457,
463, 472, 469, 481, 491, 478, 490, 475,
446, 474, 471, 485, 476, 478, 476, 468,
455, 446, 466, 464, 467, 471, 467, 462,
453, 449, 441, 451, 451, 446, 443, 444,
443, 443, 445, 436, 458, 445, 453, 439,
//KING EG
-37, -21, -11, -12, -5, 5, 0, -6,
-9, 3, 2, 4, 3, 13, 7, 4,
-1, 3, 5, 3, 4, 12, 13, 1,
-8, 4, 6, 10, 7, 9, 5, -1,
-12, -7, 6, 10, 12, 7, 0, -7,
-13, -5, 4, 10, 11, 7, 1, -6,
-16, -8, 2, 7, 7, 2, -5, -11,
-27, -20, -11, -3, -10, -4, -14, -23,
};

inline int32_t pst_pos_mg[2][6][64];
inline int32_t pst_pos_eg[2][6][64];
inline int32_t pst_pos[29][2][6][64];

int32_t eval();
int32_t eval(const move& m, bool q = false);
void init_eval();
