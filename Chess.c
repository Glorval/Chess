#include "Chess.h"

//u64 PosF(u64 x, u64 y) {
//	return(((u64)1 << x) << (y * (u64)8));
//}

u64 PosF(u64 x, u64 y) {
	return(((u64)1 << x) << y);
}

Game createGame(void) {
	Game returns = { 0 };

	//white setup, no Y value needed on the Pieces as it's initialized to zero
	returns.players[White].PieceC = 16;
	returns.players[White].Pieces[0].type = King;
	returns.players[White].Pieces[0].pos = sPos(E,0);
	returns.boards[White] |= BPosInd(E, 0);

	//returns.players[White].Pieces = calloc(16, sizeof(Piece));
	for (u8 cPawn = 1; cPawn < 9; cPawn++) {
		returns.players[White].Pieces[cPawn].type = Pawn;
		returns.players[White].Pieces[cPawn].pos = sPos(cPawn - 1, 1);

		returns.boards[White] |= BPosInd(cPawn - 1, 1);
	}

	returns.players[White].Pieces[9].type = Rook;
	returns.players[White].Pieces[9].pos = sPos(A, 0);
	returns.boards[White] |= BPosInd(A, 0);

	returns.players[White].Pieces[10].type = Knight;
	returns.players[White].Pieces[10].pos = sPos(B, 0);
	returns.boards[White] |= BPosInd(B, 0);

	returns.players[White].Pieces[11].type = Bishop;
	returns.players[White].Pieces[11].pos = sPos(C, 0);
	returns.boards[White] |= BPosInd(C, 0);

	returns.players[White].Pieces[12].type = Queen;
	returns.players[White].Pieces[12].pos = sPos(D,0);
	returns.boards[White] |= BPosInd(D, 0);

	returns.players[White].Pieces[13].type = Bishop;
	returns.players[White].Pieces[13].pos = sPos(F, 0);
	returns.boards[White] |= BPosInd(F, 0);

	returns.players[White].Pieces[14].type = Knight;
	returns.players[White].Pieces[14].pos = sPos(G, 0);
	returns.boards[White] |= BPosInd(G, 0);

	returns.players[White].Pieces[15].type = Rook;
	returns.players[White].Pieces[15].pos = sPos(H, 0);
	returns.boards[White] |= BPosInd(H, 0);



	//black setup
	returns.players[Black].PieceC = 16;
	returns.players[Black].Pieces[0].type = King;
	returns.players[Black].Pieces[0].pos = sPos(E, 7);
	returns.boards[Black] = BPosInd(E, 7);
	//returns.players[Black].Pieces = calloc(16, sizeof(Piece));
	for (u8 cPawn = 1; cPawn < 9; cPawn++) {
		returns.players[Black].Pieces[cPawn].type = Pawn;
		returns.players[Black].Pieces[cPawn].pos = sPos(cPawn - 1, 6);
		returns.boards[Black] |= BPosInd(cPawn - 1, 6);
	}



	returns.players[Black].Pieces[9].type = Rook;
	returns.players[Black].Pieces[9].pos = sPos(A, 7);
	returns.boards[Black] |= BPosInd(A, 7);

	returns.players[Black].Pieces[10].type = Knight;
	returns.players[Black].Pieces[10].pos = sPos(B, 7);
	returns.boards[Black] |= BPosInd(B, 7);

	returns.players[Black].Pieces[11].type = Bishop;
	returns.players[Black].Pieces[11].pos = sPos(C, 7);
	returns.boards[Black] |= BPosInd(C, 7);

	returns.players[Black].Pieces[12].type = Queen;
	returns.players[Black].Pieces[12].pos = sPos(D, 7);
	returns.boards[Black] |= BPosInd(D, 7);

	returns.players[Black].Pieces[13].type = Bishop;
	returns.players[Black].Pieces[13].pos = sPos(F, 7);
	returns.boards[Black] |= BPosInd(F, 7);

	returns.players[Black].Pieces[14].type = Knight;
	returns.players[Black].Pieces[14].pos = sPos(G, 7);
	returns.boards[Black] |= BPosInd(G, 7);

	returns.players[Black].Pieces[15].type = Rook;
	returns.players[Black].Pieces[15].pos = sPos(H, 7);
	returns.boards[Black] |= BPosInd(H, 7);

	return(returns);

}

void printBitboard(const u64 board){
	for (u64 y = 7; y < 10/*overflow check :p*/; y--) {
		printSpace;
		for(u64 x = 0; x< 8; x++){
			printf("%u", (BPosInd(x,y) & board) != 0);
		}
		printf("\n");
	}
}

