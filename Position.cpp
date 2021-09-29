#include <iostream>
#include "Position.h"

using namespace std;

#pragma region constructors

// constructs a position with default attributes
Position::Position() {}

// constructs a position based on an FEN
Position::Position(string FEN) {
	setToFEN(FEN);
}

// constructs a position based on the given info
Position::Position(
	char tboard[8][8],
	bool twhiteMove,
	string tcastle,
	string tep,
	unsigned char tfiftyMoveRule,
	unsigned short int tmoveCount
) : 
	whiteMove(twhiteMove),
	castle(tcastle),
	ep(tep),
	fiftyMoveRule(tfiftyMoveRule),
	moveCount(tmoveCount)
{
	for (unsigned char row = 0; row < 8; row++) {
		for (unsigned char col = 0; col < 8; col++) {
			board[row][col] = tboard[row][col];
		}
	}
}

#pragma endregion

#pragma region general functions

// returns a Position with the starting position
Position Position::StartingPosition() {
	return Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// generates an FEN based on the board position
string Position::FEN() {

	// adds the piece layout
	string out;
	for (unsigned char row = 0; row < 8; row += 1) {
		unsigned char count = 0;
		for (unsigned char col = 0; col < 8; col += 1) {
			char thisSquare = board[row][col];
			if (thisSquare == '-') {
				count++;
			}
			else {
				if (count != 0) {
					out += to_string(count);
					count = 0;
				}
				out += thisSquare;
			}
		}
		if (count != 0) {
			out += to_string(count);
		}
		out += "/";
	}
	out.pop_back();

	// adds a 'w' or 'b' depending on whose move it is
	out += " ";
	out += whiteMove ? "w" : "b";

	// adds the castle moves, en passant move, fifty move rule counter and overall move counter
	out += " " + castle + " " + ep + " " + to_string(fiftyMoveRule) + " " + to_string(moveCount);
	return out;
}

// creates a board position based on the FEN
void Position::setToFEN(string FEN) {
	unsigned char row = 0;
	unsigned char col = 0;
	unsigned char count = 0;
	char item = FEN.at(0);

	// sets the board layout
	while (item != ' ') {
		if (isdigit(item)) {
			unsigned char num = FEN.at(count) - 48;
			for (unsigned char i = 0; i < num; i++) {
				board[row][col + i] = '-';
			}
			col += num;
		}
		else if (item == '/') {
			row++;
			col = 0;
		}
		else {
			board[row][col++] = item;
		}
		item = FEN.at(++count);
	}

	// sets whose move it is
	item = FEN.at(++count);
	whiteMove = item == 'w';

	// sets the available castle moves
	count += 2;
	unsigned char num = count;
	while (FEN.at(num) != ' ') {
		num++;
	}
	castle = FEN.substr(count, num - count);
	count = num + 1;

	// sets the en passant square
	if (FEN.at(count) == '-') {
		ep = "-";
	}
	else {
		ep = FEN.substr(count, 2);
		count++;
	}
	count += 2;

	// sets the fifty move rule
	num = count;
	while (FEN.at(num) != ' ') {
		num++;
	}
	fiftyMoveRule = stoi(FEN.substr(count, num - count));
	count = num + 1;

	// sets the move count
	moveCount = stoi(FEN.substr(count, FEN.length()));
}

// returns a list of legal moves
vector<string> Position::legalMoves() {
	vector<string> moves;
	unsigned char check = 64;
	bool doubleCheck = false;
	unsigned char kingRow;
	unsigned char kingCol;
	unordered_set<unsigned char> kingDangerSquares;
	unordered_set<unsigned char> pinnedPieces;

#pragma region generate attacked squares

	for (unsigned char row = 0; row < 8; row++) {
		for (unsigned char col = 0; col < 8; col++) {
			char currentPiece = board[row][col];
			if (whiteMove ? islower(currentPiece) : isupper(currentPiece)) {
				switch (whiteMove ? currentPiece : currentPiece + 32) {
				case 'k':
					kingAttack(
						row,
						col,
						&kingDangerSquares
					);
					break;
				case 'q':
					bishopAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares,
						&pinnedPieces
					);
					rookAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares,
						&pinnedPieces
					);
					break;
				case 'b':
					bishopAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares,
						&pinnedPieces
					);
					break;
				case 'n':
					knightAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares
					);
					break;
				case 'r':
					rookAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares,
						&pinnedPieces
					);
					break;
				case 'p':
					pawnAttack(
						row,
						col,
						&check,
						&doubleCheck,
						&kingDangerSquares
					);
					break;
				}
			}
			else if (currentPiece == (whiteMove ? 'K' : 'k')) {
				kingRow = row;
				kingCol = col;
			}
		}
	}

