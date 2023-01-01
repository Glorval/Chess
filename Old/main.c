//#define DEBUG
//#define COMPAT_MODE

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004

#include <windows.h>

#include "chessBase.h"
#include "AI.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>



void flushInput(void) {
	int c = 0;
	while ((c = getchar()) != '\n' && c != EOF) {}
}


int main(void) {
// enable ANSI sequences for windows 10:
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode(console, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console, consoleMode);

	printf("size %u\n", sizeof(Game));

	Game game = startGame();


	clock_t start, end;
	double cpu_time_used;
	printBoard(game.board);
	start = clock();
	precacheKnightMoves();
	end = clock();
	cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	printf("  Precache time: %f\n", cpu_time_used);
	uint depth = 0;
	



	/*printf("Enter Depth: \n");
	depth = (uint) (getchar() - '0');
	flushInput();
	start = clock();
	Move mv = iterateLegalMoves(game, White, depth);
	printf("Move found: Piece %c, to %u,%u, rating was %d.\n", piecesSymbol[game.player[White].pieces[mv.PieceToMove].type], mv.xTo, mv.yTo, mv.score);
	
	end = clock();
	cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	printf("Board Prediction time: %f\n", cpu_time_used);*/
	
	
	printf("  Enter Depth: \n  ");
	depth = (uint)(getchar() - '0');
	printf("\n\n");
	flushInput();
	
	while (1) {
		printBoard(game.board);
		printf("\n  Enter piece to move: ");
		uint x = (uint)convCharToPos((char)getc(stdin));
		uint y = (uint)convCharToPos((char)getc(stdin));
		if (y == 8) {
			break;
		}
		flushInput();
		printf("  Enter destination: ");
		uint xTo = (uint)convCharToPos((char)getc(stdin));
		uint yTo = (uint)convCharToPos((char)getc(stdin));
		flushInput();
		Piece* piece = findPiecePlayer(&game.player[White], x, y);
		if (piece == NULL) {
			printf("Piece does not exist.");
			continue;
			//exit(1);
		}
		movePieceTaking(&game, piece, xTo, yTo);
		printBoard(game.board);
		printf("\n\n\n");
		//printPlayer(game.player[White]);
		//printPlayer(game.player[Black]);

		start = clock();
		Move mv = iterateLegalMoves(game, Black, depth);
		end = clock();
		cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
		printf("  Board Prediction time: %f\n", cpu_time_used);
		printf("  Move found: Piece %c, to %u,%u, rating was %d.\n", piecesSymbol[game.player[Black].pieces[mv.PieceToMove].type], mv.xTo, mv.yTo, mv.score);

		movePieceTaking(&game, &game.player[Black].pieces[mv.PieceToMove], mv.xTo, mv.yTo);
	}
	

	depth = (uint)(getchar() - '0');
	return(depth);
}