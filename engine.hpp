#include "board.hpp"
#include <cstring>
#include <unordered_map>
#ifndef ENGINE_HPP
#define ENGINE_HPP

const int MAX_DEPTH = 32;
const int MAX_TIME = 300000000;
const int TIME_DIVIDER = 45;
const int OVERHEAD_TIME = 50;
const int QUIESCE_MAX_DEPTH = 5;

//Values used in engine eval
enum VALUES {
    MAX = 20000,
    MATE_V = 10000,
    DRAW_V = 0
};

enum TTFlags {
    TT_EXACT = 0,
    TT_BETA = 1,
    TT_ALPHA = 2
};

//Use this structure to sort
struct move_sort {
    inline bool operator() (const Moves& move1, const Moves& move2) const {
        return (move1.order > move2.order);
    }
};

//ponder, mate and nodes has not been implemented yet
struct EngineLimits {
    vector<Moves> search_moves;
    bool ponder, infinite;
    int co_time[2], co_inc[2];
    int move_time;
    int moves_to_go, depth, nodes, mate;
     //Constructor
    EngineLimits() {
        ponder = false, infinite = false;
        co_time[WHITE] = 0, co_time[BLACK] = 0, co_inc[WHITE] = 0, co_inc[BLACK] = 0;
        move_time = 0;
        moves_to_go = 0, depth = 0, nodes = 0, mate = 0;
    }  
};

//Use this structure for the transposition table
struct TTEntry {
    int value, depth, flag;
    Moves move;
    TTEntry(int v, Moves m, int d, int f) {
        value = v;
        move = m;
        depth = d;
        flag = f;
    }
    TTEntry() {}
};

class Engine{
    public: 
        bool stop = false;
        int nodes = 0;
        //used for triangular pv table
        int pv_len[MAX_DEPTH+1];
        Moves pv[MAX_DEPTH+1][MAX_DEPTH+1];
        //used for killer heuristic and history heuristic
        Moves killers[2] = {};
        int history[2][64][64]; //[turn][from][to]
        //transposition table
        const int TT_MAX_SIZE = 1e7;
        unordered_map<U64, TTEntry> tt_table;
        
        //search parameters
        int search_depth;
        int time_for_move;
        time_point<steady_clock> start_time;

         //engine.cpp
        inline void update_pv(Moves &move, int ply);
        inline bool check_limits();

        int quiesce(Board &board, int alpha, int beta, int depth);
        int negamax(Board &board, int alpha, int beta, int depth, int ply, bool null);
        void iterative_deepening(Board& board);
        void search(Board& board, EngineLimits &limits);
        
        //eval.cpp
        int evaluation(Board &board);
        void move_ordering(vector<Moves> &moves);

        //sort.cpp
        void score_moves(Board &board, vector<Moves> &moves, Moves best_m, int ply);
        void score_quiesce_moves(vector<Moves> &moves);  

    
        void halt() {
            stop = true;
        }
};


#endif