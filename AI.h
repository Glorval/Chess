#include "chessBase.h"
#define MAX_DEPTH 30
#define MAX_MOVES_PER_PIECE 27

//enum pieces {Empty, King, Queen, Rook, Bishop, Knight, Pawn};
const static int8_t pieceValOurAgr[] = { 0, 50, 12, 5, 3,3,1 };

const static int8_t pieceValOppAgr[] = { 0, 75, 11, 6, 4, 3, 1 };

struct move {
	uint PieceToMove;//index in player list
	uint xTo, yTo;
	int8_t score;
};
typedef struct move Move;

int16_t scorePosition(Game game, uint player);

Move iterateLegalMoves(Game game, uint player, uint depth);
Move runLegalMoves(Game* game, uint player, uint depth);
int8_t recurseLegalMoves(Game* game, uint player, uint depth);