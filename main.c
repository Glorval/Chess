//#define DEBUG
//#define COMPAT_MODE



#include "chessBase.h"
#include "AI.h"

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
	printBoard(game.board);
	start = clock();
	precacheKnightMoves();
	end = clock();
	cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	printf("Precache time: %f\n", cpu_time_used);


	/*int player = White;
	while (1) {
		printf("It is %d turn.\n", player);
		printf("Enter piece to move: ");
		uint x = (uint)convCharToPos((char)getc(stdin));
		uint y = (uint)convCharToPos((char)getc(stdin));
		if (y == 8) {
			break;
		}
		flushInput();
		printf("Enter destination: ");
		uint xTo = (uint)convCharToPos((char)getc(stdin));
		uint yTo = (uint)convCharToPos((char)getc(stdin));
		flushInput();
		Piece* piece = findPiecePlayer(&game.player[player], x, y);
		if (piece == NULL) {
			continue;
		}
		movePieceTaking(&game.board, piece, xTo, yTo);
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







	uint depth = 0;
	printf("Enter Depth: \n");
	depth = (uint) (getchar() - '0');
	start = clock();
	iterateLegalMoves(game, White, depth);
	/*Piece* test = findPiecePlayer(&game.player[White], 4, 1);
	movePieceTaking(&game, test, 4, 2);

	test = findPiecePlayer(&game.player[Black], 7, 6);
	movePieceTaking(&game, test, 7, 4);
	printBoard(game.board);

	test = findPiecePlayer(&game.player[White], 3, 0);
	movePieceTaking(&game, test, 7, 4);
	printBoard(game.board);

	test = findPiecePlayer(&game.player[Black], 7, 7);
	movePieceTaking(&game, test, 7, 6);
	printBoard(game.board);
	printPlayer(game.player[White]);
	printPlayer(game.player[Black]);

	test = findPiecePlayer(&game.player[White], 7, 4);
	movePieceTaking(&game, test, 7, 6);
	printBoard(game.board);
	printPlayer(game.player[White]);
	printPlayer(game.player[Black]);*/
	
	end = clock();
	cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	printf("Board Prediction time: %f\n", cpu_time_used);
	



	
	
	//while (1) {
	//	printf("White's turn");
	//}

}