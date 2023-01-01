#define DEBUG


#include <stdio.h>
#include "ChessAI.h"



int main(void) {
	Game  test = createGame();


	printBitboard(test.boards[White] | test.boards[Black]);
	printGame(&test);
	printPlayer(test.players[White], White);
	printf("\n\n\n");
	nonFuncPrintPlayer(test.players[White], White);
	printf("\n\n\n");
	/*Move testmove = {0};
	testmove.moveFlags = MV_TAKING;
	testmove.newPos = sPos(0,6);
	testmove.pieceInds = MV_PACK_INDS(14, 1);
	testmove.player = Black;
	makeMove(&test, testmove);

	printBitboard(test.boards[White]);
	printf("\n");
	printBitboard(test.boards[Black]);
	printGame(&test);


	testmove.moveFlags = MV_TAKING;
	testmove.newPos = sPos(1, 6);
	testmove.pieceInds = MV_PACK_INDS(13, 2);
	testmove.player = Black;
	makeMove(&test, testmove);

	printBitboard(test.boards[White]);
	printf("\n");
	printBitboard(test.boards[Black]);
	printGame(&test);
	printPlayer(test.players[Black], Black);


	testmove.moveFlags = MV_BOO;
	testmove.pieceInds = MV_PACK_INDS(13, 2);
	testmove.player = Black;
	makeMove(&test, testmove);

	printBitboard(test.boards[White]);
	printf("\n");
	printBitboard(test.boards[Black]);
	printGame(&test);
	printPlayer(test.players[Black], Black);*/
	//return(0);
}