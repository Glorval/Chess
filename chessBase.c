#include "chessBase.h"
#include <stdlib.h>
#include <stdio.h>



Game startGame(void) {
	Game returns = { 0 };

	for (uint cX = 0; cX < BoardDim; cX++) {
		for (uint cY = 0; cY < BoardDim; cY++) {
			returns.board.p[cX][cY][Owner] = Neither;
			returns.board.p[cX][cY][Type] = Empty;
		}
	}

	//white setup
	returns.player[White].pieceC = 16;
	//returns.player[White].pieces = calloc(16, sizeof(Piece));
	for (uint cPawn = 0; cPawn < 8; cPawn++) {
		returns.player[White].pieces[cPawn].type = Pawn;
		returns.player[White].pieces[cPawn].x = cPawn;
		returns.player[White].pieces[cPawn].y = 1;

		returns.board.p[cPawn][1][Type] = Pawn;
		returns.board.p[cPawn][1][Owner] = White;
	}
	returns.player[White].pieces[8].type = Rook;
	returns.player[White].pieces[8].x = 0;
	returns.board.p[0][0][Type] = Rook;
	returns.board.p[0][0][Owner] = White;

	returns.player[White].pieces[9].type = Knight;
	returns.player[White].pieces[9].x = 1;
	returns.board.p[1][0][Type] = Knight;
	returns.board.p[1][0][Owner] = White;

	returns.player[White].pieces[10].type = Bishop;
	returns.player[White].pieces[10].x = 2;
	returns.board.p[2][0][Type] = Bishop;
	returns.board.p[2][0][Owner] = White;

	returns.player[White].pieces[11].type = Queen;
	returns.player[White].pieces[11].x = 3;
	returns.board.p[3][0][Type] = Queen;
	returns.board.p[3][0][Owner] = White;

	returns.player[White].pieces[12].type = King;
	returns.player[White].pieces[12].x = 4;
	returns.board.p[4][0][Type] = King;
	returns.board.p[4][0][Owner] = White;

	returns.player[White].pieces[13].type = Bishop;
	returns.player[White].pieces[13].x = 5;
	returns.board.p[5][0][Type] = Bishop;
	returns.board.p[5][0][Owner] = White;

	returns.player[White].pieces[14].type = Knight;
	returns.player[White].pieces[14].x = 6;
	returns.board.p[6][0][Type] = Knight;
	returns.board.p[6][0][Owner] = White;

	returns.player[White].pieces[15].type = Rook;
	returns.player[White].pieces[15].x = 7;
	returns.board.p[7][0][Type] = Rook;
	returns.board.p[7][0][Owner] = White;



	//black setup
	returns.player[Black].pieceC = 16;
	//returns.player[Black].pieces = calloc(16, sizeof(Piece));
	for (uint cPawn = 0; cPawn < 8; cPawn++) {
		returns.player[Black].pieces[cPawn].type = Pawn;
		returns.player[Black].pieces[cPawn].x = cPawn;
		returns.player[Black].pieces[cPawn].y = 6;

		returns.board.p[cPawn][6][Type] = Pawn;
		returns.board.p[cPawn][6][Owner] = Black;
	}
	returns.player[Black].pieces[8].type = Rook;
	returns.player[Black].pieces[8].x = 0;
	returns.player[Black].pieces[8].y = 7;
	returns.board.p[0][7][Type] = Rook;
	returns.board.p[0][7][Owner] = Black;

	returns.player[Black].pieces[9].type = Knight;
	returns.player[Black].pieces[9].x = 1;
	returns.player[Black].pieces[9].y = 7;
	returns.board.p[1][7][Type] = Knight;
	returns.board.p[1][7][Owner] = Black;

	returns.player[Black].pieces[10].type = Bishop;
	returns.player[Black].pieces[10].x = 2;
	returns.player[Black].pieces[10].y = 7;
	returns.board.p[2][7][Type] = Bishop;
	returns.board.p[2][7][Owner] = Black;

	returns.player[Black].pieces[11].type = Queen;
	returns.player[Black].pieces[11].x = 3;
	returns.player[Black].pieces[11].y = 7;
	returns.board.p[3][7][Type] = Queen;
	returns.board.p[3][7][Owner] = Black;

	returns.player[Black].pieces[12].type = King;
	returns.player[Black].pieces[12].x = 4;
	returns.player[Black].pieces[12].y = 7;
	returns.board.p[4][7][Type] = King;
	returns.board.p[4][7][Owner] = Black;

	returns.player[Black].pieces[13].type = Bishop;
	returns.player[Black].pieces[13].x = 5;
	returns.player[Black].pieces[13].y = 7;
	returns.board.p[5][7][Type] = Bishop;
	returns.board.p[5][7][Owner] = Black;

	returns.player[Black].pieces[14].type = Knight;
	returns.player[Black].pieces[14].x = 6;
	returns.player[Black].pieces[14].y = 7;
	returns.board.p[6][7][Type] = Knight;
	returns.board.p[6][7][Owner] = Black;

	returns.player[Black].pieces[15].type = Rook;
	returns.player[Black].pieces[15].x = 7;
	returns.player[Black].pieces[15].y = 7;
	returns.board.p[7][7][Type] = Rook;
	returns.board.p[7][7][Owner] = Black;

	return(returns);
	
}