#pragma endregion
	
	generateKingMoves(
		kingRow,
		kingCol,
		&moves,
		&kingDangerSquares
	);

	if (doubleCheck) {
		return moves;
	}

	vector<string> pseudoLegalMoves;

#pragma region generate pseudo legal moves

	for (unsigned char row = 0; row < 8; row++) {
		for (unsigned char col = 0; col < 8; col++) {
			char currentPiece = board[row][col];
			if (whiteMove ? isupper(currentPiece) : islower(currentPiece)) {
				switch (whiteMove ? currentPiece + 32 : currentPiece) {
				case 'q':
					generateBishopMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					generateRookMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					break;
				case 'b':
					generateBishopMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					break;
				case 'n':
					generateKnightMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					break;
				case 'r':
					generateRookMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					break;
				case 'p':
					generatePawnMoves(
						row,
						col,
						&pseudoLegalMoves,
						&pinnedPieces,
						kingRow,
						kingCol
					);
					break;
				}
			}
		}
	}

#pragma endregion

#pragma region add correct pseudo legal moves to moves

	if (check < 64) {
		unsigned char attackerRow = check / 8;
		unsigned char attackerCol = check % 8;
		char attacker = board[attackerRow][attackerCol];
		if (
			attacker != (whiteMove ? 'n' : 'N') &&
			(abs(attackerRow - kingRow) >= 2 || abs(attackerCol - kingCol) >= 2)
		) {
				// check if attacker can be blocked
				// notes:
				// pinned pieces cannot block check
				// en passant cannot block check
			for (unsigned char i = 0; i < pseudoLegalMoves.size(); i++) {
				string move = pseudoLegalMoves.at(i);
				unsigned char endMoveRow = move.at(2);
				unsigned char endMoveCol = move.at(3);
				unsigned char minRow = min(kingRow, attackerRow);
				unsigned char minCol = min(kingCol, attackerCol);
				unsigned char maxRow = max(kingRow, attackerRow);
				unsigned char maxCol = max(kingCol, attackerCol);
				if (
					onLine(
						kingRow,
						kingCol,
						attackerRow,
						attackerCol,
						endMoveRow,
						endMoveCol
					) &&
					(isBetween(endMoveRow, minRow, maxRow) || isBetween(endMoveCol, minCol, maxCol))
				) {
					moves.push_back(move);
				}
			}
		}

		// check if attacker can be captured
		for (unsigned char i = 0; i < pseudoLegalMoves.size(); i++) {
			string move = pseudoLegalMoves.at(i);
			unsigned char endMoveRow = move.at(2);
			unsigned char endMoveCol = move.at(3);
			if (endMoveCol == attackerCol) {

				if (move.length() == 5 && move.at(4) == 'e') {

					// en passant
					if (
						56 - ep.at(1) + (whiteMove ? 1 : -1) == attackerRow &&
						ep.at(0) - 97 == attackerCol
					) {
						moves.push_back(move);
					}
				}
				else if (endMoveRow == attackerRow) {
					moves.push_back(move);
				}
			}
		}
	}
	else {
		moves.insert(moves.end(), pseudoLegalMoves.begin(), pseudoLegalMoves.end());
	}

#pragma endregion

#pragma region castle
	if (check == 64) {
		if (castle.find(whiteMove ? 'K' : 'k') != string::npos) {
			signed char direction = whiteMove ? 1 : -1;
			unsigned char kingColAndDirection = kingCol + direction;
			unsigned char kingColAndDoubleDirection = kingColAndDirection + direction;
			if (
				!kingDangerSquares.count(rowColToChar(kingRow, kingColAndDirection)) &&
				!kingDangerSquares.count(rowColToChar(kingRow, kingColAndDoubleDirection)) &&
				board[kingRow][kingColAndDirection] == '-' &&
				board[kingRow][kingColAndDoubleDirection] == '-'
			) {
				moves.push_back("O-O");
			}
		}
		if (castle.find(whiteMove ? 'Q' : 'q') != string::npos) {
			signed char direction = whiteMove ? -1 : 1;
			unsigned char kingColAndDirection = kingCol + direction;
			unsigned char kingColAndDoubleDirection = kingColAndDirection + direction;
			if (
				!kingDangerSquares.count(rowColToChar(kingRow, kingColAndDirection)) &&
				!kingDangerSquares.count(rowColToChar(kingRow, kingColAndDoubleDirection)) &&
				board[kingRow][kingColAndDirection] == '-' &&
				board[kingRow][kingColAndDoubleDirection] == '-'
			) {
				moves.push_back("O-O-O");
			}
		}
	}

#pragma endregion

	return moves;
}

// prints the board to the console
void Position::printBoard() {
	cout << "\n  -------------------\n";
	for (unsigned char row = 0; row < 8; row++) {
		cout << 8 - row << " | ";
		for (unsigned char col = 0; col < 8; col++) {
			cout << board[row][col] << " ";
		}
		cout << "|\n";
	}
	cout << "  -------------------\n    a b c d e f g h\n\n";
}

