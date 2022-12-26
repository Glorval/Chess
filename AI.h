#include "chessBase.h"
#define MAX_DEPTH 16
#define MAX_MOVES_PER_PIECE 27

//#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

#define PRINT_NODES
//#define PRINT_BOARDS_DELAY
//#define PRINT_BOARDS_SOLVING
//#define PRINT_NODES_FULL
//#define PRINT_ILLEGAL_MOVE_FOUND

#define printOutTheMove printf("\n\nIllegal move from depth: %d, %d\n", curDepth - 1, curDepth);printBoard(games[curDepth - 1].board);printBoard(games[curDepth].board);printf("^ Illegal move, ignoring ^\n\n\n");

#define LEGAL_MOVE 1
#define NO_MOVE 0

//enum pieces {Empty, King, Queen, Rook, Bishop, Knight, Pawn};
const static int16_t pieceValOurAgr[] = { 0, 2000, 12, 5, 3,3,1 };

const static int16_t pieceValOppAgr[] = { 0, 2000, 12, 7, 5, 4, 2 };

const static int16_t fairScore[] = { 0, 2000, 8, 5, 4, 3, 1 };
#define STALEMATE_VAL -1000
#define CHECKMATE_VAL 5000;

struct move {
	uint PieceToMove;//index in player list
	uint xTo, yTo;
	int8_t score;
};
typedef struct move Move;

struct legalMoves {
	uint8_t moveC;
	uint8_t toX[MAX_MOVES_PER_PIECE];
	uint8_t toY[MAX_MOVES_PER_PIECE];
};
typedef struct legalMoves LegalMoves;
//first two piece slots in this aren't used, and doesn't have the pawn either due to its weirder rules
static LegalMoves AllLegalMoves[BoardDim][BoardDim] = { 0 };

//fills 'all legal moves' for the queen, rook, bishop, and knight.
void precacheKnightMoves();


int16_t scorePosition(Game* game, uint player);

Move iterateLegalMoves(Game game, uint player, uint depth);
int8_t isPositionIllegal(Board board);