void printBoard(Board board) {
	//printing from white's perspective

	//top left corner of frame
	printf("  %c", 218);
	//top of frame
	for (int c = 0; c < 17; c++) {
		printf("%c", 196);
	}
	//top right corner of frame
	printf("%c\n",191);

	//loop through the Y axis of the board
	for (int cY = BoardDim - 1; cY >= 0 ; cY--) {
		//print the current y label and then the frame
		printf(" %c%c", yLables[cY], 179);

		//alternate between printing a half-square for white & black squares to center the board
		if (cY % 2 == 0) {
			setBlackFore;
			printf("%c", 222);
		}
		else {
			setWhiteFore;
			printf("%c", 222);
		}

		//loop through the x axis
		for (uint cX = 0; cX < BoardDim; cX++) {
			//Alternate between it being a black/white square
			if (abs(cX - cY) % 2 == 0) {
				//black square

				//checking who owns the piece
				if (board.p[cX][cY][Owner] == White) {
					//White piece, black square
					#ifdef COMPAT_MODE
					printf("\33[97;100m%c", piecesSymbol[board.p[cX][cY][Type]]);
					#else					
					setBlackSquare;
					setFullWhiteFore;
					printf("%c", piecesSymbol[board.p[cX][cY][Type]]); 
					#endif				
				}
				else {
					//black pieces, black square
					#ifdef COMPAT_MODE
					printf("\33[97;100m%c", piecesSymbol[board.p[cX][cY][Type]]);
					#else
					setBlackSquare;
					setFullBlackFore;
					printf("%c", piecesSymbol[board.p[cX][cY][Type]]);
					#endif					
				}
				//print out extra space to make board square
				#ifndef COMPAT_MODE
				if (cX < BoardDim - 1) {
					setWhiteFore;
					printf("%c", 222);
				}
				else {
					setFullBlackFore;
					printf("%c", 222);
				}
				#else
				printf(" ");
				#endif
			}
			else {
				//white square

				//checking who owns the piece
				if (board.p[cX][cY][Owner] == White) {
					//White piece, white square
					#ifdef COMPAT_MODE
					printf("\33[97;100m%c", piecesSymbol[board.p[cX][cY][Type]]);
					#else
					setWhiteSquare;//white background
					setFullWhiteFore;
					printf("%c", piecesSymbol[board.p[cX][cY][Type]]);
					#endif					
				}
				else {
					//black pieces, white square
					#ifdef COMPAT_MODE
					printf("\33[97;100m%c ", piecesSymbol[board.p[cX][cY][Type]]);
					#else
					setWhiteSquare;//white background
					setFullBlackFore;
					printf("%c", piecesSymbol[board.p[cX][cY][Type]]);
					#endif					
				}
				//give extra space to make board square
				#ifndef COMPAT_MODE
				if (cX < BoardDim - 1){
					setBlackFore;
					printf("%c", 222);
				}else {
					setFullBlackFore;
					printf("%c", 222);
				}
				#else
				printf(" ");
				#endif
			}
			//reset colours
			printf("\33[0m");
		}
		//far right section of frame
		printf("%c\n", 179);
	}
	//bottom left corner of frame
	printf("  %c", 192);

	//bottom of frame
	for (int c = 0; c < 17; c++) {
		printf("%c", 196);
	}

	//bottom right corner of frame
	printf("%c\n    ", 217);

	//bottom 'abcdefgh' labels
	for (int cLabel = 0; cLabel < BoardDim; cLabel++) {
		printf("%c ", xLables[cLabel]);
	}
	printf("\n");
}

