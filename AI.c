#include "AI.h"
#include <stdlib.h>
#include <string.h>

int16_t scorePosition(Game game, uint player) {
	int16_t score = 0;
	for (int cP = 0; cP < game.player[player].pieceC; cP++) {
		score += pieceValOurAgr[game.player[player].pieces[cP].type];
	}

	for (int cP = 0; cP < game.player[!player].pieceC; cP++) {
		score -= pieceValOppAgr[game.player[!player].pieces[cP].type];
	}

	return(score);
}

//... have it be even depth
Move iterateLegalMoves(Game game, uint player, uint depth) {
	if (depth > MAX_DEPTH) {
		depth = MAX_DEPTH;
	}

	//On the following, player is not needed to be stored as depth of 10 is assumed 5 moves each, thus it alternates player naturally in the memory

	//depth, then piece index to move
	int8_t PieceToMove[MAX_DEPTH] = { 0 };

	//Moves stored as depth, move list, destination X/Y. Assumed to be for the currently selected piece
	int8_t Moves[MAX_DEPTH][MAX_MOVES_PER_PIECE][2] = { 0 };

	//what move we're currently trying on this piece
	uint MoveIndex[MAX_DEPTH] = { 0 };

	//how many moves do we have at the depth
	uint MoveCount[MAX_DEPTH] = { 0 };

	//game boards
	Game games[MAX_DEPTH] = { game, 0 };
	uint curDepth = 0;
	uint curPlayer = player;

	const uint score = 0;
	const uint pieceIndex = 1;
	const uint xDest = 2;
	const uint yDest = 3;
	int16_t bestMoves[MAX_DEPTH][4] = { 0 };

	while (1) {
		int8_t forward = 0;
		uint pawnBaseY = 0;
		Piece givenPiece;
		//select piece to move and set forward for pawns
		if (curPlayer == White) {
			forward = 1;
			pawnBaseY = 1;
		}
		else {
			forward = -1;
			pawnBaseY = 6;
		}

		//if we are not backtracking upwards, the moves need to be calculated out
		if (MoveIndex[curDepth] == 0) {
			//find what piece index we're using
			givenPiece = games[curDepth].player[curPlayer].pieces[PieceToMove[curDepth]];

			//run through all the types, each one has internal code to handle move gen
			if (givenPiece.type == Pawn) {
				//legal pawn moves are forward 2 if on base, forward 1 otherwise, capture forward diagonally 

				//one ahead
				if (games[curDepth].board.p[givenPiece.x][givenPiece.y + forward][Type] == Empty) {
					Moves[curDepth][MoveCount[curDepth]][0] = givenPiece.x;
					Moves[curDepth][MoveCount[curDepth]][1] = givenPiece.y + forward;
					MoveCount[curDepth]++;
				}

				//if we're on our start and can try two ahead
				if (givenPiece.y == pawnBaseY) {
					//two ahead
					if (games[curDepth].board.p[givenPiece.x][givenPiece.y + forward + forward][Type] == Empty) {
						Moves[curDepth][MoveCount[curDepth]][0] = givenPiece.x;
						Moves[curDepth][MoveCount[curDepth]][1] = givenPiece.y + forward + forward;
						MoveCount[curDepth]++;
					}
				}

				//checking for captures
				int8_t tempX = givenPiece.x + 1;
				int8_t tempY = givenPiece.y + forward;
				if (tempX < BoardDim && tempY < BoardDim && tempY > 0) {
					if (games[curDepth].board.p[tempX][tempY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][0] = tempX;
						Moves[curDepth][MoveCount[curDepth]][1] = tempY;
						MoveCount[curDepth]++;
					}
				}

				tempX = givenPiece.x - 1;
				tempY = givenPiece.y + forward;
				if (tempX > 0 && tempY < BoardDim && tempY > 0) {
					if (games[curDepth].board.p[tempX][tempY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][0] = tempX;
						Moves[curDepth][MoveCount[curDepth]][1] = tempY;
						MoveCount[curDepth]++;
					}
				}

			}

		}



		//Now that the piece is selected, moves laid out, copy the current board into the next and apply the move.
		//games[curDepth + 1] = games[curDepth];
		memcpy(&games[curDepth + 1], &games[curDepth], sizeof(Game));

		curDepth++;
		//reset the upcoming variables since we're going down we don't need to save what was there before, we need it clean.
		MoveIndex[curDepth] = 0;
		for (int c = 0; c < MAX_MOVES_PER_PIECE; c++) {
			Moves[curDepth][c][X] = 0;
			Moves[curDepth][c][Y] = 0;
		}
		MoveCount[curDepth] = 0;
		PieceToMove[curDepth] = 0;
			

		movePieceForce(&games[curDepth].board,//board
			&games[curDepth].player[curPlayer].pieces[PieceToMove[curDepth - 1]],//piece information from this simulated game
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][X],//x dest
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][Y]//y dest
		);
		printBoard(games[curDepth].board);
		//Now, if we're at max depth
		if (curDepth == depth) {
			//score the position, compare it, then back up one
			scorePosition(games[curDepth], player);
			curDepth--;
			MoveIndex[curDepth]++;
			if (MoveIndex[curDepth] >= MoveCount[curDepth]) {
				MoveIndex[curDepth] = 0;
				MoveCount[curDepth] = 0;
				PieceToMove[curDepth]++;
				if (PieceToMove[curDepth] >= games[curDepth].player[!curPlayer].pieceC) {
					//todo?
				}
			}
		}
		else {
			//otherwise, we're going down and need to swapsies
			curPlayer = !curPlayer;
		}
		
	}
}

Game gameCopy(Game* game) {
	Game temp = *game;
	//temp.player[White].pieces = calloc(temp.player[White].pieceC, sizeof(Piece));
	for (uint c = 0; c < temp.player[White].pieceC; c++) {
		temp.player[White].pieces[c] = game->player[White].pieces[c];
	}

	//temp.player[Black].pieces = calloc(temp.player[Black].pieceC, sizeof(Piece));
	for (uint c = 0; c < temp.player[Black].pieceC; c++) {
		temp.player[Black].pieces[c] = game->player[Black].pieces[c];
	}

	return(temp);
}

Move runLegalMoves(Game* game, uint player, uint depth) {

	int8_t forward = 0;
	if (player == White) {
		forward = -1;
	}else {
		forward = 1;
	}

	Move bestMove = { 0 };

	for (int cP = 0; cP < game->player[player].pieceC; cP++) {
		Piece piece = game->player[player].pieces[cP];
		if (piece.type == Pawn) {
			//legal moves are forward 1, forward 2 if on base, diagonal capture forward

			//forward 1 move check
			if (game->board.p[piece.x][piece.y + forward][Type] == Empty) {
				//create a copy of the game to feed recursively
				Game temp = *game;
				//move the piece in the game copy
				movePieceForce(&temp, &temp.player[player].pieces[cP], piece.x, piece.y + forward);
				//recursively check again on this move and save the best
				int8_t moveScore = recurseLegalMoves(&temp, !player, depth - 1);
			}
		}
	}
}

int8_t recurseLegalMoves(Game* game, uint player, uint depth) {

}