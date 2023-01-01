#define DEBUG


#include <stdio.h>
#include "ChessAI.h"



int main(void) {
	Game  test = createGame();


	printBitboard(test.boards[White] | test.boards[Black]);
	printGame(&test);

	Move testmove = {0};
	testmove.moveFlags = MV_TAKING;
	testmove.newPos = sPos(0,6);
	testmove.pieceInds = MV_PACK_INDS(1, 1);
	testmove.player = White;
	makeMove(&test, testmove);

	printBitboard(test.boards[White]);
	printf("\n");
	printBitboard(test.boards[Black]);
	printGame(&test);
	return(0);
}