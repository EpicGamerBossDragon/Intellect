#include "board.hpp"
#ifndef Attacks_HPP
#define Attacks_HPP

constexpr U64 RANK_4 = 0x000000FF00000000ULL, RANK_5 = 0x00000000FF000000ULL;
constexpr U64 RANK_8 = 0xFF00000000000000ULL,RANK_1 = 0x00000000000000FFULL;
constexpr U64 FILE_A = 0x8080808080808080ULL, FILE_H = 0x0101010101010101ULL;
constexpr U64 CORNERS = 0x8100000000000081ULL;
constexpr U64 EDGES = RANK_1 | RANK_8 | FILE_A | FILE_H;

namespace Attacks {
const vector<int> KNIGHT_DELTAS = {17, 15, 10, 6, -17, -15, -10, -6};
const vector<int> KING_DELTAS = {9, 8, 7, 1, -9, -8, -7, -1};
const vector<int> DIAG_DELTAS = {9, 7, -7, -9};
const vector<int> LINE_DELTAS = {8, 1, -1, -8};

extern U64 FILES_BB[8], RANKS_BB[8];

extern U64 DIAG_MASKS[64];
extern U64 LINE_MASKS[64];

extern U64 KNIGHT_ATTACKS[64], KING_ATTACKS[64], PAWN_ATTACKS[2][64];
extern U64 DIAG_ATTACKS[64][512];
extern U64 LINE_ATTACKS[64][4096];

int board_distance(int square_1, int square_2);

void init_bb_values();
void init_step_tables();
void init_diag_tables();
void init_line_tables();
void init();

U64 get_blockers(int index, int bits, U64 mask);
U64 get_edges(int square);
U64 sliding_attacks(int square, U64 occupied, vector<int> deltas);

U64 find_magic_number(int square, int bits, vector<int> deltas);
void init_magic_numbers();
//Counts bits
// inline int count_bits(U64 bitboard) {
//   int count = 0;
//   while (bitboard) {
//     count++;
//     bitboard &= bitboard - 1;
//   }
//   return count;
// }

inline int count_bits(U64 bitboard) {
  return __builtin_popcountll(bitboard);
}

// //Initilize the magics
constexpr U64 diag_magics[64] = {
  18018831494946945ULL,
  1134767471886336ULL,
  2308095375972630592ULL,
  27308574661148680ULL,
  9404081239914275072ULL,
  4683886618770800641ULL,
  216245358743802048ULL,
  9571253153235970ULL,
  27092002521253381ULL,
  1742811846410792ULL,
  8830470070272ULL,
  9235202921558442240ULL,
  1756410529322199040ULL,
  1127005325142032ULL,
  1152928124311179269ULL,
  2377913937382869017ULL,
  2314850493043704320ULL,
  4684324174200832257ULL,
  77688339246880000ULL,
  74309421802472544ULL,
  8649444578941734912ULL,
  4758897525753456914ULL,
  18168888584831744ULL,
  2463750540959940880ULL,
  9227893366251856128ULL,
  145276341141897348ULL,
  292821938185734161ULL,
  5190965918678714400ULL,
  2419567834477633538ULL,
  2308272929927873024ULL,
  18173279030480900ULL,
  612771170333492228ULL,
  4611976426970161409ULL,
  2270508834359424ULL,
  9223442681551127040ULL,
  144117389281722496ULL,
  1262208579542270208ULL,
  13988180992906560530ULL,
  4649975687305298176ULL,
  9809420809726464128ULL,
  1153222256471056394ULL,
  2901448468860109312ULL,
  40690797321924624ULL,
  4504295814726656ULL,
  299204874469892ULL,
  594838215186186752ULL,
  7210408796106130432ULL,
  144405467744964672ULL,
  145390656058359810ULL,
  1153203537948246016ULL,
  102002796048417802ULL,
  9243919728426124800ULL,
  2455024885924167748ULL,
  72066815467061280ULL,
  325424741529814049ULL,
  1175584649085829253ULL,
  18720594346444812ULL,
  584352516473913920ULL,
  1441151883179198496ULL,
  4919056693802862608ULL,
  1161950831810052608ULL,
  2464735771073020416ULL,
  54610562058947072ULL,
  580611413180448ULL
};

constexpr U64 line_magics[64] {
  11565248328107303040ULL,
  12123725398701785089ULL,
  900733188335206529ULL,
  72066458867205152ULL,
  144117387368072224ULL,
  216203568472981512ULL,
  9547631759814820096ULL,
  2341881152152807680ULL,
  140740040605696ULL,
  2316046545841029184ULL,
  72198468973629440ULL,
  81205565149155328ULL,
  146508277415412736ULL,
  703833479054336ULL,
  2450098939073003648ULL,
  576742228899270912ULL,
  36033470048378880ULL,
  72198881818984448ULL,
  1301692025185255936ULL,
  90217678106527746ULL,
  324684134750365696ULL,
  9265030608319430912ULL,
  4616194016369772546ULL,
  2199165886724ULL,
  72127964931719168ULL,
  2323857549994496000ULL,
  9323886521876609ULL,
  9024793588793472ULL,
  562992905192464ULL,
  2201179128832ULL,
  36038160048718082ULL,
  36029097666947201ULL,
  4629700967774814240ULL,
  306244980821723137ULL,
  1161084564161792ULL,
  110340390163316992ULL,
  5770254227613696ULL,
  2341876206435041792ULL,
  82199497949581313ULL,
  144120019947619460ULL,
  324329544062894112ULL,
  1152994210081882112ULL,
  13545987550281792ULL,
  17592739758089ULL,
  2306414759556218884ULL,
  144678687852232706ULL,
  9009398345171200ULL,
  2326183975409811457ULL,
  72339215047754240ULL,
  18155273440989312ULL,
  4613959945983951104ULL,
  145812974690501120ULL,
  281543763820800ULL,
  147495088967385216ULL,
  2969386217113789440ULL,
  19215066297569792ULL,
  180144054896435457ULL,
  2377928092116066437ULL,
  9277424307650174977ULL,
  4621827982418248737ULL,
  563158798583922ULL,
  5066618438763522ULL,
  144221860300195844ULL,
  281752018887682ULL
};


//Relevant bits which are important for getting attack boards
constexpr int diag_relevant_bits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6};

constexpr int line_relevant_bits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12};

//Generate the attacks for a square given the blockers in the positions
inline U64 get_diag_attacks(int square, U64 blockers) {
  blockers &= DIAG_MASKS[square];
  blockers *= diag_magics[square];
  blockers >>= 64 - diag_relevant_bits[square];
  return DIAG_ATTACKS[square][blockers];
}

inline U64 get_line_attacks(int square, U64 blockers) {
  blockers &= LINE_MASKS[square];
  blockers *= line_magics[square];
  blockers >>= 64 - line_relevant_bits[square];
  return LINE_ATTACKS[square][blockers];
}

inline U64 get_queen_attacks(int square, U64 blockers) {
  return get_diag_attacks(square, blockers) | get_line_attacks(square, blockers);
}
}



#endif