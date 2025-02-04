#include "attacks.hpp"
#include <cstring>
U64 SQUARES_BB[64];
U64 CASTLING_BB[2][2];
U64 Attacks::FILES_BB[8], Attacks::RANKS_BB[8];

U64 Attacks::DIAG_MASKS[64];
U64 Attacks::LINE_MASKS[64];

U64 Attacks::KNIGHT_ATTACKS[64], Attacks::KING_ATTACKS[64], Attacks::PAWN_ATTACKS[2][64];
U64 Attacks::DIAG_ATTACKS[64][512];
U64 Attacks::LINE_ATTACKS[64][4096];

//Gets maximum distance between 2 squares whether that be rows or cols on chess board
int Attacks::board_distance(int square_1, int square_2) {
    int row_dif, col_dif;
    row_dif = abs(square_2 / 8 - square_1 / 8);
    col_dif = abs((square_2 & 7) - (square_1 & 7));
    return max(row_dif, col_dif);
} 
void Attacks::init_bb_values() {
  //Initilize the square bitboard
  for (int i = 0; i < 64; i++) {
    SQUARES_BB[i] = 1ULL << i; 
  }
  //Initialize ranks and files in bb 
  for (int i = 0; i < 8; i++) {
    FILES_BB[i] = 0x0101010101010101ULL << i;
    RANKS_BB[i] = 0xffULL << (8 * i);
  }
  CASTLING_BB[WHITE][KINGSIDE_I] = (SQUARES_BB[61] | SQUARES_BB[62]);
  CASTLING_BB[WHITE][QUEENSIDE_I] = (SQUARES_BB[59] | SQUARES_BB[58] | SQUARES_BB[57]);
  CASTLING_BB[BLACK][KINGSIDE_I] = (SQUARES_BB[5] | SQUARES_BB[6]);
  CASTLING_BB[BLACK][QUEENSIDE_I] = (SQUARES_BB[1] | SQUARES_BB[2] | SQUARES_BB[3]);
}

//Initialize stepper pieces (K, N, P) attacks
void Attacks::init_step_tables() {
  for (int square = 0; square < 64; square++) {
    KNIGHT_ATTACKS[square] = sliding_attacks(square, ALL_BB, KNIGHT_DELTAS);
    KING_ATTACKS[square] = sliding_attacks(square, ALL_BB, KING_DELTAS);
    PAWN_ATTACKS[WHITE][square] = sliding_attacks(square, ALL_BB, {-9, -7});
    PAWN_ATTACKS[BLACK][square] = sliding_attacks(square, ALL_BB, {9, 7});
  }
}

//Generate diag masks and attack table
void Attacks::init_diag_tables() {
  for (int square = 0; square < 64; square++) {
    //Masks
    DIAG_MASKS[square] = sliding_attacks(square, EMPTY_BB, DIAG_DELTAS);
    BITMASK_CLEAR(DIAG_MASKS[square], EDGES);
    //Attack Table
    int blocker_bits = count_bits(DIAG_MASKS[square]);
    int blocker_i = (1 << blocker_bits);
    for (int i = 0; i < blocker_i; i++) {
      U64 blockers = get_blockers(i, blocker_bits, DIAG_MASKS[square]);
      int magic_index = (blockers * diag_magics[square]) >> (64 - diag_relevant_bits[square]);
      DIAG_ATTACKS[square][magic_index] = sliding_attacks(square, blockers, DIAG_DELTAS);
    }
  }
}

//Generate line masks and attack table
void Attacks::init_line_tables() {
  for (int square = 0; square < 64; square++) {
    //Masks
    LINE_MASKS[square] = sliding_attacks(square, EMPTY_BB, LINE_DELTAS);
    BITMASK_CLEAR(LINE_MASKS[square], get_edges(square));
    BITMASK_CLEAR(LINE_MASKS[square], CORNERS);
    //Attack Table
    U64 blocker_bits = count_bits(LINE_MASKS[square]);
    for (int i = 0; i < (1 << blocker_bits); i++) {
      U64 blockers = get_blockers(i, blocker_bits, LINE_MASKS[square]);
      int magic_index = (blockers * line_magics[square]) >> (64 - line_relevant_bits[square]);
      LINE_ATTACKS[square][magic_index] = sliding_attacks(square, blockers, LINE_DELTAS);
    }
  }
}

void Attacks::init() {
  init_bb_values();
  init_step_tables();
  init_diag_tables();
  init_line_tables();
}

//Get blockers for diag and line tables
inline U64 Attacks::get_blockers(int index, int bits, U64 mask) {
  U64 blockers = EMPTY_BB;
  for (int i = 0; i < bits; i++) {
    int square = pop_lsb(&mask);
    if (index & (1 << i)) {
      blockers |= SQUARES_BB[square];
    }
  }
  return blockers;
}   

//Used to cut off edges of the board which are not needed in diag or line masks
inline U64 Attacks::get_edges(int square)  {
  U64 square_rank = RANKS_BB[(int)(square/8)];
  U64 square_file = FILES_BB[(square & 7)];
  
  return ((RANKS_BB[0] & ~square_rank) | (RANKS_BB[7] & ~square_rank) | (FILES_BB[0] & ~square_file) | (FILES_BB[7] & ~square_file));
}

//Checks which squares can be attacked. 
inline U64 Attacks::sliding_attacks(int square, U64 occupied, vector<int> deltas) {
  U64 attack_bb = EMPTY_BB;
  for (auto delta : deltas) {
    int square_to = square;
    
    while (true) {
      square_to += delta;
      if (((square_to < 0) || (square_to >= 64)) || (board_distance(square_to, square_to-delta) > 2)) {
        break;
      }
      attack_bb |= SQUARES_BB[square_to];
      if (occupied & (SQUARES_BB[square_to])) {
        break;
      }
    }
  }
  return attack_bb;
}