// returns a string representing a move that is more readable for humans
string Position::translateMove(string move) {
	if (move == "O-O" || move == "O-O-O") {
		return move;
	}
	string out = "";
	out += (move.at(0) % 8) + 97;
	out += to_string(8 - (move.at(0) / 8));
	out += '-';
	out += (move.at(1) % 8) + 97;
	out += to_string(8 - (move.at(1) / 8));

	if (move.length() == 3) {
		if (move.at(2) == 'e') {
			out += " ep";
		}
		else {
			out += '=';
			out += move.at(2);
		}
	}

	return out;
}

#pragma endregion

#pragma region helper functions

// sets squares which are attacked by the enemy king
void Position::kingAttack(
	unsigned char row,
	unsigned char col,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (row == 0) {
		unsigned char rowBottom = row + 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colRight));
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colLeft));
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colRight));
			kingDangerSquares->insert(rowColToChar(rowBottom, colLeft));
		}
	}
	else if (row == 7) {
		unsigned char rowTop = row - 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colRight));
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colLeft));
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colRight));
			kingDangerSquares->insert(rowColToChar(rowTop, colLeft));
		}
	}
	else {
		unsigned char rowTop = row - 1;
		unsigned char rowBottom = row + 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colRight));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colRight));
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colLeft));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colLeft));
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			kingDangerSquares->insert(rowColToChar(row, colRight));
			kingDangerSquares->insert(rowColToChar(row, colLeft));
			kingDangerSquares->insert(rowColToChar(rowTop, col));
			kingDangerSquares->insert(rowColToChar(rowTop, colRight));
			kingDangerSquares->insert(rowColToChar(rowTop, colLeft));
			kingDangerSquares->insert(rowColToChar(rowBottom, col));
			kingDangerSquares->insert(rowColToChar(rowBottom, colRight));
			kingDangerSquares->insert(rowColToChar(rowBottom, colLeft));
		}
	}
}

