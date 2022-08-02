#include "board.hpp"
#include <cstring>
#ifndef ENGINE_HPP
#define ENGINE_HPP

const int MAX_DEPTH = 32;
const int MAX_TIME = 300000000;
const int TIME_DIVIDER = 50;
const int OVERHEAD_TIME = 75;
const int QUIESCE_MAX_DEPTH = 5;

//Values used in engine eval
enum VALUES {
    MAX = 20000,
    MATE_V = 10000,
    DRAW_V = 0
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
        ponder, infinite = false;
        co_time[WHITE], co_time[BLACK], co_inc[WHITE], co_inc[BLACK] = 0;
        move_time = 0;
        moves_to_go, depth, nodes, mate = 0;
    }  
};


class Engine{
    public: 
       
        
        bool stop = false;
        //used for triangular pv table
        int pv_len[MAX_DEPTH];
        Moves pv[MAX_DEPTH][MAX_DEPTH];
        //used for killer heuristic
        Moves killers[2] = {Moves()};
        
        //search parameters
        int search_depth;
        int time_for_move;
        time_point<steady_clock> start_time;

         //engine.cpp
        inline void update_pv(Moves &move, int ply);

        int quiesce(Board &board, int alpha, int beta, int depth);
        int negamax(Board &board, int alpha, int beta, int depth, int ply);
        void iterative_deepening(Board& board);
        void search(Board& board, EngineLimits &limits);
        
        //eval.cpp
        int evaluation(Board &board);
        void move_ordering(vector<Moves> &moves);

        //sort.cpp
        void score_moves(vector<Moves> &moves);
        void score_quiesce_moves(vector<Moves> &moves);    
    
        void halt() {
            // loop over the moves within a PV line
            for (int count = 0; count < pv_len[0]; count++){
                // print PV move
                cout << to_uci(pv[0][count]) << "  ";
            }
        }
};


#endif