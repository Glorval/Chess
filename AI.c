#include "AI.h"
#include <stdlib.h>
#include <string.h>

int16_t scorePosition(Game* game, uint player) {
	int16_t score = 0;
	for (int cP = 0; cP < game->player[player].pieceC; cP++) {
		score += pieceValOurAgr[game->player[player].pieces[cP].type];
	}

	for (int cP = 0; cP < game->player[!player].pieceC; cP++) {
		score -= pieceValOppAgr[game->player[!player].pieces[cP].type];
	}

	return(score);
}


const uint Score = 0;
const uint pieceIndex = 1;
const uint xDest = 2;
const uint yDest = 3;

//goodBad is whether we're looking for best or worst, if curPlayer == us, we score for the best move we can make (Because we don't want to make a bad move ofc)
//if curPlayer != us, score for the 'worst move'
int16_t scoreAndSave(int16_t* bestMoves, Game* game, uint player, int8_t PieceToMove, uint potentialXDest, uint potentialYDest, uint goodBad) {
	//score = 0 
	int16_t posScore = scorePosition(game, player);
	if (goodBad) {
		if (posScore > bestMoves[Score]) {
			bestMoves[Score] = posScore;
			bestMoves[pieceIndex] = PieceToMove;
			bestMoves[xDest] = potentialXDest;
			bestMoves[yDest] = potentialYDest;
		}
	}
	else {
		if (posScore < bestMoves[Score]) {
			bestMoves[Score] = posScore;
			bestMoves[pieceIndex] = PieceToMove;
			bestMoves[xDest] = potentialXDest;
			bestMoves[yDest] = potentialYDest;
		}
	}
	

	return(posScore);
}




//... have it be odd depth
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

		//if no legal moves were generated, move on to the next peice
		//go to the code that handles this
		if (MoveCount[curDepth] == 0) {
			goto SelectNext;
		}

		//Now that the piece is selected, moves laid out, copy the current board into the next and apply the move.
		//games[curDepth + 1] = games[curDepth];
		memcpy(&games[curDepth + 1], &games[curDepth], sizeof(Game));

		//Dive down one, resetting all the variables for the upcoming layer as we go in. (If diving down we don't need to save what was below, as this is a new branch)
		curDepth++;

		MoveIndex[curDepth] = 0;
		for (int c = 0; c < MAX_MOVES_PER_PIECE; c++) {
			Moves[curDepth][c][X] = 0;
			Moves[curDepth][c][Y] = 0;
		}
		MoveCount[curDepth] = 0;
		PieceToMove[curDepth] = 0;
		bestMoves[curDepth][Score] = INT16_MIN;
		bestMoves[curDepth][pieceIndex] = -1;
		bestMoves[curDepth][xDest] = -1;
		bestMoves[curDepth][yDest] = -1;

		//Update the new board with the given move
		movePieceForce(&games[curDepth].board,//board
			&games[curDepth].player[curPlayer].pieces[PieceToMove[curDepth - 1]],//piece information from this simulated game
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][X],//x dest
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][Y]//y dest
		);
		printf("\n\nDepth: %d\n", curDepth);
		printBoard(games[curDepth].board);
		
		

		//Now, if we're at max depth
		if (curDepth == depth) {
			//Back up one, score what the position was, 'score and save' only saves if the score is better/worse
			curDepth--;
			scoreAndSave(&bestMoves[curDepth], &games[curDepth + 1], player, PieceToMove[curDepth], Moves[curDepth][MoveIndex[curDepth]][X], Moves[curDepth][MoveIndex[curDepth]][Y], curPlayer == player);
		
			

			//Advance the move on the current piece,
			//If no more moves are left on this piece, go to the next piece. 
			//If there are no more pieces, back up, then try again.
			//If there is no more room to back up, we are done.
		SelectNext:;
			while (1) {
				//go to the next move on the piece
				MoveIndex[curDepth]++;

				//If no more moves left on piece
				if (MoveIndex[curDepth] >= MoveCount[curDepth]) {
					//reset moves and go to the next piece to generate their moves on the next main while loop iteration
					MoveIndex[curDepth] = 0;
					MoveCount[curDepth] = 0;
					PieceToMove[curDepth]++;
					//if there are no more pieces left at this depth, back up one more.
					if (PieceToMove[curDepth] >= games[curDepth].player[!curPlayer].pieceC) {
						if (curDepth - 1 == -1) {//if we're out of depth, be done, as the only way to hit 'out of depth' at this point is to have no more pieces left to check
							goto Done;
						}
						else {
							curDepth--;
							curPlayer = !curPlayer;
						}
					}else {//Otherwise, we're done here because we found a new piece to roll with
						break;
					}
				}else {//Otherwise, we're done here because we found a move on our current piece to roll with
					break;
				}
			}
			
		}
		else {
			//otherwise, we're going down and need to swapsies
			curPlayer = !curPlayer;
		}
		
	}


Done:;

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