void printPlayer(Player player) {
	for (int cP = 0; cP < player.pieceC; cP++) {
		printf("%c, %c, %c.  ", piecesSymbol[player.pieces[cP].type], xLables[player.pieces[cP].x], yLables[player.pieces[cP].y]);
	}
	printf("\n");
}

uint convCharToPos(char inputChar) {
	if (inputChar >= 'a' && inputChar <= 'h') {
		return((uint)(inputChar - 'a'));
	}
	else {
		return((uint)(inputChar - '1'));
	}
}

Piece* findPiecePlayer(Player player, uint xPos, uint yPos) {
	for (int cPiece = 0; cPiece < player.pieceC; cPiece++) {
		if (player.pieces[cPiece].x == xPos && player.pieces[cPiece].y == yPos) {
			return(&player.pieces[cPiece]);
		}
	}
	return(NULL);
}

Piece* findPiece(Game game, uint xPos, uint yPos) {
	for (int cPiece = 0; cPiece < game.player[White].pieceC; cPiece++) {
		if (game.player[White].pieces[cPiece].x == xPos && game.player[White].pieces[cPiece].y == yPos) {
			return(&game.player[White].pieces[cPiece]);
		}
	}

	for (int cPiece = 0; cPiece < game.player[Black].pieceC; cPiece++) {
		if (game.player[Black].pieces[cPiece].x == xPos && game.player[Black].pieces[cPiece].y == yPos) {
			return(&game.player[Black].pieces[cPiece]);
		}
	}

	return(NULL);
}

void removePiece(Player* player, Piece piece) {
	for (int cPiece = 0; cPiece < player->pieceC; cPiece++) {
		if (compPieces(player->pieces[cPiece], piece)) {
			player->pieces[cPiece] = player->pieces[player->pieceC - 1];
			player->pieceC--;
		}
	}
}

uint movePiece(Game* game, Piece* piece, uint newX, uint newY) {
	if (game->board.p[piece->x][piece->y][Type] != piece->type) {
		return(ERR_NoPieceToMove);
	}
	uint ourOwner = game->board.p[piece->x][piece->y][Owner];
	//TODO, implement a check to see if moving this piece will check ourselves.

	if (game->board.p[newX][newY][Type] != Empty){
		if (game->board.p[newX][newY][Owner] == ourOwner) {
			return(ERR_CannotMoveUntoSelf);
		}
		else {
			if (ourOwner == White) {
				removePiece(&game->player[Black], *findPiecePlayer(game->player[Black], newX, newY));
			}
		}
	}

	movePieceForce(&game->board, piece, newX, newY);
	return(0);
}


void movePieceForce(Board* board, Piece* piece, uint newX, uint newY) {
	board->p[newX][newY][Type] = piece->type;
	board->p[newX][newY][Owner] = board->p[piece->x][piece->y][Owner];//steal the owner out of the past
	board->p[piece->x][piece->y][Type] = Empty;
	board->p[piece->x][piece->y][Owner] = Neither;

	piece->x = newX;
	piece->y = newY;
}