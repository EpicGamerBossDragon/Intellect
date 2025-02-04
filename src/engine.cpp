#include "engine.hpp"
#include "eval.hpp"

inline void Engine::update_pv(Moves &move, int ply) {
    pv[ply][ply] = move; //update pv table
    
    for (int next_ply = ply + 1; next_ply < pv_len[ply + 1]; next_ply++){
        // copy move from deeper ply into a current ply's line
        pv[ply][next_ply] = pv[ply + 1][next_ply];
    }
        // adjust PV length
    pv_len[ply] = pv_len[ply + 1];           
}

//check engine limits
inline bool Engine::check_limits(int depth) {
    int elapsed_time = duration_cast<milliseconds>(steady_clock::now() - start_time).count();
    if (elapsed_time >= (time_for_move) || (depth >= MAX_DEPTH-1)) {stop = true;}
    return stop;
}

int Engine::set_tt_memory(TTMAP &tt_table, int megabytes) {
    auto entry_size = sizeof(U64) + sizeof(TTEntry) + sizeof(void*);
    auto bucket_size = sizeof(void*);
    auto admin_size = 3 * sizeof(void*) + sizeof(size_t);
    int entries = megabytes * 1e6 - tt_table.max_bucket_count() * bucket_size - admin_size;
    entries /= entry_size;
    return entries;
}

//quiesce search (run till position is stable)
inline int Engine::quiesce(Board &board, int alpha, int beta, int depth = QUIESCE_MAX_DEPTH) {
    int capture_moves = 0;
    int value;
    Moves best_m, tt_move;
    int best_v = -MAX_V;
    bool is_pv = beta - alpha != 1;
    TTFLAG flag = TT_ALPHA;
    //count nodes
    nodes++;
    
    if (nodes & 2047 && check_limits(depth)) {return 0;}
    //if this quiesce is really deep then just return eval
    if (depth <= 0) {return evaluation(board);} 

    //check transposition table: if it is not root, 
    if(!is_pv && tt_table.find(board.zobrist_hash) != tt_table.end()) {   
        if(tt_table[board.zobrist_hash].flag == TT_EXACT) {return tt_table[board.zobrist_hash].value;}
        else if (tt_table[board.zobrist_hash].flag == TT_ALPHA && tt_table[board.zobrist_hash].value <= alpha) {return tt_table[board.zobrist_hash].value;}
        else if (tt_table[board.zobrist_hash].flag == TT_BETA && tt_table[board.zobrist_hash].value >= beta) {return tt_table[board.zobrist_hash].value;}
        tt_move = (tt_table[board.zobrist_hash].move);
    }

    int stand_pat = evaluation(board);

    //eval pruning for quiesce search
    //if the static eval is above beta we don't need to keep searching
    if (stand_pat >= beta) {return stand_pat;}
    
    //delta pruning
    //if we cannot improve our position even with a massive gain then this node can be pruned
    if (stand_pat + DELTA_MARGIN < alpha) {return stand_pat;}

    if (stand_pat > alpha) {alpha = stand_pat;}
    best_v = stand_pat;
    //iterate through moves
    vector<Moves> moves;
    moves.reserve(MOVE_LIST_RESERVE);
    board.generate_piece_captures(moves);
    score_quiesce_moves(board, moves, tt_move);
    sort(moves.begin(), moves.end(), move_sort());
    //run through capture moves
    for (Moves &move : moves) {
        //static engine evaluation pruning (SEE pruning)
        //this prunes bad captures right away
        if (!see(board, move, 0)) {continue;}
        board.push(move); // play move
        //check move legality; if it isn't legal skip the move
        if(board.in_check(board.turn^1)) {
            board.pop();
            continue;
        }
        capture_moves++;
        value = -quiesce(board, -beta, -alpha, depth-1);
        board.pop();
        //alpha beta pruning
        if (value > best_v) {
            best_v = value;
            best_m = move;
            if (value >= beta) {
                flag = TT_BETA;
                break;
            }
            if (value > alpha) {
                flag = TT_EXACT;
                alpha = value;
            }
        }        
    }

    //store transposition table values
    if (tt_table.size() >= TT_MAX_SIZE) {
        tt_table.erase(tt_table.begin());
    }
    tt_table[board.zobrist_hash] = TTEntry(best_v, best_m, 0, flag);

    return best_v;
}

