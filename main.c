//#define DEBUG
//#define COMPAT_MODE



#include "chessBase.h"
#include"AI.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>


void flushInput(void) {
	char c = 0;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void) {
	Game game = startGame();


	clock_t start, end;
	double cpu_time_used;

	start = clock();
	precacheKnightMoves();
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Precache time: %f\n", cpu_time_used);

	start = clock();
	iterateLegalMoves(game, White, 6);
	
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Board Prediction time: %f\n", cpu_time_used);
	printBoard(game.board);



	/*int player = White;
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
	}*/
	
	//while (1) {
	//	printf("White's turn");
	//}

}