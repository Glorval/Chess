#include <stdio.h>
#include <stdint.h>
#include "chessBase.h"
#include"AI.h"

//#define DEBUG
//#define COMPAT_MODE

void flushInput(void) {
	char c = 0;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
	Game game = startGame();

	printf("%d\n", sizeof(Game));
	
	//printf("\33[254;244;005m Select RGB foreground color testing\n");
	//printf("\33[0m");
	//printf("\33[47;100m testing");
	//printf("\33[0m\n\n");
	printBoard(game.board);

	iterateLegalMoves(game, White, 2);
	int player = White;
	while (1) {
		printf("It is %d turn.\n", player);
		printf("Enter piece to move: ");
		uint x = (uint)convCharToPos((char)getc(stdin));
		uint y = (uint)convCharToPos((char)getc(stdin));
		flushInput();
		printf("Enter destination: ");
		uint xTo = (uint)convCharToPos((char)getc(stdin));
		uint yTo = (uint)convCharToPos((char)getc(stdin));
		flushInput();
		Piece* piece = findPiecePlayer(game.player[player], x, y);
		if (piece == NULL) {
			continue;
		}
		movePiece(&game.board, piece, xTo, yTo);
		printBoard(game.board);
		printf("\n");
		printPlayer(game.player[White]);
		printPlayer(game.player[Black]);
		printf("\n");
		if (player == White) {
			player = Black;
		}
		else {
			player = White;
		}
	}
	
	//while (1) {
	//	printf("White's turn");
	//}

}