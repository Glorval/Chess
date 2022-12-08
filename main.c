#include <stdio.h>
#include <stdint.h>
#include "chessBase.h"

//#define DEBUG
//#define COMPAT_MODE

void flushInput(void) {
	char c = 0;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
	Game test = startGame();
	
	//printf("\33[254;244;005m Select RGB foreground color testing\n");
	//printf("\33[0m");
	//printf("\33[47;100m testing");
	//printf("\33[0m\n\n");
	printBoard(test.board);

	int player = White;
	/*while (1) {
		printf("It is %d turn.\n", player);
		printf("Enter piece to move: ");
		uint8_t x = getc(stdin);
		x = convCharToPos(x);
		uint8_t y = getc(stdin);
		y = convCharToPos(y);
		flushInput();
		printf("Enter destination: ");
		uint8_t xTo = convCharToPos(getc(stdin));
		uint8_t yTo = convCharToPos(getc(stdin));
		Piece piece = findPiecePlayer(test.player[player], x, y);
		if (piece.type == Empty) {
			continue;
		}
		movePiece(&test.board, piece, xTo, yTo);
		printBoard(test.board);

		if (player == White) {
			player = Black;
		}
		else {
			player = White;
		}
	}*/
	
	//while (1) {
	//	printf("White's turn");
	//}

}