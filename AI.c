#include "AI.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

//slow
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
//SLOW
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
	printf("\nLooking %d deep.\n", depth);
	if (depth > MAX_DEPTH) {
		depth = MAX_DEPTH;
	}

	clock_t start, end;
	start = clock();
	uint64_t nodes = 0;


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
			if (givenPiece.type == Knight) {

				//Optimization, instead of copying over, have checks on the stuff later to see if we're moving a pawn
				//and rely more on the current index we store than this.
				MoveCount[curDepth] = AllLegalMoves[givenPiece.x][givenPiece.y].moveC;
				uint cLegalMove = 0;
				uint toX = 0;
				uint toY = 0;
				for (int cM = 0; cM < AllLegalMoves[givenPiece.x][givenPiece.y].moveC; cM++) {
					toX = AllLegalMoves[givenPiece.x][givenPiece.y].toX[cM];
					toY = AllLegalMoves[givenPiece.x][givenPiece.y].toY[cM];
					Moves[curDepth][cLegalMove][X] = toX;
					Moves[curDepth][cLegalMove][Y] = toY;
					MoveCount[curDepth] -= games[curDepth].board.p[toX][toY][Owner] == curPlayer;
					//cM += games[curDepth].board.p[toX][toY][Owner] != curPlayer;
					cLegalMove += games[curDepth].board.p[toX][toY][Owner] != curPlayer;
				}

			}
			/*else if (givenPiece.type == Pawn) {
				//legal pawn moves are forward 2 if on base, forward 1 otherwise, capture forward diagonally 

				//one ahead
				int8_t tempX = givenPiece.x;
				int8_t tempY = givenPiece.y + forward;
				if (games[curDepth].board.p[tempX][tempY][Type] == Empty) {
					Moves[curDepth][MoveCount[curDepth]][0] = tempX;
					Moves[curDepth][MoveCount[curDepth]][1] = tempY;
					MoveCount[curDepth]++;
				}

				//if we're on our start and can try two ahead
				tempY += forward;
				if (givenPiece.y == pawnBaseY) {
					//two ahead
					if (games[curDepth].board.p[tempX][tempY][Type] == Empty) {
						Moves[curDepth][MoveCount[curDepth]][0] = tempX;
						Moves[curDepth][MoveCount[curDepth]][1] = tempY;
						MoveCount[curDepth]++;
					}
				}

				//checking for captures
				tempY -= forward;
				tempX++;
				if (tempX < BoardDim && tempY < BoardDim && tempY > 0) {
					if (games[curDepth].board.p[tempX][tempY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][0] = tempX;
						Moves[curDepth][MoveCount[curDepth]][1] = tempY;
						MoveCount[curDepth]++;
					}
				}

				tempX -= 2;
				if (tempX > 0 && tempY < BoardDim && tempY > 0) {
					if (games[curDepth].board.p[tempX][tempY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][0] = tempX;
						Moves[curDepth][MoveCount[curDepth]][1] = tempY;
						MoveCount[curDepth]++;
					}
				}

			}*/
			else if (givenPiece.type == Rook) {
			LinearMovementCalc:;
				int8_t baseY = givenPiece.y;
				//go right adding moves until we hit a piece, if enemy piece save it too
				int8_t cX = givenPiece.x + 1;
				while (cX < BoardDim) {
					if (games[curDepth].board.p[cX][baseY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][baseY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
					MoveCount[curDepth]++;
					cX++;
				}

				//Do the same but left
				cX = givenPiece.x - 1;
				while (cX >= 0) {
					if (games[curDepth].board.p[cX][baseY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][baseY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
					MoveCount[curDepth]++;
					cX--;
				}

				//now up
				baseY = givenPiece.y + 1;
				cX = givenPiece.x;
				while (baseY < BoardDim) {
					if (games[curDepth].board.p[cX][baseY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][baseY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
					MoveCount[curDepth]++;
					baseY++;
				}

				//now down
				baseY = givenPiece.y - 1;
				cX = givenPiece.x;
				while (baseY >= 0) {
					if (games[curDepth].board.p[cX][baseY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][baseY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = baseY;
					MoveCount[curDepth]++;
					baseY--;
				}

			}
			else if (givenPiece.type == Bishop) {
			DiagonalMovementCalc:;
				//go up and right adding moves until we hit a piece, if enemy piece save it too
				int8_t cY = givenPiece.y + 1;
				int8_t cX = givenPiece.x + 1;
				while (cX < BoardDim && cY < BoardDim) {
					if (games[curDepth].board.p[cX][cY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][cY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = cY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = cY;
					MoveCount[curDepth]++;
					cX++;
					cY++;
				}

				//go down and right adding moves until we hit a piece, if enemy piece save it too
				cY = givenPiece.y - 1;
				cX = givenPiece.x + 1;
				while (cX < BoardDim && cY >= 0) {
					if (games[curDepth].board.p[cX][cY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][cY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = cY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = cY;
					MoveCount[curDepth]++;
					cX++;
					cY--;
				}

				//go up and left adding moves until we hit a piece, if enemy piece save it too
				cY = givenPiece.y + 1;
				cX = givenPiece.x - 1;
				while (cX >= 0 && cY < BoardDim) {
					if (games[curDepth].board.p[cX][cY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][cY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = cY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = cY;
					MoveCount[curDepth]++;
					cX--;
					cY++;
				}

				//go down and left adding moves until we hit a piece, if enemy piece save it too
				cY = givenPiece.y - 1;
				cX = givenPiece.x - 1;
				while (cX >= 0 && cY >= 0) {
					if (games[curDepth].board.p[cX][cY][Owner] == curPlayer) {
						break;
					}
					else if (games[curDepth].board.p[cX][cY][Owner] == !curPlayer) {
						Moves[curDepth][MoveCount[curDepth]][X] = cX;
						Moves[curDepth][MoveCount[curDepth]][Y] = cY;
						MoveCount[curDepth]++;
						break;
					}
					Moves[curDepth][MoveCount[curDepth]][X] = cX;
					Moves[curDepth][MoveCount[curDepth]][Y] = cY;
					MoveCount[curDepth]++;
					cX--;
					cY--;
				}
				goto DoneMoveGen;
			}
			else if (givenPiece.type == Queen) {
				goto LinearMovementCalc;
			}
			else if (givenPiece.type == King) {
				//flags
				uint right = givenPiece.x < (BoardDim - 2);
				uint down = givenPiece.y > 0;
				uint left = givenPiece.x > 0;
				uint up = givenPiece.y < (BoardDim - 2);

				//right
				if (games[curDepth].board.p[givenPiece.x + 1][givenPiece.y][Owner] != curPlayer && right) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x + 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y;
					MoveCount[curDepth]++;
				}

				//down right one
				if (games[curDepth].board.p[givenPiece.x + 1][givenPiece.y - 1][Owner] != curPlayer && (right & down)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x + 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y - 1;
					MoveCount[curDepth]++;
				}

				//down
				if (games[curDepth].board.p[givenPiece.x][givenPiece.y - 1][Owner] != curPlayer && (down)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y - 1;
					MoveCount[curDepth]++;
				}

				//down left one
				if (games[curDepth].board.p[givenPiece.x - 1][givenPiece.y - 1][Owner] != curPlayer && (left & down)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x - 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y - 1;
					MoveCount[curDepth]++;
				}

				//left
				if (games[curDepth].board.p[givenPiece.x - 1][givenPiece.y][Owner] != curPlayer && left) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x - 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y;
					MoveCount[curDepth]++;
				}

				//left up one
				if (games[curDepth].board.p[givenPiece.x - 1][givenPiece.y + 1][Owner] != curPlayer && (left & up)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x - 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y + 1;
					MoveCount[curDepth]++;
				}

				//up one
				if (games[curDepth].board.p[givenPiece.x][givenPiece.y + 1][Owner] != curPlayer && (up)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y + 1;
					MoveCount[curDepth]++;
				}

				//up right one
				if (games[curDepth].board.p[givenPiece.x + 1][givenPiece.y + 1][Owner] != curPlayer && (up & right)) {
					Moves[curDepth][MoveCount[curDepth]][X] = givenPiece.x + 1;
					Moves[curDepth][MoveCount[curDepth]][Y] = givenPiece.y + 1;
					MoveCount[curDepth]++;
				}
				
			}
			if (givenPiece.type == Queen) {
				goto DiagonalMovementCalc;
			}
		}

		DoneMoveGen:;
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

		if (curDepth != depth) {
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
		}
		

		//Update the new board with the given move
		movePieceTaking(&games[curDepth].board,//board
			&games[curDepth].player[curPlayer].pieces[PieceToMove[curDepth - 1]],//piece information from this simulated game
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][X],//x dest
			Moves[curDepth - 1][MoveIndex[curDepth - 1]][Y]//y dest
		);
#ifdef PRINT_BOARDS_SOLVING
		printf("\n\nDepth: %d\n", curDepth);
		printBoard(games[curDepth].board);
#endif

		
	IllegalMoveCheck:;
		//check if this is an illegal move
		Piece* curEnP = NULL;
		/*//Special king-move specific- you cannot move a king too close to the enemy king so it can have some 'default' language to try and catch this rather
		//than trying to figure out all enemy king moves that could put us in check.... because that's also impossible lol
		if (givenPiece.type == King) {
			int8_t xdist = (int8_t)games[curDepth].player[!curPlayer].pieces[0].x - (int8_t)givenPiece.x;
			int8_t ydist = (int8_t)games[curDepth].player[!curPlayer].pieces[0].y - (int8_t)givenPiece.y;
			if (xdist <= 1 && ydist <= 1) {
				goto SelectNext;
			}
		}*/
		const uint ourX = games[curDepth].player[curPlayer].pieces[0].x;
		const uint ourY = games[curDepth].player[curPlayer].pieces[0].y;
		for (int cP = 0; cP < games[curDepth].player[!curPlayer].pieceC; cP++) {
			curEnP = &games[curDepth].player[!curPlayer].pieces[cP];

			if (curEnP->type == Rook) {//linear boi
			LinearKingTakeCheck:;//for queenie
				if (curEnP->x == ourX) {//check to see if on same x first off
					//if so, see if it can slip down on us
					if (curEnP->y > ourY) {
						for (uint cY = curEnP - 1; cY > ourY; cY--) {
							if (games[curDepth].board.p[curEnP->x][cY][Type]) {
								continue;//nah, this rook hit something on its way. Doesn't matter what, it just means the rook couldn't take the king at this go
							}
						}
						#ifdef PRINT_BOARDS_SOLVING
						printf("^ Illegal move, ignoring ^\n\n\n");
						#endif
						goto HadIllegalMove;//There was no obstruction
					}
					else {//or up on us. /Technically/, if the Ys are on top of each other, this likely wont work properly, but at that point the pieces should be merged...
						for (uint cY = curEnP - 1; cY < ourY; cY++) {
							if (games[curDepth].board.p[curEnP->x][cY][Type]) {
								continue;//nah, this rook hit something on its way. Doesn't matter what, it just means the rook couldn't take the king at this go
							}
						}
						#ifdef PRINT_BOARDS_SOLVING
						printf("^ Illegal move, ignoring ^\n\n\n");
						#endif
						goto HadIllegalMove;//There was no obstruction
					}
				}
				else if (curEnP->y == ourY) {//If they're on the same y, they could potentially nick us
				   //if so, see if it can slip in on us from the right
					if (curEnP->x > ourX) {
						for (uint cX = curEnP - 1; cX > ourX; cX--) {
							if (games[curDepth].board.p[cX][curEnP->y][Type]) {
								continue;//nah, this rook hit something on its way. Doesn't matter what, it just means the rook couldn't take the king at this go
							}
						}
						#ifdef PRINT_BOARDS_SOLVING
						printf("^ Illegal move, ignoring ^\n\n\n");
						#endif
						goto HadIllegalMove;//There was no obstruction
					}
					else {
						for (uint cX = curEnP - 1; cX < ourX; cX++) {
							if (games[curDepth].board.p[cX][curEnP->y][Type]) {
								continue;//nah, this rook hit something on its way. Doesn't matter what, it just means the rook couldn't take the king at this go
							}
						}
						#ifdef PRINT_BOARDS_SOLVING
						printf("^ Illegal move, ignoring ^\n\n\n");
						#endif
						goto HadIllegalMove;//There was no obstruction
					}

				}

			}
		}







		

		//Now, if we're at max depth
		if (curDepth == depth) {
			//Back up one, score what the position was, 'score and save' only saves if the score is better/worse
		HadIllegalMove:;
			curDepth--;
			//scoreAndSave(&bestMoves[curDepth], &games[curDepth + 1], player, PieceToMove[curDepth], Moves[curDepth][MoveIndex[curDepth]][X], Moves[curDepth][MoveIndex[curDepth]][Y], curPlayer == player);
		
			//Advance the move on the current piece,
			//If no more moves are left on this piece, go to the next piece. 
			//If there are no more pieces, back up, then try again.
			//If there is no more room to back up, we are done.
		SelectNext:;
			nodes++;
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
#ifdef PRINT_NODES
	end = clock();
	double cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	double temp = (double)nodes;
	int nps = (int)round(temp / cpu_time_used);
	printf("Nodes: %d\nThat is %d nodes per second.\n", (int)nodes, (int)nps);
#endif
	Move returns = { 0 };
	return(returns);
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


//I know that the loops should be merged together for speed, but this is a oneoff and doesn't
//take much time anyhow, so writing it for human understanding is better...
void precacheKnightMoves() {
	int8_t toX = 0;
	int8_t toY = 0;
	//knight
	for (int cX = 0; cX < 8; cX++) {
		for (int cY = 0; cY < 8; cY++) {
			//up right
			toX = cX + 1;
			toY = cY + 2;
			if (toX < 8 && toY < 8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//right up
			toX = cX + 2;
			toY = cY + 1;
			if (toX < 8 && toY < 8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//right down
			toX = cX + 2;
			toY = cY - 1;
			if (toX < 8 && toY >= 0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//down right
			toX = cX + 1;
			toY = cY - 2;
			if (toX < 8 && toY >= 0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//down left
			toX = cX - 1;
			toY = cY - 2;
			if (toX >= 0 && toY >= 0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//left down
			toX = cX - 2;
			toY = cY - 1;
			if (toX >= 0 && toY >= 0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//left up
			toX = cX - 2;
			toY = cY + 1;
			if (toX >= 0 && toY < 8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//up left
			toX = cX - 1;
			toY = cY + 2;
			if (toX >= 0 && toY < 8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}
		}
	}


	/*for (int cX = 0; cX < 8; cX++) {
		for (int cY = 0; cY < 8; cY++) {
			printf("X: %d, Y: %d. Moves: %u\n\n", cX, cY, AllLegalMoves[cX][cY].moveC);
			for (int cM = 0; cM < AllLegalMoves[cX][cY].moveC; cM++) {
				printf("To %u,%u\n", AllLegalMoves[cX][cY].toX[cM], AllLegalMoves[cX][cY].toY[cM]);
			}
			printf("\n");
		}
	}*/
}

/*
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

}*/