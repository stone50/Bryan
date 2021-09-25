#pragma once

#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

class Position {
public:
	char board[8][8] = {				// layout of pieces represented in a 2d array
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'},
		{'-', '-', '-', '-', '-', '-', '-', '-'}
	};
	bool whiteMove = true;				// true if it is white's turn
	string castle = "-";				// available castle moves
	string ep = "-";					// en passant square | is "-" if no en passant
	unsigned char fiftyMoveRule = 0;	// counting the number of moves without a pawn move or a capture
	unsigned short int moveCount = 1;	// move number

#pragma region constructors

	Position();

	Position(string FEN);

	Position(char tboard[8][8], bool twhiteMove, string tcastle, string tep, unsigned char tfiftyMoveRule, unsigned short int tmoveCount);

#pragma endregion

	static Position StartingPosition();

#pragma region general functions

	// generates an FEN based on the board position
	string FEN();

	// creates a board position based on the FEN
	void setToFEN(string FEN);

	// calculates a list of legal moves
	vector<string> legalMoves();
	
	// prints the board to the console
	void printBoard();

#pragma endregion

private:

#pragma region helper functions

	// sets squares which are attacked by the enemy king
	void kingAttack(
		unsigned char row,
		unsigned char col,
		unordered_set<unsigned char>* kingDangerSquares
	);

	// sets squares which are attacked by the enemy bishops
	void bishopAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares,
		unordered_set<unsigned char>* pinnedPieces
	);

	// sets squares which are attacked by the enemy knights
	void knightAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

	// sets squares which are attacked by the enemy rooks
	void rookAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares,
		unordered_set<unsigned char>* pinnedPieces
	);
	
	// sets squares which are attacked by the enemy pawns
	void pawnAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

	// sets squares which are attacked by enemy 'sliders' (queens, bishops, and rooks)
	// returns true when a piece should stop attacking in the current direction
	bool sliderAttack(
		unsigned char pieceRow,
		unsigned char pieceCol,
		unsigned char attackRow,
		unsigned char attackCol,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares,
		unsigned char* pin,
		unordered_set<unsigned char>* pinnedPieces
	);

	// sets squares which are attacked by enemy 'non-sliders' (kings, knights, and pawns)
	void nonSliderAttack(
		unsigned char pieceRow,
		unsigned char pieceCol,
		unsigned char attackRow,
		unsigned char attackCol,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

	// generates a string that represents a move and adds it to the moves vector
	// returns false if a pinned piece cannot move to the given square
	bool generateMove(
		unsigned char startRow,
		unsigned char startCol,
		unsigned char endRow,
		unsigned char endCol,
		vector<string>* moves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol,
		char promote = '-',
		bool ep = false
	);

	// generates moves for bishops
	void generateBishopMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	// generates moves for knights
	void generateKnightMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	// generates moves for rooks
	void generateRookMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	// generates moves for pawns
	void generatePawnMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	string translateMove(string move);

#pragma endregion
};