// sets squares which are attacked by the enemy bishops
void Position::bishopAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares,
	unordered_set<unsigned char>* pinnedPieces
) {
	unsigned char topLeftMax = min(row, col);
	unsigned char topRightMax = min(row, unsigned char(7 - col));
	unsigned char bottomRightMax = min(unsigned char(7 - row), unsigned char(7 - col));
	unsigned char bottomLeftMax = min(unsigned char(7 - row), col);
	unsigned char pin = 64;

	// attack squares up and left
	for (unsigned char topLeft = 1; topLeft <= topLeftMax; topLeft++) {
		if (sliderAttack(
			row,
			col,
			row - topLeft,
			col - topLeft,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	// attack squares up and right
	for (unsigned char topRight = 1; topRight <= topRightMax; topRight++) {
		if (sliderAttack(
			row,
			col,
			row - topRight,
			col + topRight,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	//attack squares down and left
	for (unsigned char bottomLeft = 1; bottomLeft <= bottomLeftMax; bottomLeft++) {
		if (sliderAttack(
			row,
			col,
			row + bottomLeft,
			col - bottomLeft,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	// attack squares down and right
	for (unsigned char bottomRight = 1; bottomRight <= bottomRightMax; bottomRight++) {
		if (sliderAttack(
			row,
			col,
			row + bottomRight,
			col + bottomRight,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
}

// sets squares which are attacked by the enemy knights
void Position::knightAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (row <= 1) {
		if (row == 0) {
			if (col <= 1) {

				// row = 0 | col = 0
				if (col == 0) {
					nonSliderAttack(
						row,
						col,
						2,
						1,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						1,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 0 | col = 1
				else {
					nonSliderAttack(
						row,
						col,
						2,
						0,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						1,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}
			else if (col >= 6) {

				// row = 0 | col = 7
				if (col == 7) {
					nonSliderAttack(
						row,
						col,
						1,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						6,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 0 | col = 6
				else {
					nonSliderAttack(
						row,
						col,
						1,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}

			// row = 0 | 1 < col < 6
			else {
				nonSliderAttack(
					row,
					col,
					1,
					col - 2,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					2,
					col - 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					2,
					col + 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					1,
					col + 2,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}
		else {
			if (col <= 1) {

				// row = 1 | col = 0
				if (col == 0) {
					nonSliderAttack(
						row,
						col,
						0,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						3,
						1,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 1 | col = 1
				else {
					nonSliderAttack(
						row,
						col,
						0,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						3,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						3,
						0,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}
			else if (col >= 6) {

				// row = 1 | col = 7
				if (col == 7) {
					nonSliderAttack(
						row,
						col,
						0,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					); nonSliderAttack(
						row,
						col,
						3,
						6,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 1 | col = 6
				else {
					nonSliderAttack(
						row,
						col,
						0,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						2,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						3,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						3,
						7,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}

			// row = 1 | 1 < col < 6
			else {
				unsigned char downOne = row + 1;
				unsigned char downTwo = row + 2;
				unsigned char upOne = row - 1;
				unsigned char rightTwo = col + 2;
				unsigned char leftTwo = col - 2;
				nonSliderAttack(
					row,
					col,
					downOne,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					col - 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					col + 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downOne,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upOne,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upOne,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}
	}
	else if (row >= 6) {
		if (row == 7) {
			if (col <= 1) {

				// row = 7 | col = 0
				if (col == 0) {
					nonSliderAttack(
						row,
						col,
						5,
						1,
						check,
						doubleCheck,
						kingDangerSquares
					); nonSliderAttack(
						row,
						col,
						6,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 7 | col = 1
				else {
					nonSliderAttack(
						row,
						col,
						5,
						0,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						6,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}
			else if (col >= 6) {

				// row = 7 | col = 7
				if (col == 7) {
					nonSliderAttack(
						row,
						col,
						5,
						6,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						6,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 7 | col = 6
				else {
					nonSliderAttack(
						row,
						col,
						5,
						7,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						6,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}

			// row = 7 | 1 < col < 6
			else {
				unsigned char upOne = row - 1;
				unsigned char upTwo = row - 2;
				nonSliderAttack(
					row,
					col,
					upOne,
					col - 2,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					col - 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					col + 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upOne,
					col + 2,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}
		else {
			if (col <= 1) {

				// row = 6 | col = 0
				if (col == 0) {
					nonSliderAttack(
						row,
						col,
						4,
						1,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						7,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 6 | col = 1
				else {
					nonSliderAttack(
						row,
						col,
						4,
						0,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						4,
						2,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						7,
						3,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}
			else if (col >= 6) {

				// row = 6 | col = 7
				if (col == 7) {
					nonSliderAttack(
						row,
						col,
						4,
						6,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						7,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}

				// row = 6 | col = 6
				else {
					nonSliderAttack(
						row,
						col,
						7,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						4,
						5,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						5,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
					nonSliderAttack(
						row,
						col,
						7,
						4,
						check,
						doubleCheck,
						kingDangerSquares
					);
				}
			}

			// row = 6 | 1 < col < 6
			else {
				unsigned char upOne = row - 1;
				unsigned char upTwo = row - 2;
				unsigned char downOne = row + 1;
				unsigned char rightTwo = col + 2;
				unsigned char leftTwo = col - 2;
				nonSliderAttack(
					row,
					col,
					downOne,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upOne,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					col - 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					col + 1,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upOne,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downOne,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}
	}
	else {
		if (col <= 1) {

			// 1 < row < 6 | col = 0
			if (col == 0) {
				unsigned char rightOne = col + 1;
				unsigned char rightTwo = col + 2;
				nonSliderAttack(
					row,
					col,
					row - 2,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row - 1,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 1,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 2,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}

			// 1 < row < 6 | col = 1
			else {
				unsigned char leftOne = col - 1;
				unsigned char rightOne = col + 1;
				unsigned char rightTwo = col + 2;
				unsigned char upTwo = row - 2;
				unsigned char downTwo = row + 2;
				nonSliderAttack(
					row,
					col,
					upTwo,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row - 1,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 1,
					rightTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}
		else if (col >= 6) {

			// 1 < row < 6 | col = 7
			if (col == 7) {
				unsigned char leftOne = col - 1;
				unsigned char leftTwo = col - 2;
				nonSliderAttack(
					row,
					col,
					row - 2,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row - 1,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 1,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 2,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}

			// 1 < row < 6 | col = 6
			else {
				unsigned char rightOne = col + 1;
				unsigned char leftOne = col - 1;
				unsigned char leftTwo = col - 2;
				unsigned char upTwo = row - 2;
				unsigned char downTwo = row + 2;
				nonSliderAttack(
					row,
					col,
					upTwo,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					upTwo,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row - 1,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					row + 1,
					leftTwo,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					leftOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
				nonSliderAttack(
					row,
					col,
					downTwo,
					rightOne,
					check,
					doubleCheck,
					kingDangerSquares
				);
			}
		}

		// 1 < row < 6 | 1 < col < 6
		else {
			unsigned char rightOne = col + 1;
			unsigned char rightTwo = col + 2;
			unsigned char leftOne = col - 1;
			unsigned char leftTwo = col - 2;
			unsigned char upOne = row - 1;
			unsigned char upTwo = row - 2;
			unsigned char downOne = row + 1;
			unsigned char downTwo = row + 2;
			nonSliderAttack(
				row,
				col,
				upTwo,
				leftOne,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				upTwo,
				rightOne,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				upOne,
				rightTwo,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				downOne,
				rightTwo,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				downTwo,
				rightOne,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				downTwo,
				leftOne,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				downOne,
				leftTwo,
				check,
				doubleCheck,
				kingDangerSquares
			);
			nonSliderAttack(
				row,
				col,
				upOne,
				leftTwo,
				check,
				doubleCheck,
				kingDangerSquares
			);
		}
	}
}

// sets squares which are attacked by the enemy rooks
void Position::rookAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares,
	unordered_set<unsigned char>* pinnedPieces
) {
	unsigned char pin = 64;
	unsigned char bottomMax = 7 - row;
	unsigned char rightMax = 7 - col;

	// attack up
	for (unsigned char top = 1; top <= row; top++) {
		if (sliderAttack(
			row,
			col,
			row - top,
			col,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	//attack down
	for (unsigned char bottom = 1; bottom <= bottomMax; bottom++) {
		if (sliderAttack(
			row,
			col,
			row + bottom,
			col,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	//attack right
	for (unsigned char right = 1; right <= rightMax; right++) {
		if (sliderAttack(
			row,
			col,
			row,
			col + right,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
	pin = 64;

	// attack left
	for (unsigned char left = 1; left <= col; left++) {
		if (sliderAttack(
			row,
			col,
			row,
			col - left,
			check,
			doubleCheck,
			kingDangerSquares,
			&pin,
			pinnedPieces
		)) {
			break;
		}
	}
}

// sets squares which are attacked by the enemy pawns
void Position::pawnAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares
) {
	unsigned char rowAndDirection = row + (whiteMove ? 1 : -1);

	// attack right
	if (col <= 6) {
		nonSliderAttack(
			row,
			col,
			rowAndDirection,
			col + 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}

	// attack left
	if (col >= 1) {
		nonSliderAttack(
			row,
			col,
			rowAndDirection,
			col - 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
}

// sets squares which are attacked by enemy 'sliders' (queens, bishops, and rooks)
// returns true when a piece should stop attacking in the current direction
bool Position::sliderAttack(
	unsigned char pieceRow,
	unsigned char pieceCol,
	unsigned char attackRow,
	unsigned char attackCol,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares,
	unsigned char* pin,
	unordered_set<unsigned char>* pinnedPieces
) {
	char square = board[attackRow][attackCol];

	// if the pin variable is set and the attacked square is the enemy king then the piece on the pin square is pinned
	if (*pin < 64) {
		if (square == (whiteMove ? 'K' : 'k')) {
			pinnedPieces->insert(*pin);
			return true;
		}
		else if (square != '-') {
			return true;
		}
		return false;
	}

	// if the attacked piece is an enemy then check for a pin
	else {
		if (square == '-') {
			kingDangerSquares->insert(rowColToChar(attackRow, attackCol));
			return false;
		}
		else if (square == (whiteMove ? 'K' : 'k')) {
			if (*check < 64) {
				*doubleCheck = true;
			}
			else {
				*check = rowColToChar(pieceRow, pieceCol);
				kingDangerSquares->insert(rowColToChar(attackRow, attackCol));
			}
			return false;
		}
		else {
			if (whiteMove ? islower(square) : isupper(square)) {
				kingDangerSquares->insert(rowColToChar(attackRow, attackCol));
				return true;
			}
			else {
				*pin = rowColToChar(attackRow, attackCol);
				return false;
			}
		}
	}
}

// sets squares which are attacked by enemy 'non-sliders' (kings, knights, and pawns)
void Position::nonSliderAttack(
	unsigned char pieceRow,
	unsigned char pieceCol,
	unsigned char attackRow,
	unsigned char attackCol,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (board[attackRow][attackCol] == (whiteMove ? 'K' : 'k')) {
		if (*check < 64) {
			*doubleCheck = true;
		}
		else {
			*check = rowColToChar(pieceRow, pieceCol);
			kingDangerSquares->insert(rowColToChar(attackRow, attackCol));
		}
	}
	else {
		kingDangerSquares->insert(rowColToChar(attackRow, attackCol));
	}
}

// generates a string that represents a move and adds it to the moves vector
// returns false if a pinned piece cannot move to the given square
bool Position::generateMove(
	unsigned char startRow,
	unsigned char startCol,
	unsigned char endRow,
	unsigned char endCol,
	vector<string>* moves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol,
	char promote,
	bool ep
) {
	if (whiteMove ? isupper(board[endRow][endCol]) : islower(board[endRow][endCol])) {
		return false;
	}

	// if a piece is pinned then it should only be able to move in a straight line towards or away from the king
	if (pinnedPieces->count(rowColToChar(startRow, startCol))) {
		if (board[startRow][startCol] == (whiteMove ? 'N' : 'n') || !onLine(
			kingRow,
			kingCol,
			startRow,
			startCol,
			endRow,
			endCol
		)) {
			return false;
		}
	}

	string move = "";
	move += rowColToChar(startRow, startCol);
	move += rowColToChar(endRow, endCol);

	if (promote != '-') {
		move += promote;
	}
	else if (ep) {
		move += 'e';
	}
	moves->push_back(move);
	return true;
}

// generates a string that represents a move and adds it to the moves vector
// returns false if the move is not valid
bool Position::generateKingMove(
	unsigned char startRow,
	unsigned char startCol,
	unsigned char endRow,
	unsigned char endCol,
	vector<string>* moves,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (
		kingDangerSquares->count(rowColToChar(endRow, endCol)) ||
		(whiteMove ? isupper(board[endRow][endCol]) : islower(board[endRow][endCol]))
	) {
		return false;
	}

	string move = "";
	move += rowColToChar(startRow, startCol);
	move += rowColToChar(endRow, endCol);

	moves->push_back(move);
	return true;
}

// generates moves for king
void Position::generateKingMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* moves,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (row == 0) {
		unsigned char rowBottom = row + 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colRight,
				moves,
				kingDangerSquares
			);
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
	}
	else if (row == 7) {
		unsigned char rowTop = row - 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colRight,
				moves,
				kingDangerSquares
			);
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
	}
	else {
		unsigned char rowTop = row - 1;
		unsigned char rowBottom = row + 1;
		if (col == 0) {
			unsigned char colRight = col + 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colRight,
				moves,
				kingDangerSquares
			);
		}
		else if (col == 7) {
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
		else {
			unsigned char colRight = col + 1;
			unsigned char colLeft = col - 1;
			generateKingMove(
				row,
				col,
				row,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				row,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowTop,
				colLeft,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				col,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colRight,
				moves,
				kingDangerSquares
			);
			generateKingMove(
				row,
				col,
				rowBottom,
				colLeft,
				moves,
				kingDangerSquares
			);
		}
	}
}

// generates moves for bishops
void Position::generateBishopMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	unsigned char topLeftMax = min(row, col);
	unsigned char topRightMax = min(row, unsigned char(7 - col));
	unsigned char bottomRightMax = min(unsigned char(7 - row), unsigned char(7 - col));
	unsigned char bottomLeftMax = min(unsigned char(7 - row), col);

	// search squares up and left
	for (unsigned char topLeft = 1; topLeft <= topLeftMax; topLeft++) {
		unsigned char rowIndex = row - topLeft;
		unsigned char colIndex = col - topLeft;
		char square = board[rowIndex][colIndex];
		if (!generateMove(
			row,
			col,
			rowIndex,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	// search squares up and right
	for (unsigned char topRight = 1; topRight <= topRightMax; topRight++) {
		unsigned char rowIndex = row - topRight;
		unsigned char colIndex = col + topRight;
		char square = board[rowIndex][colIndex];
		if (!generateMove(
			row,
			col,
			rowIndex,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	// search squares down and left
	for (unsigned char bottomLeft = 1; bottomLeft <= bottomLeftMax; bottomLeft++) {
		unsigned char rowIndex = row + bottomLeft;
		unsigned char colIndex = col - bottomLeft;
		char square = board[rowIndex][colIndex];
		if (!generateMove(
			row,
			col,
			rowIndex,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	//search squares down and right
	for (unsigned char bottomRight = 1; bottomRight <= bottomRightMax; bottomRight++) {
		unsigned char rowIndex = row + bottomRight;
		unsigned char colIndex = col + bottomRight;
		char square = board[rowIndex][colIndex];
		if (!generateMove(
			row,
			col,
			rowIndex,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}
}

// generates moves for knights
void Position::generateKnightMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	if (row <= 1) {
		if (row == 0) {
			if (col <= 1) {

				// row = 0 | col = 0
				if (col == 0) {
					generateMove(
						row,
						col,
						2,
						1,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						1,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 0 | col = 1
				else {
					generateMove(
						row,
						col,
						2,
						0,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						1,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}
			else if (col >= 6) {

				// row = 0 | col = 7
				if (col == 7) {
					generateMove(
						row,
						col,
						1,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						6,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 0 | col = 6
				else {
					generateMove(
						row,
						col,
						1,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}

			// row = 0 | 1 < col < 6
			else {
				generateMove(
					row,
					col,
					1,
					col - 2,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					2,
					col - 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					2,
					col + 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					1,
					col + 2,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}
		else {
			if (col <= 1) {

				// row = 1 | col = 0
				if (col == 0) {
					generateMove(
						row,
						col,
						0,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						3,
						1,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 1 | col = 1
				else {
					generateMove(
						row,
						col,
						0,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						3,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						3,
						0,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}
			else if (col >= 6) {

				// row = 1 | col = 7
				if (col == 7) {
					generateMove(
						row,
						col,
						0,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					); generateMove(
						row,
						col,
						3,
						6,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 1 | col = 6
				else {
					generateMove(
						row,
						col,
						0,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						2,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						3,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						3,
						7,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}

			// row = 1 | 1 < col < 6
			else {
				unsigned char downOne = row + 1;
				unsigned char downTwo = row + 2;
				unsigned char upOne = row - 1;
				unsigned char rightTwo = col + 2;
				unsigned char leftTwo = col - 2;
				generateMove(
					row,
					col,
					downOne,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					col - 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					col + 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downOne,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upOne,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upOne,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}
	}
	else if (row >= 6) {
		if (row == 7) {
			if (col <= 1) {

				// row = 7 | col = 0
				if (col == 0) {
					generateMove(
						row,
						col,
						5,
						1,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					); generateMove(
						row,
						col,
						6,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 7 | col = 1
				else {
					generateMove(
						row,
						col,
						5,
						0,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						6,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}
			else if (col >= 6) {

				// row = 7 | col = 7
				if (col == 7) {
					generateMove(
						row,
						col,
						5,
						6,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						6,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 7 | col = 6
				else {
					generateMove(
						row,
						col,
						5,
						7,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						6,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}

			// row = 7 | 1 < col < 6
			else {
			unsigned char upOne = row - 1;
			unsigned char upTwo = row - 2;
				generateMove(
					row,
					col,
					upOne,
					col - 2,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					col - 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					col + 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upOne,
					col + 2,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}
		else {
			if (col <= 1) {

				// row = 6 | col = 0
				if (col == 0) {
					generateMove(
						row,
						col,
						4,
						1,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						7,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 6 | col = 1
				else {
					generateMove(
						row,
						col,
						4,
						0,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						4,
						2,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						7,
						3,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}
			else if (col >= 6) {

				// row = 6 | col = 7
				if (col == 7) {
					generateMove(
						row,
						col,
						4,
						6,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						7,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}

				// row = 6 | col = 6
				else {
					generateMove(
						row,
						col,
						7,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						4,
						5,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						5,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
					generateMove(
						row,
						col,
						7,
						4,
						pseudoLegalMoves,
						pinnedPieces,
						kingRow,
						kingCol
					);
				}
			}

			// row = 6 | 1 < col < 6
			else {
			unsigned char upOne = row - 1;
			unsigned char upTwo = row - 2;
			unsigned char downOne = row + 1;
			unsigned char rightTwo = col + 2;
			unsigned char leftTwo = col - 2;
				generateMove(
					row,
					col,
					downOne,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upOne,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					col - 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					col + 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upOne,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downOne,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}
	}
	else {
		if (col <= 1) {

			// 1 < row < 6 | col = 0
			if (col == 0) {
				unsigned char rightOne = col + 1;
				unsigned char rightTwo = col + 2;
				generateMove(
					row,
					col,
					row - 2,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row - 1,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 1,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 2,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}

			// 1 < row < 6 | col = 1
			else {
				unsigned char leftOne = col - 1;
				unsigned char rightOne = col + 1;
				unsigned char rightTwo = col + 2;
				unsigned char upTwo = row - 2;
				unsigned char downTwo = row + 2;
				generateMove(
					row,
					col,
					upTwo,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row - 1,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 1,
					rightTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}
		else if (col >= 6) {

			// 1 < row < 6 | col = 7
			if (col == 7) {
				unsigned char leftOne = col - 1;
				unsigned char leftTwo = col - 2;
				generateMove(
					row,
					col,
					row - 2,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row - 1,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 1,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 2,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}

			// 1 < row < 6 | col = 6
			else {
				unsigned char rightOne = col + 1;
				unsigned char leftOne = col - 1;
				unsigned char leftTwo = col - 2;
				unsigned char upTwo = row - 2;
				unsigned char downTwo = row + 2;
				generateMove(
					row,
					col,
					upTwo,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					upTwo,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row - 1,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					row + 1,
					leftTwo,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					leftOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
				generateMove(
					row,
					col,
					downTwo,
					rightOne,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol
				);
			}
		}

		// 1 < row < 6 | 1 < col < 6
		else {
			unsigned char rightOne = col + 1;
			unsigned char rightTwo = col + 2;
			unsigned char leftOne = col - 1;
			unsigned char leftTwo = col - 2;
			unsigned char upOne = row - 1;
			unsigned char upTwo = row - 2;
			unsigned char downOne = row + 1;
			unsigned char downTwo = row + 2;
			generateMove(
				row,
				col,
				upTwo,
				leftOne,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				upTwo,
				rightOne,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				upOne,
				rightTwo,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				downOne,
				rightTwo,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				downTwo,
				rightOne,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				downTwo,
				leftOne,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				downOne,
				leftTwo,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
			generateMove(
				row,
				col,
				upOne,
				leftTwo,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
		}
	}
}

// generates moves for rooks
void Position::generateRookMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	unsigned char bottomMax = 7 - row;
	unsigned char rightMax = 7 - col;

	// search squares above
	for (unsigned char top = 1; top <= row; top++) {
		unsigned char rowIndex = row - top;
		char square = board[rowIndex][col];
		if (!generateMove(
			row,
			col,
			rowIndex,
			col,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	// search squares below
	for (unsigned char bottom = 1; bottom <= bottomMax; bottom++) {
		unsigned char rowIndex = row + bottom;
		char square = board[rowIndex][col];
		if (!generateMove(
			row,
			col,
			rowIndex,
			col,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	// search squares to the right
	for (unsigned char right = 1; right <= rightMax; right++) {
		unsigned char colIndex = col + right;
		char square = board[row][colIndex];
		if (!generateMove(
			row,
			col,
			row,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}

	// search squares to the left
	for (unsigned char left = 1; left <= col; left++) {
		unsigned char colIndex = col - left;
		char square = board[row][colIndex];
		if (!generateMove(
			row,
			col,
			row,
			colIndex,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		)) {
			break;
		}
		if (square != '-') {
			break;
		}
	}
}

// generates moves for pawns
void Position::generatePawnMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	signed char direction = whiteMove ? -1 : 1;
	unsigned char rowAndDirection = row + direction;
	unsigned char rightCol = col + 1;
	unsigned char leftCol = col - 1;
	unsigned char frontSquare = board[rowAndDirection][col];

	// capture right
	if (
		col <= 6 &&
		(whiteMove ? islower(board[rowAndDirection][rightCol]) : isupper(board[rowAndDirection][rightCol]))
	) {
		// promotion
		if (row == (whiteMove ? 1 : 6)) {
			generateMove(
				row,
				col,
				rowAndDirection,
				rightCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'Q' : 'q'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				rightCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'B' : 'b'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				rightCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'N' : 'n'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				rightCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'R' : 'r'
			);
		}
		else {
			generateMove(
				row,
				col,
				rowAndDirection,
				rightCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
		}
	}

	// capture left
	if (
		col >= 1 &&
		(whiteMove ? islower(board[rowAndDirection][leftCol]) : isupper(board[rowAndDirection][leftCol]))
	) {

		// promotion
		if (row == (whiteMove ? 1 : 6)) {
			generateMove(
				row,
				col,
				rowAndDirection,
				leftCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'Q' : 'q'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				leftCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'B' : 'b'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				leftCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'N' : 'n'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				leftCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'R' : 'r'
			);
		}
		else {
			generateMove(
				row,
				col,
				rowAndDirection,
				leftCol,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
		}
	}

	// normal forward move
	if (frontSquare == '-') {

		// promotion
		if (row == (whiteMove ? 1 : 6)) {
			generateMove(
				row,
				col,
				rowAndDirection,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'Q' : 'q'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'B' : 'b'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'N' : 'n'
			);
			generateMove(
				row,
				col,
				rowAndDirection,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol,
				whiteMove ? 'R' : 'r'
			);
		}
		else {
			generateMove(
				row,
				col,
				rowAndDirection,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			);
		}
	}

	// move 2 squares on first move
	if (
		row == (whiteMove ? 6 : 1) &&
		frontSquare == '-' &&
		board[rowAndDirection + direction][col] == '-'
	) {
		generateMove(
			row,
			col,
			rowAndDirection + direction,
			col,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}

	// en passant
	if (ep != "-") {
		unsigned char epCol = ep.at(0) - 97;
		if (rowAndDirection == 56 - ep.at(1)) {
			if (rightCol == epCol) {
				generateMove(
					row,
					col,
					rowAndDirection,
					rightCol,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol,
					'-',
					true
				);
			}
			else if (leftCol == epCol) {
				generateMove(
					row,
					col,
					rowAndDirection,
					leftCol,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol,
					'-',
					true
				);
			}
		}
	}
}

// returns true if all 3 points are on the same line
bool Position::onLine(
	unsigned char anchorRow,
	unsigned char anchorCol,
	unsigned char pointOneRow,
	unsigned char pointOneCol,
	unsigned char pointTwoRow,
	unsigned char pointTwoCol
) {
	signed char startRowDist = pointOneRow - anchorRow;
	signed char startColDist = pointOneCol - anchorCol;
	signed char endRowDist = pointTwoRow - anchorRow;
	signed char endColDist = pointTwoCol - anchorCol;

	return min(startRowDist, startColDist) / double(max(startRowDist, startColDist)) == min(endRowDist, endColDist) / double(max(endRowDist, endColDist));
}

// returns true if the number is strictly between lower and upper
bool Position::isBetween(
	unsigned char number,
	unsigned char lower,
	unsigned char upper
) {
	return (number < upper && number > lower);
}

// returns a char representing a square on the board
unsigned char Position::rowColToChar(
	unsigned char row,
	unsigned char col
) {
	return (row * 8) + col;
}

#pragma endregion