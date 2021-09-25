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

	void kingAttack(
		unsigned char row,
		unsigned char col,
		unordered_set<unsigned char>* kingDangerSquares
	);

	void bishopAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares,
		unordered_set<unsigned char>* pinnedPieces
	);

	void knightAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

	void rookAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares,
		unordered_set<unsigned char>* pinnedPieces
	);
	
	void pawnAttack(
		unsigned char row,
		unsigned char col,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

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

	void nonSliderAttack(
		unsigned char pieceRow,
		unsigned char pieceCol,
		unsigned char attackRow,
		unsigned char attackCol,
		unsigned char* check,
		bool* doubleCheck,
		unordered_set<unsigned char>* kingDangerSquares
	);

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

	void generateBishopMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	void generateKnightMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

	void generateRookMoves(
		unsigned char row,
		unsigned char col,
		vector<string>* pseudoLegalMoves,
		unordered_set<unsigned char>* pinnedPieces,
		unsigned char kingRow,
		unsigned char kingCol
	);

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