//run a negamax (PVS) search
inline int Engine::negamax(Board &board, int alpha, int beta, int depth, int ply=0, NodeInfo info = NodeInfo()) {
    int value;
    int best_v = -MAX_V;
    Moves best_m, tt_move;
    int legal_moves = 0; // number of legal moves tested in this node
    TTFLAG flag = TT_ALPHA; //presume we will fail high
    bool is_pv = beta - alpha != 1; 
    bool not_tactical;

    //store info
    bool null = info.null; 
    Moves prev_move = info.prev_move; 
    bool in_check;
    //if we know it is a check just use that
    if (prev_move == Moves()) {
        in_check = board.in_check(board.turn);
    }
    else {
        in_check = info.prev_check;
    }
    

    pv_len[ply] = ply;

    //Check for draw (threefold repition, 50 move and insufficient material)
    if (board.is_repetition() || is_insufficient(board) || (board.halfmove_clock >= 100 && !in_check)) {
        return DRAW_V;
    }

    //eval/quiesce
    if (depth <= 0) {
        //search extension for being in check
        if (in_check) {depth++;}
        //get eval
        else {return quiesce(board, alpha, beta);}   
    }
    //count node
    nodes++;
    //checks if the position is in the transposition table
    bool is_tt = tt_table.find(board.zobrist_hash) != tt_table.end(); 
    //check transposition table if the position has appeared before
    TTEntry tt_entry;
    if(is_tt) {
        tt_entry = tt_table[board.zobrist_hash];
        if(ply && !is_pv && tt_entry.depth >= depth) {   
            if(tt_entry.flag == TT_EXACT) {return tt_entry.value;}
            else if (tt_entry.flag == TT_ALPHA && tt_entry.value <= alpha) {return tt_entry.value;}
            else if (tt_entry.flag == TT_BETA && tt_entry.value >= beta) {return tt_entry.value;}   
            tt_move = (tt_entry.move);
        }
    }   

    //IID reduction technique/IIR
    //If there was no value in transposition table and we meet other requirements we can reduce depth
    if (depth >= 4 && !is_tt) {depth--;}

    //static evaluation of position
    int eval;
    //*add use of transposition table eval to enhance static eval
    if (is_tt) {
        eval = tt_entry.value;
    }
    else {
        eval = evaluation(board);
    }

    //reverse futility pruning/static null move pruning
    //if we are sufficiently above beta we can prune and meet requirements
    if (!in_check && !is_pv && depth < 6
        && eval - REVERSE_FUTILITY_MARGIN * depth >= beta 
        && eval >= beta && !(beta >= MATE_V-MAX_DEPTH || beta <= -MATE_V+MAX_DEPTH)) {
        return eval - REVERSE_FUTILITY_MARGIN * depth;
    }

    //null move pruning
    //if we don't play a move but still have a good position we can prune
    // *add methods to stop zugzwang 
    int R;
    if (null && !in_check && depth >= 3 && !is_pv) {
        R = 3 + (depth/6);
        R = min(depth-1, R);
        board.push_null();
        value = -negamax(board, -beta, -beta+1, depth-R-1, ply+1, NodeInfo(false, Moves(), eval, false));
        board.pop_null();
        //check for beta cutoff
        if (value >= beta) {
            return value;
        }
    } 

    //razoring
    //if we are sufficiently below alpha then we can prune
    // if (depth <= 5 && !is_pv && !in_check && eval + RAZORING_MARGIN*depth <= alpha) {
    //     value = quiesce(board, alpha, beta);
    //     if (value <= alpha) {return value;}
    // }

    //move generation
    vector<Moves> moves = board.generate_psuedolegal_moves();
    score_moves(board, moves, tt_move, ply, prev_move);
    sort(moves.begin(), moves.end(), move_sort());

    //search moves
    for (Moves &move : moves) {

        board.push(move); // play move
        //check move legality; if it isn't legal skip the move
        if(board.in_check(board.turn^1)) {
            board.pop();
            continue;
        }
        bool next_check = board.in_check(board.turn); //the move gave a check
        //check if a move is tactical or related to checks
        not_tactical = move.captured == NO_PIECE && !in_check && move.promoted == PAWN_I && !next_check;

        //if legal increase legal move count
        legal_moves++;  

        // // late move pruning (LMP)
        // // we can prune moves that are late in the node cause they are probably not as good
        if (depth <= 5 && !is_pv && not_tactical && legal_moves > LMP_TABLE[depth]) {
            board.pop();
            continue;
        }

        // // futility pruning *Needs more testing
        // //if we are far enough below alpha we can prune
        // if (depth <= 2 && legal_moves > 1 && not_tactical && !is_pv 
        //     && eval + FUTILITY_MARGIN[depth] <= alpha) {
        //     board.pop();
        //     continue;
        // }

        R = 0; //reduction

        //use late move reduction (LMR)
        //reduces search depth for moves that are late in the node
        //also don't reduce deep pawn pushes
        if (depth >= 3 && legal_moves >= 4 && not_tactical \
            && !(move.piece == PAWN_I && ((move.to_square >= 40 && board.turn) || (move.to_square < 24 && !board.turn)))) {
            R = LMR_TABLE[depth][legal_moves] + !is_pv;

            //don't reduce as much if it is a killer or counter
            R -= (killers[0][ply] == move || killers[1][ply] == move || countermoves[board.turn^1][prev_move.from_square][prev_move.to_square] == move);
            // R -= history[board.turn^1][move.piece][move.to_square] / 1000;
            R = min(depth - 1, max(R, 0));
        }

        //zero window
        if (legal_moves > 1 || !is_pv) {  
            value = -negamax(board, -alpha-1, -alpha, depth-1-R, ply+1, NodeInfo(true, move, eval, next_check)); //search with lmr
            //research the move at full depth if late move didn't fail low
            if (R && value > alpha) {
                value = -negamax(board, -alpha-1, -alpha, depth-1, ply+1, NodeInfo(true, move, eval, next_check));
            }

            //search again using a full window
            if (value > alpha && value < beta) {
                value = -negamax(board, -beta, -alpha, depth-1, ply+1, NodeInfo(true, move, eval, next_check));
            } 
        }
        //full window
        else {
            value = -negamax(board, -beta, -alpha, depth-1, ply+1, NodeInfo(true, move, eval, next_check)); //search
        }
        board.pop(); //unmake move

        //make sure we don't have to stop
        if (nodes & 2047 && (check_limits(depth))) {return 0;}

        //alpha beta pruning
        if (value > best_v) {
            //update best move
            best_v = value; 
            best_m = move;   

            //fail high/cut node: this is so good that the opponent wouldn't let us do this
            if(value >= beta) {
                flag = TT_BETA; //fail low
                //update various heuristics (killer, countermove, history)
                if (move.captured == NO_PIECE) {
                    killers[1][ply] = killers[0][ply];
                    killers[0][ply] = move;
                    countermoves[board.turn][prev_move.from_square][prev_move.to_square] = move;
                    history[board.turn][move.piece][move.to_square] += depth*depth;
                    //age the histories
                    if (history[board.turn][move.piece][move.to_square] > MAX_HISTORY_V) {
                        for (int c = WHITE; c <= BLACK; c++) {
                            for (int p = PAWN_I; p <= KING_I; p++) {
                                for (int s = 0; s < 64; s++) {
                                    history[c][p][s] /= 2;
                                }
                            }  
                        }
                    }
                }
                break; //cutoff
            }

            //this is good so it is our new minimium
            if (value > alpha) {
                alpha = value;
                flag = TT_EXACT; //we were in alpha-beta bounds
                update_pv(move, ply);
            }
        }       
    }
    
    //check for stalemate and checkmate
    if (legal_moves == 0) {
        //-MATE because current player does not want to get checkmated + ply (so we look for fastest mate)
        if(in_check) {return -MATE_V + ply;}
        //STALEMATE
        else {return DRAW_V;}
    }

    //store transposition table values
    //make room for new tt entry
    if (tt_table.size() >= TT_MAX_SIZE) {
        tt_table.erase(tt_table.begin());
    }
    //store tt entry
    tt_table[board.zobrist_hash] = TTEntry(best_v, (best_m), depth, flag);
    
    return best_v;
}
//iterative deepening loop
inline void Engine::iterative_deepening(Board& board) {
    Moves best_move; //holds best move from previous play
    start_time = steady_clock::now();   
    int alpha = -MAX_V, beta = MAX_V;
    int value = 0;
    int upperbound, lowerbound;
    int delta_v = 10;
    for (int depth = 1; depth <= search_depth; depth++) {       
        //check engine limits
        if (check_limits(depth)) {break;}
        //don't use aspiration windows at low depths
        if (depth <= 5) {
           value = negamax(board, alpha, beta, depth);
        }
        else {
            while (!stop) {
                lowerbound = max(alpha, value-delta_v);
                upperbound = min(beta, value+delta_v);
                value = negamax(board, lowerbound, upperbound, depth);
                //we failed low at root
                if (value <= lowerbound) {
                    upperbound = (upperbound+lowerbound)/2;
                    lowerbound = max(value - delta_v, alpha);
                }
                //we failed high at root
                else if (value >= upperbound) {
                    upperbound = min(beta, value+delta_v);
                }
                else {break;}
                //increase the window if we were outside alpha and beta
                delta_v += delta_v/2;
            }
        }

    
        if (check_limits(depth)) {break;}
        int elapsed_time = duration_cast<milliseconds>(steady_clock::now() - start_time).count();

        //print uci info
        cout << "info";
        if (value < -MATE_V+MAX_DEPTH+1) {cout << " score mate " << (int)((-MATE_V-value)/2);}
        else if (value > MATE_V-MAX_DEPTH-1) {cout << " score mate " << (int)((MATE_V-value+1)/2);}
        else {cout << " score cp " << value;}
        cout << " depth " << depth;
        cout << " time " << elapsed_time;
        cout << " nodes " << nodes;
        cout << " pv ";

        // loop over the moves within a PV line
        best_move = pv[0][0];
        for (int count = 0; count < pv_len[0]; count++){
            // print PV move
            cout << to_uci(pv[0][count]) << "  ";
        }     
        cout << endl;  
    }
    //bestmove for uci
    if (stop == false) {cout << "bestmove " << to_uci(best_move) << endl;}
    else {cout << "bestmove " << to_uci(best_move) << endl;}
}

void Engine::search(Board& board, EngineLimits &limits) {
    //reset important values
    stop = false;
    nodes = 0;
    reset();
    //infinite
    search_depth = MAX_DEPTH;
    time_for_move = MAX_TIME;
    if (limits.depth != 0) {
        search_depth = limits.depth;
    }
    else if (limits.move_time != 0) {
        time_for_move = limits.move_time - OVERHEAD_TIME;
    }
    else if (limits.co_time[board.turn] != 0) {
        //there are no moves until next time control (ie. sudden death): use TIME_DIVIDER
        int divider;
        limits.moves_to_go == 0 ? divider = TIME_DIVIDER : divider = limits.moves_to_go;

        time_for_move = limits.co_time[board.turn]/divider - OVERHEAD_TIME;
        //add on increment if we can
        if (limits.co_time[board.turn] > limits.co_inc[board.turn]) {time_for_move += limits.co_inc[board.turn];}

    }
    time_for_move = max(time_for_move, 5);


    iterative_deepening(board);
}
