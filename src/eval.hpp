
#ifndef EVAL_HPP
#define EVAL_HPP
constexpr int PIECE_VALUES[6] = {100, 300, 300, 500, 900, 20000};
//phase
constexpr int PHASE_VALUES[6] = {0, 1, 1, 2, 5, 0};
constexpr int MAX_PHASE = 26;

//mobility
constexpr int MG_MOBILITY[6] = {1, 4, 4, 2, 1, 0};
constexpr int EG_MOBILITY[6] = {0, 2, 2, 2, 2, 0};
constexpr int MAX_PIECE_BITS[6] = {4, 8, 13, 14, 27, 8};

//pawn structure values
constexpr int PASSED_VALUE[8] = {0, 20, 12, 7, 4, 3, 2, 0}; //From white point of view
constexpr int EG_PASSED_BONUS = 10;
constexpr int MG_DOUBLED_VALUE = 7;
constexpr int EG_DOUBLED_VALUE = 20;
constexpr int MG_ISOLATED_VALUE = 10;
constexpr int EG_ISOLATED_VALUE = 5;

//piece values
constexpr int MG_PIECE_VALUES[6] = {96, 312, 330, 530, 999, 0, };
constexpr int EG_PIECE_VALUES[6] = {105, 308, 335, 540, 992, 0, };

//king safety
constexpr int KING_ATTACKS[6] = {0, 4, 4, 6, 10, 0};

//additional terms
constexpr int MG_BISHOP_PAIR = 15;
constexpr int EG_BISHOP_PAIR = 32;



