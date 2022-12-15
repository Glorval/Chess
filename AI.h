#include "chessBase.h"
#define MAX_DEPTH 16
#define MAX_MOVES_PER_PIECE 27

//#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

#define PRINT_NODES
//#define PRINT_BOARDS_SOLVING
//#define PRINT_ILLEGAL_MOVE_FOUND

#define printOutTheMove printf("\n\nIllegal move from depth: %d, %d\n", curDepth - 1, curDepth);printBoard(games[curDepth - 1].board);printBoard(games[curDepth].board);printf("^ Illegal move, ignoring ^\n\n\n");

//enum pieces {Empty, King, Queen, Rook, Bishop, Knight, Pawn};
const static int8_t pieceValOurAgr[] = { 0, 50, 12, 5, 3,3,1 };

const static int8_t pieceValOppAgr[] = { 0, 75, 11, 6, 4, 3, 1 };

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