#pragma once
#include <stdint.h>


typedef uint8_t uint;


//Piece moving errors
#define ERR_NoPieceToMove (uint) -1
#define ERR_CannotMoveUntoSelf (uint) -2

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

#define BoardDim 8
#define Type 0
#define Owner 1
#define X 0
#define Y 1
enum owner{White, Black, Neither};
enum pieces {Empty, King, Queen, Rook, Bishop, Knight, Pawn};
const static char piecesSymbol[] = {
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

const static char whiteSquare = ' ';//176 light dither, 177 medium, 178 heavy, 219 solid, 254 solid but square
const static char blackSquare = 177;

const static char yLables[] = "12345678";
const static char xLables[] = "abcdefgh";

struct board {
	//x/y orientation
	uint p[BoardDim][BoardDim][2];
};
typedef struct board Board;

struct piece {
	//Memory optimization? Make them 4 bits
	uint x;// : 4;
	uint y;// : 4;
	uint type;
};
typedef struct piece Piece;
typedef uint32_t PieceComp;
#define compPieces(a, b) ((a).type == (b).type && (a).x == (b).x && (a).y == (b).y)
#define Square(x) ((x)*(x))

//always put the king in position zero
struct player {
	Piece pieces[16];
	uint pieceC;
};
//Always put the king in position zero
typedef struct player Player;

struct game {
	Board board;
	Player player[2];
	//Player black;
	//uint turn;
};
typedef struct game Game;

//Sets up both players and returns a fresh gamestate
Game startGame(void);
void printBoard(Board);
void printPlayer(Player player);

//For user input, converts 'abcdefgh' & '12345678' into the proper positions
uint convCharToPos(char inputChar);

//mainly used for player inputs, the AI approaches things from a piece's perspective to being with and thus the piece data doesn't need to be sought out.
Piece* findPiecePlayer(Player* player, uint xPos, uint yPos);
Piece* findPiece(Game* game, uint xPos, uint yPos);

//removes piece from player's list, takes last piece, overwrites the one to be removed, then decrements the piece count.
void removePiece(Player* player, Piece* piece);

//returns if successful or not.
uint movePiece(Game* game, Piece* piece, uint newX, uint newY);

//forces the move, good for if the checking is already done.
void movePieceForce(Board*, Piece*, uint newX, uint newY);

//new safe one
void movePieceTaking(Game* game, Piece* piece, const uint newX, const uint newY);

int validateGame(Game* game);

struct compBoard {
	uint_fast64_t lines[BoardDim];
};
typedef struct compBoard FastBoard;

int compBoards(const FastBoard* one, const FastBoard* two);
