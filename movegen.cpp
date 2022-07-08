#include "attacks.hpp"

vector<Moves> Board::is_square_attacked(int square) {
  U64 attack_board;
  U64 blockers = piece_co[(turn^1)];
  int opp_col = turn^1;
  
  //pawn attacks
  attack_board = Attacks::PAWN_ATTACKS[opp_col][square];
  if (attack_board & piece_boards[opp_col][PAWN_I]) {
    return true;
  }
  //knight attacks
  attack_board = Attacks::KNIGHT_ATTACKS[square];
  if (attack_board & piece_boards[opp_col][KNIGHT_I]) {
    return true;
  }
  //king attacks
  attack_board = Attacks::KING_ATTACKS[square];
  if (attack_board & piece_boards[opp_col][KING_I]) {
    return true;
  }
  //bishop attacks
  attack_board = Attacks::get_diag_attacks(square, both_blockers);
  if (attack_board & piece_boards[opp_col][KING_I]) {
    return true;
  }


  //bishop captures
  piece_board = piece_boards[turn][BISHOP_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    basic_rendering(both_blockers);
    attack_board = Attacks::get_diag_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //rook captures
  piece_board = piece_boards[turn][ROOK_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_line_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //queen captures
  piece_board = piece_boards[turn][QUEEN_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_queen_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //king captures 
  piece_board = piece_boards[turn][KING_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::KING_ATTACKS[from_square];
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  return move_list;
}

vector<Moves> Board::generate_castling_moves(){
  //If the turn is white
  if (turn == WHITE) {
    if (castling_rights[turn][KINGSIDE_I] == true) {
      if (CASTLING_BB[turn][KINGSIDE_I] & piece_co[turn]) {

      }
    }
  }
}

vector<Moves> Board::generate_piece_quiets(){
  int from_square, to_square;
  int pawn_shift;
  vector<Moves> move_list;
  move_list.reserve(218);
  Moves move;
  U64 piece_board, attack_board, double_attack_board;
  U64 blockers = piece_co[turn] | piece_co[(turn^1)];
  turn == WHITE ? pawn_shift = 8 : pawn_shift = -8;
  
  //pawn moves
  piece_board = piece_boards[turn][PAWN_I];
  //white pawn moves
  if (turn == WHITE) {
    attack_board = piece_board >> 8;
    BITMASK_CLEAR(attack_board, blockers);
    double_attack_board = attack_board >> 8;
    BITMASK_INTERSECT(double_attack_board, RANK_5);
    BITMASK_CLEAR(double_attack_board, blockers);
  }
  else {
    attack_board = piece_board << 8;
    BITMASK_CLEAR(attack_board, blockers);
    double_attack_board = attack_board << 8;
    BITMASK_INTERSECT(double_attack_board, RANK_5);
    BITMASK_CLEAR(double_attack_board, blockers);
  }
  
  //single pawn push
  while (attack_board) {
    to_square = pop_lsb(&attack_board);
    from_square = to_square + pawn_shift;
    //promotion
    if ((to_square >= 56) || (to_square <= 7)) {
      move_list.push_back(Moves(from_square, to_square, 0, 3, QUEEN_I));
      move_list.push_back(Moves(from_square, to_square, 0, 3, ROOK_I));
      move_list.push_back(Moves(from_square, to_square, 0, 3, KNIGHT_I));
      move_list.push_back(Moves(from_square, to_square, 0, 3, BISHOP_I));
    }
    else {
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //double pawn push 
  while (double_attack_board) {
    to_square = pop_lsb(&double_attack_board);
    from_square = to_square + (pawn_shift << 1);
    move_list.push_back(Moves(from_square, to_square));
  }

  //knight moves 
  piece_board = piece_boards[turn][KNIGHT_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::KNIGHT_ATTACKS[from_square];
    BITMASK_CLEAR(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //bishop moves
  piece_board = piece_boards[turn][BISHOP_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_diag_attacks(from_square, blockers);
    BITMASK_CLEAR(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //rook moves
  piece_board = piece_boards[turn][ROOK_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_line_attacks(from_square, blockers);
    BITMASK_CLEAR(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //queen moves
  piece_board = piece_boards[turn][QUEEN_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_queen_attacks(from_square, blockers);
    BITMASK_CLEAR(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //king moves 
  piece_board = piece_boards[turn][KING_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::KING_ATTACKS[from_square];
    BITMASK_CLEAR(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  return move_list;
}
vector<Moves> Board::generate_piece_captures(){
  int from_square, to_square;
  vector<Moves> move_list;
  
  Moves move;
  U64 piece_board, attack_board;
  U64 both_blockers = piece_co[turn] | piece_co[(turn^1)];
  U64 blockers = piece_co[(turn^1)];
  move_list.reserve(60);
  
  //pawn captures 
  piece_board = piece_boards[turn][PAWN_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::PAWN_ATTACKS[turn][from_square];
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      //promotion
      if ((to_square >= 56) || (to_square <= 7)) {
        move_list.push_back(Moves(from_square, to_square, 0, 3, QUEEN_I));
        move_list.push_back(Moves(from_square, to_square, 0, 3, ROOK_I));
        move_list.push_back(Moves(from_square, to_square, 0, 3, KNIGHT_I));
        move_list.push_back(Moves(from_square, to_square, 0, 3, BISHOP_I));
      }
      else {
        move_list.push_back(Moves(from_square, to_square));
      }
    }
  }
  //knight captures 
  piece_board = piece_boards[turn][KNIGHT_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::KNIGHT_ATTACKS[from_square];
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //bishop captures
  piece_board = piece_boards[turn][BISHOP_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    basic_rendering(both_blockers);
    attack_board = Attacks::get_diag_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //rook captures
  piece_board = piece_boards[turn][ROOK_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_line_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //queen captures
  piece_board = piece_boards[turn][QUEEN_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::get_queen_attacks(from_square, both_blockers);
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  //king captures 
  piece_board = piece_boards[turn][KING_I];
  while(piece_board) {
    from_square = pop_lsb(&piece_board);
    attack_board = Attacks::KING_ATTACKS[from_square];
    BITMASK_INTERSECT(attack_board, blockers);
    while (attack_board) {
      to_square = pop_lsb(&attack_board);
      move_list.push_back(Moves(from_square, to_square));
    }
  }
  return move_list;
}