//COMPAT_PRINT makes it not use 24 bit colours (Not implemented in this version)
void printGame(const Game* game) {
	const static char yLables[] = "12345678";
	const static char xLables[] = "abcdefgh";
	u8 UncompBoard[BoardDim][BoardDim][2] = {0};

	
	for (u8 CWP = 0; CWP < game->players[White].PieceC; CWP++) {
		UncompBoard[game->players[White].Pieces[CWP].pos Xind][game->players[White].Pieces[CWP].pos Yind][0] = White;
		UncompBoard[game->players[White].Pieces[CWP].pos Xind][game->players[White].Pieces[CWP].pos Yind][1] = game->players[White].Pieces[CWP].type;
	}
	for (u8 CBP = 0; CBP < game->players[Black].PieceC; CBP++) {
		UncompBoard[game->players[Black].Pieces[CBP].pos Xind][game->players[Black].Pieces[CBP].pos Yind][0] = Black;
		UncompBoard[game->players[Black].Pieces[CBP].pos Xind][game->players[Black].Pieces[CBP].pos Yind][1] = game->players[Black].Pieces[CBP].type;
	}

	printf("\n");//Later shall be used for more info


	//top left corner of frame
	printf("  %c", 218);
	//top of frame
	for (u8 c = 0; c < 17; c++) {
		printf("%c", 196);
	}
	//top right corner of frame
	printf("%c\n", 191);

	//loop through the Y axis of the board
	for (i8 cY = BoardDim - 1; cY >= 0; cY--) {
		//print the current y label and then the frame
		printf(" %c%c", yLables[cY], 179);

		#ifndef COMPAT_MODE
		//alternate between printing a half-square for white & black squares to center the board
		if (cY % 2 == 0) {
			setBlackFore;
			printf("%c", 222);
		} else {
			setWhiteFore;
			printf("%c", 222);
		}
		#endif



		//loop through the x axis
		for (u8 cX = 0; cX < BoardDim; cX++) {
			//Alternate between it being a black/white square
			if (abs(cX - cY) % 2 == 0) {
				//black square

				//checking who owns the piece
				if (UncompBoard[cX][cY][0] == White) {
					//White piece, black square
					#ifdef COMPAT_MODE
					printf("\33[97;100m%c", PiecesSymbol[UncompBoard[cX][cY][0]]);
					#else					
					setBlackSquare;
					setFullWhiteFore;
					printf("%c", PiecesSymbol[UncompBoard[cX][cY][1]]);
					#endif				
				} else {
					//black Pieces, black square
					#ifdef COMPAT_MODE
					printf("\33[30;100m%c", PiecesSymbol[UncompBoard[cX][cY][0]]);
					#else
					setBlackSquare;
					setFullBlackFore;
					printf("%c", PiecesSymbol[UncompBoard[cX][cY][1]]);
					#endif					
				}
				//print out extra space to make board square
				#ifndef COMPAT_MODE
				if (cX < BoardDim - 1) {
					setWhiteFore;
					printf("%c", 222);
				} else {
					setFullBlackFore;
					printf("%c", 222);
				}
				#else
				printf(" ");
				#endif
			} else {
				//white square

				//checking who owns the piece
				if (UncompBoard[cX][cY][0] == White) {
					//White piece, white square
					#ifdef COMPAT_MODE
					printf("\33[97;47m%c", PiecesSymbol[UncompBoard[cX][cY][0]]);
					#else
					setWhiteSquare;//white background
					setFullWhiteFore;
					printf("%c", PiecesSymbol[UncompBoard[cX][cY][1]]);
					#endif					
				} else {
					//black Pieces, white square
					#ifdef COMPAT_MODE
					printf("\33[30;47m%c", PiecesSymbol[UncompBoard[cX][cY][0]]);
					#else
					setWhiteSquare;//white background
					setFullBlackFore;
					printf("%c", PiecesSymbol[UncompBoard[cX][cY][1]]);
					#endif					
				}
				//give extra space to make board square
				#ifndef COMPAT_MODE
				if (cX < BoardDim - 1) {
					setBlackFore;
					printf("%c", 222);
				} else {
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
		#ifdef COMPAT_MODE
		printf(" ");
		#endif
		printf("%c\n", 179);
	}
	//bottom left corner of frame
	printf("  %c", 192);

	//bottom of frame
	for (u8 c = 0; c < 17; c++) {
		printf("%c", 196);
	}

	//bottom right corner of frame
	printf("%c\n    ", 217);

	//bottom 'abcdefgh' labels
	for (u8 cLabel = 0; cLabel < BoardDim; cLabel++) {
		printf("%c ", xLables[cLabel]);
	}
	printf("\n");
	






}


void makeMove(Game* game, const Move move) {

	if (move.moveFlags == MV_NORMAL) {//Non-taking normal move
		//Use the piece's original position to remove it from the bitboard, update the piece's position, then add it back to the board
		game->boards[move.player] ^= BPosC(game->players[move.player].Pieces[move.pieceInds].pos);
		game->players[move.player].Pieces[move.pieceInds].pos = move.newPos;
		game->boards[move.player] |= BPosC(game->players[move.player].Pieces[move.pieceInds].pos);
	} else if (move.moveFlags == MV_TAKING) {//Normal move, but taking		
		const u8 indOne = move.pieceInds MV_PIND_ONE;
		const u8 indTwo = move.pieceInds MV_PIND_TWO;

		//Use the piece's original position to remove it from the bitboard, update the piece's position, then add it back to the board
		game->boards[move.player] ^= BPosC(game->players[move.player].Pieces[indOne].pos);
		game->players[move.player].Pieces[indOne].pos = move.newPos;
		game->boards[move.player] |= BPosC(game->players[move.player].Pieces[indOne].pos);

		//Then, remove the captured piece from their piece list and board
		game->boards[!move.player] ^= BPosC(game->players[move.player].Pieces[indOne].pos);
		game->players[!move.player].PieceC--;
		game->players[!move.player].Pieces[indTwo] = game->players[!move.player].Pieces[game->players[!move.player].PieceC];
	} else if (move.moveFlags == MV_BOO) {
		const u8 indOne = move.pieceInds MV_PIND_ONE;
		const u8 indTwo = move.pieceInds MV_PIND_TWO;

		//Remove both original positions, then update piece list positions and use that to add in their new positions
		game->boards[move.player] ^= BPosC(game->players[move.player].Pieces[indOne].pos);
		game->boards[move.player] ^= BPosC(game->players[move.player].Pieces[indTwo].pos);
	}
	
}