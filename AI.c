#include "AI.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

//slow
int16_t scorePosition(Game* game, uint player) {
	int16_t score = 0;
	for (uint cP = 0; cP < game->player[player].pieceC; cP++) {
		score += pieceValOurAgr[game->player[player].pieces[cP].type];
	}

	for (uint cP = 0; cP < game->player[!player].pieceC; cP++) {
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
void scoreAndSave(int16_t* bestMoves, Game* game, uint player, uint PieceToMove, uint potentialXDest, uint potentialYDest, uint goodBad) {
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
		if (posScore * -1 > bestMoves[Score]) {
			bestMoves[Score] = posScore;
			bestMoves[pieceIndex] = PieceToMove;
			bestMoves[xDest] = potentialXDest;
			bestMoves[yDest] = potentialYDest;
		}
	}
	

	//return(posScore);
}





//... have it be odd depth
Move iterateLegalMoves(Game game, uint player, uint depth) {
	printf("\n  Looking %d deep.\n", depth);
	if (depth > MAX_DEPTH) {
		depth = MAX_DEPTH;
	}

	clock_t start, end;
	start = clock();
	uint64_t nodes = 0;


	//On the following, player is not needed to be stored as depth of 10 is assumed 5 moves each, thus it alternates player naturally in the memory

	//depth, then piece index to move
	uint PieceToMove[MAX_DEPTH] = { 0 };

	//Moves stored as depth, move list, destination X/Y. Assumed to be for the currently selected piece
	uint Moves[MAX_DEPTH][MAX_MOVES_PER_PIECE][2] = { 0 };

	//what move we're currently trying on this piece
	uint MoveIndex[MAX_DEPTH] = { 0 };

	//how many moves do we have at the depth
	uint MoveCount[MAX_DEPTH] = { 0 };

	//Have we successfully made a legal move at this depth? Used to determine if a position is a checkmate.
	//0 = None, 1 = found. If no legal move is found and it's time to back up, there needs to be a check  run to see if
	//the starting position is in check or not. If it is, that means it's checkmate (in check, no legal moves)
	//If not, stalemate (worth -1000 [STALEMATE_VAL], but is not as bad as loss)
	uint FoundLegalMove[MAX_DEPTH] = { 0 };

	//The x/y of a pawn that just moved two foward. 
	//How this works is that on depth A, if a pawn moves two forward, depth A+1 will have the spot it moved to written in to check against.
	//Setting it to 0 should be safe because that's not a place that can physically be en passant'd on
	uint EnPassantKnowledge[MAX_DEPTH + 1][2] = { 0 };
	uint EnPassantMoveInd[MAX_DEPTH] = {MAX_MOVES_PER_PIECE + 1};//The index of the current piece move that can trigger an en passant, as in, a pawn just moved two forward
	uint WeCanEnPassantInd[MAX_DEPTH] = { MAX_MOVES_PER_PIECE + 1 };//The move index we can actually en passant on, needed to remove the taken pawn properly.

	//game boards
	Game games[MAX_DEPTH] = { game, 0 };
	uint curDepth = 0;
	uint curPlayer = player;

	
	int16_t bestMoves[MAX_DEPTH][4] = { INT16_MIN, -1,-1,-1 };

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
				int8_t tempX = givenPiece.x;
				int8_t tempY = givenPiece.y + forward;
				if (games[curDepth].board.p[tempX][tempY][Type] == Empty) {
					Moves[curDepth][MoveCount[curDepth]][0] = tempX;
					Moves[curDepth][MoveCount[curDepth]][1] = tempY;
					MoveCount[curDepth]++;

					//if we're on our start and can try two ahead
					if (givenPiece.y == pawnBaseY) {
						tempY += forward;
						//two ahead
						if (games[curDepth].board.p[tempX][tempY][Type] == Empty) {
							Moves[curDepth][MoveCount[curDepth]][0] = tempX;
							Moves[curDepth][MoveCount[curDepth]][1] = tempY;
							EnPassantMoveInd[curDepth] = MoveCount[curDepth];//Save this  two ahead move's index such that later we can set the en passant stuff

							MoveCount[curDepth]++;
						}
					}
				}

				//checking for captures
				tempY = givenPiece.y + forward;
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

				//now check to see if we can en passant, 
				//it needs an enemy pawn to move two forward and end up next to us for it to work.
				//first see if we're on the right Y area
				if(givenPiece.y == EnPassantKnowledge[curDepth][Y]){
					//now see if we are one left of it
					if(givenPiece.x + 1 == EnPassantKnowledge[curDepth][X]){
						Moves[curDepth][MoveCount[curDepth]][0] = givenPiece.x + 1;
						Moves[curDepth][MoveCount[curDepth]][1] = givenPiece.y + forward;
						WeCanEnPassantInd[curDepth] = MoveCount[curDepth];
						MoveCount[curDepth]++;						
					}

					//now see if we are one right of it
					if (givenPiece.x - 1 == EnPassantKnowledge[curDepth][X]) {
						Moves[curDepth][MoveCount[curDepth]][0] = givenPiece.x - 1;
						Moves[curDepth][MoveCount[curDepth]][1] = givenPiece.y + forward;
						WeCanEnPassantInd[curDepth] = MoveCount[curDepth];
						MoveCount[curDepth]++;
					}
				}

			}
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
			else if (givenPiece.type == Knight) {
				//Optimization, instead of copying over, have checks on the stuff later to see if we're moving a knight (cause it precached)?
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
		games[curDepth + 1] = games[curDepth];

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
			EnPassantKnowledge[curDepth][X] = 0;
			EnPassantKnowledge[curDepth][Y] = 0;
			EnPassantMoveInd[curDepth] = MAX_MOVES_PER_PIECE + 1;
			WeCanEnPassantInd[curDepth] = MAX_MOVES_PER_PIECE + 1;
		}
		
		 
			
		const uint prevDepth = curDepth - 1;
		//Update the new board with the given move
		movePieceTaking(&games[curDepth],//board
			&games[curDepth].player[curPlayer].pieces[PieceToMove[prevDepth]],//piece information from this simulated game
			Moves[prevDepth][MoveIndex[prevDepth]][X],//x dest
			Moves[prevDepth][MoveIndex[prevDepth]][Y]//y dest
		);

		//Additional move logic for en passanting
		if(WeCanEnPassantInd[prevDepth] == MoveIndex[prevDepth]){
			//printf("Trying to en passant.\n");
			uint pawnToTakeX = Moves[prevDepth][MoveIndex[prevDepth]][X];
			uint pawnToTakeY= Moves[prevDepth][MoveIndex[prevDepth]][Y] - forward;
			removePiece(&games[curDepth].player[!curPlayer], findPiecePlayer(&games[curDepth].player[!curPlayer], pawnToTakeX, pawnToTakeY));
			games[curDepth].board.p[pawnToTakeX][pawnToTakeY][Type] = Empty;
			games[curDepth].board.p[pawnToTakeX][pawnToTakeY][Owner] = Neither;
		}


		#ifdef PRINT_BOARDS_SOLVING
		printf("\n\nDepth: %d. Current node: %u\n", curDepth, nodes);
		printBoard(games[curDepth].board);
		//Sleep(100);
		#endif
		#ifdef PRINT_BOARDS_DELAY
		Sleep(250);
		#endif

		IllegalMoveCheck:;
		//check if this is an illegal move
		Piece* curEnP = NULL;
		const uint ourX = games[curDepth].player[curPlayer].pieces[0].x;
		const uint ourY = games[curDepth].player[curPlayer].pieces[0].y;
		for (int cP = 0; cP < games[curDepth].player[!curPlayer].pieceC; cP++) {
			curEnP = &games[curDepth].player[!curPlayer].pieces[cP];
			//King isn't listed here because king moves pre-check if they're illegally close to the enemy king cause optimization I guess idk

			if (curEnP->type == Rook) {
				IllegalMoveCheckLinear:;
				//if on the same vertical
				if (curEnP->x == ourX) {
					if (curEnP->y > ourY) {//and above
						//check if it can come down to us and whack us
						for (uint cY = curEnP->y - 1; cY > ourY; cY--) {
							if (games[curDepth].board.p[ourX][cY][Type]) {
								goto EndOfIllegalMoveLoop;//blocker, we need to continue the outer loop
							}
						}
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
					else {
						//check if it can come up to us and whack us
						for (uint cY = curEnP->y + 1; cY < ourY; cY++) {
							if (games[curDepth].board.p[ourX][cY][Type]) {
								goto EndOfIllegalMoveLoop;//blocker, we need to continue the outer loop
							}
						}
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
				}
			
				//if on the same horizontal
				if (curEnP->y == ourY) {
					if (curEnP->x > ourX) {//and right
						//check if it can come left to us and whack us
						for (uint cX = curEnP->x - 1; cX > ourX; cX--) {
							if (games[curDepth].board.p[cX][ourY][Type]) {
								goto EndOfIllegalMoveLoop;//blocker, we need to continue the outer loop
							}
						}
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
					else {
						//check if it can come right to us and whack us
						for (uint cX = curEnP->x + 1; cX < ourX; cX++) {
							if (games[curDepth].board.p[cX][ourY][Type]) {
								goto EndOfIllegalMoveLoop;//blocker, we need to continue the outer loop
							}
						}
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
				}
			}
			else if (curEnP->type == Bishop) {
				IllegalMoveCheckDiagonal:;

				//same positive slope diagonal check
				uint xDist = ourX - curEnP->x;
				uint yDist = ourY - curEnP->y;
				if (xDist == yDist) {
					//if overflowed, it's upper right because king has smol num and bishop big
					if (xDist > BoardDim) {
						//So, check if the bishop can slide in from the upper right lowering the distance every time,
						//also starts with the distance lowered one because we shouldn't check the square the bishop is on for an interruption
						uint curX = curEnP->x - 1;
						uint curY = curEnP->y - 1;
						while(curX > ourX) {
							//If there is a blocker,
							if (games[curDepth].board.p[curX][curY][Type]) {
								//we don't need to keep checking and can just move on. This piece still no see king
								goto EndOfIllegalMoveLoop;
							}
							curX--;
							curY--;
						}
						//if no blockers are found, it is an illegal move.
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					} else {//Otherwise, it's to the bottom left as king big on both and bishop smol
						//So, check if the bishop can slide in from the lower left
						uint curX = curEnP->x + 1;
						uint curY = curEnP->y + 1;
						while (curX < ourX) {
							//If there is a blocker,
							if (games[curDepth].board.p[curX][curY][Type]) {
								//we don't need to keep checking and can just move on. This piece still no see king
								goto EndOfIllegalMoveLoop;
							}
							curX++;
							curY++;
						}
						//if no blockers are found, it is an illegal move.
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
				}
			

				xDist = ourX - curEnP->x;
				yDist = curEnP->y - ourY;
				if (xDist == yDist) {
					//if overflowed, it's lower right
					if (xDist > BoardDim) {
						uint curX = curEnP->x - 1;
						uint curY = curEnP->y + 1;
						while (curX > ourX) {
							//If there is a blocker,
							if (games[curDepth].board.p[curX][curY][Type]) {
								//we don't need to keep checking and can just move on. This piece still no see king
								goto EndOfIllegalMoveLoop;
							}
							curX--;
							curY++;
						}
						//if no blockers are found, it is an illegal move.
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}else{//enemy bishop upper left
						uint curX = curEnP->x + 1;
						uint curY = curEnP->y - 1;
						while (curX < ourX) {
							//If there is a blocker,
							if (games[curDepth].board.p[curX][curY][Type]) {
								//we don't need to keep checking and can just move on. This piece still no see king
								goto EndOfIllegalMoveLoop;
							}
							curX++;
							curY--;
						}
						//if no blockers are found, it is an illegal move.
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}

				}
				continue;
			}
			else if (curEnP->type == Pawn) {
				//If the pawn is one in front of us, we could be threatened
				if (curEnP->y - forward == ourY) {
					//if one in front and one to the side, that's a diagonal to us
					if (curEnP->x + 1 == ourX || curEnP->x - 1 == ourX) {
						//if no blockers are found, it is an illegal move.
						#ifdef PRINT_ILLEGAL_MOVE_FOUND
						printOutTheMove
						#endif
						goto HadIllegalMove;//Nothing blocked, illegal move
					}
				}
				continue;
			}
			else if (curEnP->type == Queen) {
				goto IllegalMoveCheckLinear;
			}
			if (curEnP->type == Queen) {
				goto IllegalMoveCheckDiagonal;
			}

			EndOfIllegalMoveLoop:;
		}
		
		//if we make it here, it was a legal move
		FoundLegalMove[prevDepth] = LEGAL_MOVE;

		//Now, if we just did a double mover as a pawn, as indicated by the index of the move being the same as the move we're on, set our new position as the thing
		if (EnPassantMoveInd[prevDepth] == MoveIndex[prevDepth]) {
			//printf("^ Saving an en passant possibility at depth %u to %u, %u\n", curDepth, Moves[prevDepth][MoveIndex[prevDepth]][X], Moves[prevDepth][MoveIndex[prevDepth]][Y]);
			EnPassantKnowledge[curDepth][X] = Moves[prevDepth][MoveIndex[prevDepth]][X];//the issue is 'ourx' isnt the piece dum dum
			EnPassantKnowledge[curDepth][Y] = Moves[prevDepth][MoveIndex[prevDepth]][Y];
		}

		/*if (nodes == 44) {
			printf("debug");
		}*/

		


		//Now, if we're at max depth
		if (curDepth == depth) {
			//score position as this is where we look at how we got here 
			nodes++;
			#ifdef PRINT_NODES_FULL
			printf("CurNode: %u, depth %u. Move index is on %u, piece to move is on %u.\n", nodes, curDepth, MoveIndex[curDepth], PieceToMove[curDepth]);
			#endif
			scoreAndSave(&bestMoves[prevDepth], &games[curDepth], player, PieceToMove[0], Moves[0][MoveIndex[0]][X], Moves[0][MoveIndex[0]][Y], curPlayer == player);
			
			//Back up one as we are at the max depth, which was just for looking at the produced position.	
			HadIllegalMove:;
			curDepth--;
			
			SelectNext:;
			

			//Advance the move on the current piece,
			//If no more moves are left on this piece, go to the next piece. 
			//If there are no more pieces, back up, then try again.
			//If there is no more room to back up, we are done.
			while (1) {
				//go to the next move on the piece
				MoveIndex[curDepth]++;

				//If no more moves left on piece
				if (MoveIndex[curDepth] >= MoveCount[curDepth]) {
					//reset moves and go to the next piece to generate their moves on the next main while loop iteration
					MoveIndex[curDepth] = 0;
					MoveCount[curDepth] = 0;
					PieceToMove[curDepth]++;
					WeCanEnPassantInd[curDepth] = MAX_MOVES_PER_PIECE + 1;
					EnPassantMoveInd[curDepth] = MAX_MOVES_PER_PIECE + 1;

					//if there are no more pieces left at this depth, back up one more. 
					if (PieceToMove[curDepth] >= games[curDepth].player[curPlayer].pieceC) {
						if (curDepth - 1 < 0) {//if we're out of depth, be done, as the only way to hit 'out of depth' at this point is to have no more pieces left to check
							goto Done;
						}
						else {
							//This is where we back up a depth and take the score with us. 
							/*if (curDepth - 1 < 0) {//not needed as impossible to reach while true if everything is programmed right.
								printf("bruh");
								exit(0);
							}*/

							if (curPlayer == White) {
								forward = 1;
								pawnBaseY = 1;
							} else {
								forward = -1;
								pawnBaseY = 6;
							}

							//If no moves were found during this search, figure out if it's a stalemate or check and assign accordingly.
							if (FoundLegalMove[curDepth] == NO_MOVE) {
								//check to see if we're in check before moving
								const uint ourXHere = games[curDepth].player[curPlayer].pieces[0].x;
								const uint ourYHere = games[curDepth].player[curPlayer].pieces[0].y;
								for (int cP = 0; cP < games[curDepth].player[!curPlayer].pieceC; cP++) {
									curEnP = &games[curDepth].player[!curPlayer].pieces[cP];
									//Todo, make it check against the enemy king as well for this one is different

									if (curEnP->type == Rook) {
										SecondIllegalMoveCheckLinear:;
										//if on the same vertical
										if (curEnP->x == ourXHere) {
											if (curEnP->y > ourYHere) {//and above
												//check if it can come down to us and whack us
												for (uint cY = curEnP->y - 1; cY > ourYHere; cY--) {
													if (games[curDepth].board.p[ourXHere][cY][Type]) {
														goto EndOfSecondIllegalMoveLoop;//blocker, we need to continue the outer loop
													}
													goto IsInCheck;
												}
											} else {
												//check if it can come up to us and whack us
												for (uint cY = curEnP->y + 1; cY < ourYHere; cY++) {
													if (games[curDepth].board.p[ourYHere][cY][Type]) {
														goto EndOfSecondIllegalMoveLoop;//blocker, we need to continue the outer loop
													}
													goto IsInCheck;
												}
											}
										}

										//if on the same horizontal
										if (curEnP->y == ourYHere) {
											if (curEnP->x > ourYHere) {//and right
												//check if it can come left to us and whack us
												for (uint cX = curEnP->x - 1; cX > ourYHere; cX--) {
													if (games[curDepth].board.p[cX][ourYHere][Type]) {
														goto EndOfSecondIllegalMoveLoop;//blocker, we need to continue the outer loop
													}
												}
												goto IsInCheck;
											} else {
												//check if it can come right to us and whack us
												for (uint cX = curEnP->x + 1; cX < ourYHere; cX++) {
													if (games[curDepth].board.p[cX][ourYHere][Type]) {
														goto EndOfSecondIllegalMoveLoop;//blocker, we need to continue the outer loop
													}
												}
												goto IsInCheck;
											}
										}
									} 
									else if (curEnP->type == Bishop) {
										SecondIllegalMoveCheckDiagonal:;

										//same positive slope diagonal check
										uint xDist = ourXHere - curEnP->x;
										uint yDist = ourYHere - curEnP->y;
										if (xDist == yDist) {
											//if overflowed, it's upper right because king has smol num and bishop big
											if (xDist > BoardDim) {
												//So, check if the bishop can slide in from the upper right lowering the distance every time,
												//also starts with the distance lowered one because we shouldn't check the square the bishop is on for an interruption
												uint curX = curEnP->x - 1;
												uint curY = curEnP->y - 1;
												while (curX > ourXHere) {
													//If there is a blocker,
													if (games[curDepth].board.p[curX][curY][Type]) {
														//we don't need to keep checking and can just move on. This piece still no see king
														goto EndOfSecondIllegalMoveLoop;
													}
													curX--;
													curY--;
												}
												//if no blockers are found, it is an illegal move.
												goto IsInCheck;
											} else {//Otherwise, it's to the bottom left as king big on both and bishop smol
												//So, check if the bishop can slide in from the lower left
												uint curX = curEnP->x + 1;
												uint curY = curEnP->y + 1;
												while (curX < ourXHere) {
													//If there is a blocker,
													if (games[curDepth].board.p[curX][curY][Type]) {
														//we don't need to keep checking and can just move on. This piece still no see king
														goto EndOfSecondIllegalMoveLoop;
													}
													curX++;
													curY++;
												}
												//if no blockers are found, it is an illegal move.
												goto IsInCheck;
											}
										}


										xDist = ourXHere - curEnP->x;
										yDist = curEnP->y - ourYHere;
										if (xDist == yDist) {
											//if overflowed, it's lower right
											if (xDist > BoardDim) {
												uint curX = curEnP->x - 1;
												uint curY = curEnP->y + 1;
												while (curX > ourXHere) {
													//If there is a blocker,
													if (games[curDepth].board.p[curX][curY][Type]) {
														//we don't need to keep checking and can just move on. This piece still no see king
														goto EndOfSecondIllegalMoveLoop;
													}
													curX--;
													curY++;
												}
												//if no blockers are found, it is an illegal move.
												goto IsInCheck;
											} else {//enemy bishop upper left
												uint curX = curEnP->x + 1;
												uint curY = curEnP->y - 1;
												while (curX < ourXHere) {
													//If there is a blocker,
													if (games[curDepth].board.p[curX][curY][Type]) {
														//we don't need to keep checking and can just move on. This piece still no see king
														goto EndOfSecondIllegalMoveLoop;
													}
													curX++;
													curY--;
												}
												//if no blockers are found, it is an illegal move.
												goto IsInCheck;
											}

										}
										continue;
									} 
									else if (curEnP->type == Pawn) {
										//If the pawn is one in front of us, we could be threatened
										if (curEnP->y - forward == ourYHere) {
											//if one in front and one to the side, that's a diagonal to us
											if (curEnP->x + 1 == ourXHere || curEnP->x - 1 == ourXHere) {
												//if no blockers are found, it is an illegal move.
												goto IsInCheck;
											}
										}
										continue;
									} 
									else if (curEnP->type == Queen) {
										goto SecondIllegalMoveCheckLinear;
									}
									if (curEnP->type == Queen) {
										goto SecondIllegalMoveCheckDiagonal;
									}

									EndOfSecondIllegalMoveLoop:;
								}
								
								//If we make it here, we were not in check and it's a stalemate
								
								//If the current player is us and in stalemate (no legal moves, not in check), this is meh
								if (curPlayer == player) {
									bestMoves[curDepth - 1][Score] = STALEMATE_VAL;
									bestMoves[curDepth - 1][pieceIndex] = PieceToMove[0];
									bestMoves[curDepth - 1][xDest] = Moves[0][MoveIndex[0]][X];
									bestMoves[curDepth - 1][yDest] = Moves[0][MoveIndex[0]][Y];
								} else {//Otherwise, the opponent is in stalemate, this is still meh
									bestMoves[curDepth - 1][Score] = STALEMATE_VAL;
									bestMoves[curDepth - 1][pieceIndex] = PieceToMove[0];
									bestMoves[curDepth - 1][xDest] = Moves[0][MoveIndex[0]][X];
									bestMoves[curDepth - 1][yDest] = Moves[0][MoveIndex[0]][Y];
								}
								printf("Is in stalemate.");
								goto EndOfBackup;

								IsInCheck:;
								printf("Is in checkmate\n");
								//If the current player in checkmate (check with no legal moves), this is the big bad as it's a forced checkmate.
								if (curPlayer == player) {
									bestMoves[curDepth - 1][Score] = CHECKMATE_VAL;
									bestMoves[curDepth - 1][pieceIndex] = PieceToMove[0];
									bestMoves[curDepth - 1][xDest] = Moves[0][MoveIndex[0]][X];
									bestMoves[curDepth - 1][yDest] = Moves[0][MoveIndex[0]][Y];
								} else {//Otherwise, hurrah for a checkmate move.
									bestMoves[curDepth - 1][Score] = -CHECKMATE_VAL;
									bestMoves[curDepth - 1][pieceIndex] = PieceToMove[0];
									bestMoves[curDepth - 1][xDest] = Moves[0][MoveIndex[0]][X];
									bestMoves[curDepth - 1][yDest] = Moves[0][MoveIndex[0]][Y];
								}
								
								goto EndOfBackup;

							}else{
								//printf("Current player, %u, depth %u, score %d, piece is %d, to %d, %d.\n\n",curPlayer, curDepth, bestMoves[curDepth][Score], bestMoves[curDepth][pieceIndex],bestMoves[curDepth][xDest], bestMoves[curDepth][yDest]);
								if (curPlayer != player) {//save the best score out of the nodes below
									if (bestMoves[curDepth][Score] > bestMoves[curDepth - 1][Score] || bestMoves[curDepth - 1][pieceIndex] < 0) {
										bestMoves[curDepth - 1][Score] = bestMoves[curDepth][Score];
										bestMoves[curDepth - 1][pieceIndex] = bestMoves[curDepth][pieceIndex];
										bestMoves[curDepth - 1][xDest] = bestMoves[curDepth][xDest];
										bestMoves[curDepth - 1][yDest] = bestMoves[curDepth][yDest];
									}
								} else {
									if (bestMoves[curDepth][Score] < bestMoves[curDepth - 1][Score] || bestMoves[curDepth - 1][pieceIndex] < 0) {
										bestMoves[curDepth - 1][Score] = bestMoves[curDepth][Score];
										bestMoves[curDepth - 1][pieceIndex] = bestMoves[curDepth][pieceIndex];
										bestMoves[curDepth - 1][xDest] = bestMoves[curDepth][xDest];
										bestMoves[curDepth - 1][yDest] = bestMoves[curDepth][yDest];
									}
								}
							}

							EndOfBackup:;
							curPlayer = !curPlayer;
							curDepth--;
							//TODO- atm this likely won't work with check/stalemate positions
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
		
	}//End of massive while loop


Done:;
#ifdef PRINT_NODES
	end = clock();
	double cpu_time_used = ((double)((unsigned long long)end - (unsigned long long)start)) / CLOCKS_PER_SEC;
	double temp = (double)nodes;
	int nps = (int)round(temp / cpu_time_used);
	printf("  Nodes: %d\n  That is %d nodes per second.\n", (int)nodes, (int)nps);
#endif
	Move returns = { 0 };
	returns.PieceToMove = bestMoves[0][pieceIndex];
	returns.score = bestMoves[0][Score];
	returns.xTo = bestMoves[0][xDest];
	returns.yTo = bestMoves[0][yDest];
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
	for (int8_t cX = 0; cX < 8; cX++) {
		for (int8_t cY = 0; cY < 8; cY++) {
			//up right
			toX = cX + (int8_t)1;
			toY = cY + (int8_t)2;
			if (toX < (int8_t)8 && toY < (int8_t)8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//right up
			toX = cX + (int8_t)2;
			toY = cY + (int8_t)1;
			if (toX < (int8_t)8 && toY < (int8_t)8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//right down
			toX = cX + (int8_t)2;
			toY = cY - (int8_t)1;
			if (toX < (int8_t)8 && toY >= (int8_t)0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//down right
			toX = cX + (int8_t)1;
			toY = cY - (int8_t)2;
			if (toX < (int8_t)8 && toY >= (int8_t)0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//down left
			toX = cX - (int8_t)1;
			toY = cY - (int8_t)2;
			if (toX >= (int8_t)0 && toY >= (int8_t)0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//left down
			toX = cX - (int8_t)2;
			toY = cY - (int8_t)1;
			if (toX >= (int8_t)0 && toY >= (int8_t)0) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//left up
			toX = cX - (int8_t)2;
			toY = cY + (int8_t)1;
			if (toX >= (int8_t)0 && toY < (int8_t)8) {
				AllLegalMoves[cX][cY].toX[AllLegalMoves[cX][cY].moveC] = toX;
				AllLegalMoves[cX][cY].toY[AllLegalMoves[cX][cY].moveC] = toY;
				AllLegalMoves[cX][cY].moveC++;
			}

			//up left
			toX = cX - (int8_t)1;
			toY = cY + (int8_t)2;
			if (toX >= (int8_t)0 && toY < (int8_t)8) {
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