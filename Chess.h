#include <stdint.h>
#include <stdio.h>


/*typedef uint_fast8_t u8;
typedef int_fast8_t i8;
typedef uint_fast16_t u16;
typedef int_fast16_t i16;
typedef uint_fast32_t u32;
typedef int_fast32_t i32;
typedef uint_fast64_t u64;
typedef int_fast64_t i64;*/

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

//64 bit 

#define BPos(x,y) (u64)((y) << (x))//Creates a single piece board from a pair of (u8) pos with the y already offset (Mostly should be used for constants as the x/y interleaves nicely anyhow)
#define BPosC(pos) (u64)(((u64)1 << ((u64)(pos)&(u64)0x07)) << ((u64)(pos)&(u64)0x38))//Creates a single piece board from an interleaved/compressed u8 position.
#define BPosInd(x,y) ((u64)(((u64)1 << (x)) << ((y)*((u64)8))))//Creates a single piece board from a u8 x/y index, such as x = 4, y = 5.

#define sPos(x, y) (u8)((y) << 3) | (x) //creates a small position (u8) from an x/y index
#define X & (u8)0x07//Strips out just the x from a small position
#define Xind & (u8)0x07//strips out the x in index form from a small position, identical to X but here for balance
#define Y & (u8)0x38//Strips out just the y from a small position (Not the y index, but y * 8)
#define Yind >> 3//Strips out the y in index form from a small position

#define yconst0 (u64)1
#define yconst1 (u64)256
#define yconst2 (u64)65536
#define yconst3 (u64)16777216
#define yconst4 (u64)4294967296
#define yconst5 (u64)1099511627776
#define yconst6 (u64)281474976710656
#define yconst7 (u64)72057594037927936
#define yinc 8

#define x0 (u64)0
#define x1 (u64)1
#define x2 (u64)2
#define x3 (u64)3
#define x4 (u64)4
#define x5 (u64)5
#define x6 (u64)6
#define x7 (u64)7

//constant X's for constant usage
#define A (u8)0
#define B (u8)1
#define C (u8)2
#define D (u8)3
#define E (u8)4
#define F (u8)5
#define G (u8)6
#define H (u8)7

#define BoardDim 8

#define printSpace printf("   ");
//Board constants
#define setWhiteSquare printf("\33[48;2;148;148;148m")
#define setBlackSquare printf("\33[48;2;100;100;100m")
//To be used with half-character bars to center stuff
#define setWhiteFore printf("\33[38;2;148;148;148m")
#define setBlackFore printf("\33[38;2;100;100;100m")

#define setFullBlackBack printf("\33[48;2;0;0;0m")
#define setFullBlackFore printf("\33[38;2;0;0;0m")

#define setFullWhiteBack printf("\33[48;2;255;255;255m")
#define setFullWhiteFore printf("\33[38;2;255;255;255m")

const static char PlayerStrings[2][6] = {"White", "Black"};

////////////////
///Piece data///
////////////////

//symbols for printing
const static char PiecesSymbol[] = {
#ifdef DEBUG 
	'?',//Empty
#else
	' ',//empty
#endif
	246, //King ÷ 246
	241, //Queen ± 241
	254, //Rook ■ 254
	197, //Bishop ┼ 197, 206 is an alt
	'L',//knight ═
	239// pawn ∩ 239
};

#define MAX_PIECES 16//Max pieces a player can have
enum pieceTypes{None, King, Queen, Rook, Bishop, Knight, Pawn};//enum for piece type to be saved in piece structs
const static i8 ourPieceVals[] = {10, 2000, 5, 3, 4, 1};//constant adjustable values for scoring positions
const static i8 theirPieceVals[] = { 9, 1000, 6, 4, 3, 1 };//constant adjustable values for scoring positions
struct piece {
	u8 pos, type;
};
typedef struct piece Piece;


struct player {
	Piece Pieces[MAX_PIECES];
	u8  PieceC;
};
typedef struct player Player;

//Gamestate flag defines
#define GS_EN_P 0x01
#define GS_WOOO 0x02
#define GS_WOO 0x04
#define GS_BOOO 0x08
#define GS_BOO 0x10
//Colours for gamestate player array
enum colourCodes { White, Black };

struct gamestate {
	u64 boards[2];//White board/black board
	Player players[2];//White/black
	u8 MemoryFlags;
	u8 EnPassPos;//The x/y that the double move pawn is on.
	u8 EnPassInd;//The piece index of the en passant (Should always be seen from the opponent's perspective, as in the opponnent makes the double move, it stores here, then on your turn you see this set) 
	u8 CurPlayer;//Not in flags for easier access, idk
};
typedef struct gamestate Game;

#define MV_NORMAL (u8)0x01
#define MV_EN_P_LEFT (u8)0x02
#define MV_EN_P_RIGHT (u8)0x04
#define MV_WOOO (u8)0x08
#define MV_WOO (u8)0x10
#define MV_BOOO (u8)0x20
#define MV_BOO (u8)0x40
#define MV_TAKING (u8)0x80

#define MV_NOTHING_SPECIAL MV_NORMAL | MV_TAKING //There isn't anything special about the move, no castling/en passant, just either a move or capture.

#define MV_PACK_INDS(indOne, indTwo) (indOne) | ((indTwo)<<4)
#define MV_PIND_ONE &0x0F
#define MV_PIND_TWO >>4
struct move {
	u8 newPos, player;
	u8 pieceInds;//For MV_NORMAL moves, this is a singular index. For everything else, it's a packed pair
	u8 moveFlags;//Uses MV flags to indicate what kind of move it was.
};
typedef struct move Move;

//Constants for castling positions so as to not have to calculate them out
#define MV_BOO_K_POS sPos(G,7)
#define MV_BOO_R_POS sPos(F,7)

#define MV_BOOO_K_POS sPos(C,7)
#define MV_BOOO_R_POS sPos(D,7)

#define MV_WOO_K_POS sPos(G,0)
#define MV_WOO_R_POS sPos(F,0)

#define MV_WOOO_K_POS sPos(C,0)
#define MV_WOOO_R_POS sPos(D,0)

///////////////
///Functions///
///////////////


//creates fresh game
Game createGame(void);

//Prints the game out with some information at the top.
void printGame(const Game* game);

//prints out a bitboard as 1s and 0s
void printBitboard(const u64 board);

//prints out the player's pieces with the index. No newline
void printPlayer(const Player player, u8 playerColour);


//Makes a move and updates the given gamestate. Does not check legality
void makeMove(Game* game, const Move move);