constexpr int MG_PST_VALUES[6][64] =
{
{
0, 0, 0, 0, 0, 0, 0, 0,
28, 68, 7, 67, 55, 61, -8, -32,
-43, -20, -8, 0, -2, 75, -14, -47, 
-26, -4, -15, -7, -2, -28, -16, -22,
-31, -5, -18, -4, -6, 0, -19, -47,
-27, -17, -8, -11, -1, -16, 5, -16, 
-28, -4, -30, -20, -20, 19, 23, -24,
0, 0, 0, 0, 0, 0, 0, 0,
}, 
{
-237, -106, -78, -84, 112, -90, -49, -63,
-72, 7, 68, 51, 70, 102, -37, -6,
9, 27, 25, 70, 111, 167, 97, 32, 
0, 13, 8, 40, 25, 24, 11, 42,
9, -10, 27, 19, 31, 10, 9, -13, 
-18, 2, 17, 21, 1, 14, 6, -17,
-26, -8, 14, 11, 21, 28, 4, 1,
-51, -8, -46, -7, -18, -5, -10, -10, 
},
{
-34, -50, -82, -66, -80, -38, 37, -40, 
15, 14, -4, 41, -26, 73, 0, 13,
-17, -2, 48, 21, 57, 77, 64, 15, 
-17, 12, 27, 29, 25, -1, 11, 22,
-14, 20, 9, 45, 32, 9, 20, -8, 
38, 42, 27, 18, 18, 39, 15, 32,
-13, 40, 14, 13, 28, 69, 62, 23, 
38, 27, 13, 22, 16, 20, 3, 15, 
},
{
-9, 24, -5, 47, 79, 38, 0, 21,
-23, -29, 1, 22, 16, 102, -11, -5, 
-9, -35, -12, 18, 25, 31, 47, 39,
-51, -22, -36, 27, -42, 24, -18, -25, 
-49, -45, -23, -62, -13, -47, -55, -37,
-7, -58, -11, -5, -26, 7, 21, 4,
-21, -46, -22, -21, -30, -18, -10, -87, 
-20, -4, -2, 6, 4, 2, -50, -19,
},
{
-58, 25, 55, 31, 35, 62, 26, 46,
-53, -46, -33, -70, -76, 32, 28, 91,
-19, -50, -34, -2, 19, 51, -15, 23, 
-16, -35, -14, -46, -14, -21, 16, -9,
-3, -44, -33, -26, -17, -14, 9, -25,
-45, -8, -2, -7, -10, -5, -13, -22, 
-8, -1, -1, 3, 14, 28, 45, 10,
4, 3, 10, 9, -12, 7, -54, -10,
},
{
-110, 67, 9, 29, -63, -16, 46, -23, 
-25, -7, 13, -36, -39, 24, -86, 7,
22, 5, -11, 11, -31, 55, 40, -1,
-27, 17, -48, -45, -85, -34, -14, -51, 
-10, 14, 39, -31, -47, -63, -52, -34,
-2, 15, 2, -53, -67, -33, -11, 4, 
-23, -22, -22, -64, -59, -3, 9, 7,
-50, 29, 17, -66, 11, -26, 19, 13,
},
};
constexpr int EG_PST_VALUES[6][64] =
{
{
0, 0, 0, 0, 0, 0, 0, 0,
188, 154, 162, 138, 104, 153, 181, 183,
114, 105, 83, 69, 59, 30, 68, 96, 
32, 17, 9, 0, -14, 4, 14, 3,
6, 1, -14, -22, -23, -18, -3, -4, 
7, 14, -27, 1, -17, -8, -11, -13,
9, 7, 10, 12, 5, -10, -14, -16,
0, 0, 0, 0, 0, 0, 0, 0, 
},
{
-61, -45, -67, 11, -42, -8, -19, -59, 
35, -26, -62, -61, -17, -36, 0, -26,
-53, -21, 11, -30, -6, -37, -5, -18,
-1, 11, 19, 36, 32, 36, 19, -42, 
0, 17, 10, 22, 14, 1, 13, -22,
-36, -4, -11, 20, 26, -2, -2, 14, 
-38, -50, -4, 14, 2, 19, -6, 0,
-55, -12, 15, 11, -5, -26, -18, -10, 
},
{
-6, -46, -12, 4, -22, 1, -2, 5,
-45, -19, -11, -32, 1, -15, -7, -2,
-7, 3, -25, -1, -2, -2, -35, 8, 
14, 7, -11, 20, 3, -7, -3, -26,
-17, -18, -1, 9, -2, 10, -19, -40,
-10, -3, -9, 15, 9, 1, 16, -7, 
21, -19, -8, 25, -11, 3, -16, -8,
-20, -5, -6, 9, 4, -20, -14, -36,
},
{
20, 7, -15, -2, 2, -5, -4, -2,
-7, 11, 1, 1, -3, -21, 7, -5, 
6, 23, 3, -7, -25, -32, -24, -22,
9, -2, 9, -20, 6, -5, -22, -7,
5, 15, -19, 8, -13, -9, -14, -16, 
-3, 20, -11, -11, 2, -10, -58, -14,
16, 2, 11, 6, -5, -10, -3, -20,
-5, 0, -5, -12, -12, -15, 3, -24, 
},
{
35, 53, 2, 33, -29, 19, 50, 62,
40, 17, 25, 73, 64, 58, 34, -48, 
-9, -23, 57, 1, 69, 33, 41, -30, 
-8, 24, -25, 55, 17, 20, 36, 2, 
-15, 30, 59, 6, 20, 1, 60, 46,
7, -24, 21, 2, 18, 20, 48, 45, 
27, 9, -11, 13, -18, -28, -56, 22, 
-5, -18, 22, -12, 31, -45, 39, 8,
},
{
-124, 9, -33, -31, -32, 23, -1, -71,
-7, 27, -35, -21, 17, -2, 19, 4,
-38, -5, 32, -7, 12, 17, 35, 19, 
-34, 1, 14, 31, 9, 23, 14, 12, 
-43, -27, -1, 22, 10, 18, 0, -15,
1, -8, 9, 28, 25, 12, -14, -14, 
-32, 1, 27, 33, 33, 11, -7, -13,
-18, -39, -11, 7, -12, -19, -42, -49, 
},
};


constexpr int KING_EDGE[64] = 
{
  -100,  -95,  -90,  -90,  -90,  -90,  -95,  -100,  
  -95,  -50,  -50,  -50,  -50,  -50,  -50,  -95,  
  -90,  -50,  -10,  -10,  -10,  -10,  -50,  -90,  
  -90,  -50,  -10,    0,    0,  -10,  -50,  -90,  
  -90,  -50,  -10,    0,    0,  -10,  -50,  -90,  
  -90,  -50,  -10,  -10,  -10,  -10,  -50,  -90,  
  -95,  -50,  -50,  -50,  -50,  -50,  -50,  -95,  
  -100,  -95,  -90,  -90,  -90,  -90,  -95,  -100,
};

constexpr int flip_board[2][64] = {
{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63
},
{
  56, 57, 58, 59, 60, 61, 62, 63,
  48, 49, 50, 51, 52, 53, 54, 55,
  40, 41, 42, 43, 44, 45, 46, 47,
  32, 33, 34, 35, 36, 37, 38, 39,
  24, 25, 26, 27, 28, 29, 30, 31,
  16, 17, 18, 19, 20, 21, 22, 23,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7
}
};


#endif



