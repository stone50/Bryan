#include <iostream>
#include "Position.h"

using namespace std;

#pragma region constructors

Position::Position() {}

Position::Position(string FEN) {
	setToFEN(FEN);
}

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

Position Position::StartingPosition() {
	return Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

#pragma region general functions

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
	out += " " + castle + " " + ep + " " + to_string(fiftyMoveRule) +" " + to_string(moveCount);
	return out;
}

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

vector<string> Position::legalMoves() {
	vector<string> moves;
	unsigned char check = 64;
	bool doubleCheck = false;
	unsigned char kingRow;
	unsigned char kingCol;

#pragma region generate attacked squares

	unordered_set<unsigned char> kingDangerSquares;
	unordered_set<unsigned char> pinnedPieces;
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

#pragma region generate king moves

	if (
		kingRow >= 1 &&
		(whiteMove ? !isupper(board[kingRow - 1][kingCol]) : !islower(board[kingRow - 1][kingCol])) &&
		kingDangerSquares.count(((kingRow - 1) * 8) + kingCol) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow - 1,
			kingCol,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingRow <= 6 &&
		(whiteMove ? !isupper(board[kingRow + 1][kingCol]) : !islower(board[kingRow + 1][kingCol])) &&
		kingDangerSquares.count(((kingRow + 1) * 8) + kingCol) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow + 1,
			kingCol,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingCol <= 6 &&
		(whiteMove ? !isupper(board[kingRow][kingCol + 1]) : !islower(board[kingRow][kingCol + 1])) &&
		kingDangerSquares.count((kingRow * 8) + kingCol + 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow,
			kingCol + 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingCol >= 1 &&
		(whiteMove ? !isupper(board[kingRow][kingCol - 1]) : !islower(board[kingRow][kingCol - 1])) &&
		kingDangerSquares.count((kingRow * 8) + kingCol - 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow,
			kingCol - 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingRow >= 1 &&
		kingCol <= 6 &&
		(whiteMove ? !isupper(board[kingRow - 1][kingCol + 1]) : !islower(board[kingRow - 1][kingCol + 1])) &&
		kingDangerSquares.count(((kingRow - 1) * 8) + kingCol + 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow - 1,
			kingCol + 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingRow >= 1 &&
		kingCol >= 1 &&
		(whiteMove ? !isupper(board[kingRow - 1][kingCol - 1]) : !islower(board[kingRow - 1][kingCol - 1])) &&
		kingDangerSquares.count(((kingRow - 1) * 8) + kingCol - 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow - 1,
			kingCol - 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingRow <= 6 &&
		kingCol <= 6 &&
		(whiteMove ? !isupper(board[kingRow + 1][kingCol + 1]) : !islower(board[kingRow + 1][kingCol + 1])) &&
		kingDangerSquares.count(((kingRow + 1) * 8) + kingCol + 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow + 1,
			kingCol + 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		kingRow <= 6 &&
		kingCol >= 1 &&
		(whiteMove ? !isupper(board[kingRow + 1][kingCol - 1]) : !islower(board[kingRow + 1][kingCol - 1])) &&
		kingDangerSquares.count(((kingRow + 1) * 8) + kingCol - 1) == 0
	) {
		generateMove(
			kingRow,
			kingCol,
			kingRow + 1,
			kingCol - 1,
			&moves,
			&pinnedPieces,
			kingRow,
			kingCol
		);
	}

#pragma endregion

	if (doubleCheck) {
		//return moves;
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

	if (check < 64) {
		unsigned char attackerRow = check / 8;
		unsigned char attackerCol = check % 8;
		if (board[attackerRow][attackerCol] != (whiteMove ? 'n' : 'N')) {
				//check if attacker can be blocked
				//notes:
				//pinned pieces cannot block check
				//en passant cannot block check
				//account for pawns moving 2 squares on their first move

		}

		//check if attacker can be captured
		//notes:
		//en passant is accounted for in pseudo legal move generation
		//
	}
	else {

	}

	for (int i = 0; i < pseudoLegalMoves.size(); i++) {
		moves.push_back(pseudoLegalMoves.at(i));
	}

	cout << "Possible moves:\n";
	for (unsigned int i = 0; i < moves.size(); i++) {
		cout << translateMove(moves.at(i)) << endl;
	}

	return moves;
}

void Position::printBoard() {
	cout << "\n-------------------\n";
	for (unsigned char row = 0; row < 8; row++) {
		cout << "| ";
		for (unsigned char col = 0; col < 8; col++) {
			cout << board[row][col] << " ";
		}
		cout << "|\n";
	}
	cout << "-------------------\n\n";
}

#pragma endregion

#pragma region helper functions


void Position::kingAttack(
	unsigned char row,
	unsigned char col,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (row >= 1) {
		kingDangerSquares->insert(((row - 1) * 8) + col);
	}
	if (row <= 6) {
		kingDangerSquares->insert(((row + 1) * 8) + col);
	}
	if (col <= 6) {
		kingDangerSquares->insert((row * 8) + col + 1);
	}
	if (col >= 1) {
		kingDangerSquares->insert((row * 8) + col - 1);
	}
	if (row >= 1 && col <= 6) {
		kingDangerSquares->insert(((row - 1) * 8) + col + 1);
	}
	if (row >= 1 && col >= 1) {
		kingDangerSquares->insert(((row - 1) * 8) + col - 1);
	}
	if (row <= 6 && col <= 6) {
		kingDangerSquares->insert(((row + 1) * 8) + col + 1);
	}
	if (row <= 6 && col >= 1) {
		kingDangerSquares->insert(((row + 1) * 8) + col - 1);
	}
}

void Position::bishopAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares,
	unordered_set<unsigned char>* pinnedPieces
) {
	unsigned char pin = 64;
	for (unsigned char topLeft = 1; row - topLeft >= 0 && col - topLeft >= 0; topLeft++) {
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
	for (unsigned char topRight = 1; row - topRight >= 0 && col + topRight < 8; topRight++) {
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
	for (unsigned char bottomLeft = 1; row + bottomLeft < 8 && col - bottomLeft >= 0; bottomLeft++) {
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
	for (unsigned char bottomRight = 1; row + bottomRight < 8 && col + bottomRight < 8; bottomRight++) {
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

void Position::knightAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares
) {
	if (row >= 2 && col >= 1) {
		nonSliderAttack(
			row,
			col,
			row - 2,
			col - 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row >= 2 && col <= 6) {
		nonSliderAttack(
			row,
			col,
			row - 2,
			col + 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row >= 1 && col <= 5) {
		nonSliderAttack(
			row,
			col,
			row - 1,
			col + 2,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row <= 6 && col <= 5) {
		nonSliderAttack(
			row,
			col,
			row + 1,
			col + 2,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row <= 5 && col <= 6) {
		nonSliderAttack(
			row,
			col,
			row + 2,
			col + 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row <= 5 && col >= 1) {
		nonSliderAttack(
			row,
			col,
			row + 2,
			col - 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row <= 6 && col >= 2) {
		nonSliderAttack(
			row,
			col,
			row + 1,
			col - 2,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (row >= 1 && col >= 2) {
		nonSliderAttack(
			row,
			col,
			row - 1,
			col - 2,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
}

void Position::rookAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares,
	unordered_set<unsigned char>* pinnedPieces
) {
	unsigned char pin = 64;
	for (unsigned char top = 1; row - top >= 0; top++) {
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
	for (unsigned char bottom = 1; row + bottom < 8; bottom++) {
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
	for (unsigned char right = 1; col + right < 8; right++) {
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
	for (unsigned char left = 1; col - left >= 0; left++) {
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

void Position::pawnAttack(
	unsigned char row,
	unsigned char col,
	unsigned char* check,
	bool* doubleCheck,
	unordered_set<unsigned char>* kingDangerSquares
) {
	signed char direction = ((islower(board[row][col]) > 0) * 2) - 1;
	if (col <= 6) {
		nonSliderAttack(
			row,
			col,
			row + direction,
			col + 1,
			check,
			doubleCheck,
			kingDangerSquares
		);
	}
	if (col >= 1) {
		nonSliderAttack(
row,
col,
row + direction,
col - 1,
check,
doubleCheck,
kingDangerSquares
);
	}
}

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
	if (*pin < 64) {
		if (square == (whiteMove ? 'K' : 'k')) {
			pinnedPieces->insert(*pin);
			return true;
		}
		else if (square != '-') {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if (square == '-') {
			kingDangerSquares->insert((attackRow * 8) + attackCol);
			return false;
		}
		else if (square == (whiteMove ? 'K' : 'k')) {
			if (*check < 64) {
				*doubleCheck = true;
				return false;
			}
			else {
				*check = (pieceRow * 8) + pieceCol;
				kingDangerSquares->insert((attackRow * 8) + attackCol);
				return false;
			}
		}
		else {
			if (whiteMove ? islower(square) : isupper(square)) {
				kingDangerSquares->insert((attackRow * 8) + attackCol);
				return true;
			}
			else {
				*pin = (attackRow * 8 + attackCol);
				return false;
			}
		}
	}
}

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
			*check = (pieceRow * 8) + pieceCol;
			kingDangerSquares->insert((attackRow * 8) + attackCol);
		}
	}
	else {
		kingDangerSquares->insert((attackRow * 8) + attackCol);
	}
}

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
	if (pinnedPieces->count((startRow * 8) + startCol)) {
		signed char startRowDist = startRow - kingRow;
		signed char startColDist = startCol - kingCol;
		signed char endRowDist = endRow - kingRow;
		signed char endColDist = endCol - kingCol;

		if (min(startRowDist, startColDist) / double(max(startRowDist, startColDist)) != min(endRowDist, endColDist) / double(max(endRowDist, endColDist))) {
			return false;
		}
	}
	string move = "";
	move += startRow;
	move += startCol;
	move += endRow;
	move += endCol;
	if (promote != '-') {
		move += promote;
	}
	else if (ep) {
		move += 'e';
	}
	moves->push_back(move);
	return true;
}

void Position::generateBishopMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	for (unsigned char topLeft = 1; row - topLeft >= 0 && col - topLeft >= 0; topLeft++) {
		char square = board[row - topLeft][col - topLeft];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row - topLeft,
				col - topLeft,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char topRight = 1; row - topRight >= 0 && col + topRight < 8; topRight++) {
		char square = board[row - topRight][col + topRight];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row - topRight,
				col + topRight,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char bottomLeft = 1; row + bottomLeft < 8 && col - bottomLeft >= 0; bottomLeft++) {
		char square = board[row + bottomLeft][col - bottomLeft];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row + bottomLeft,
				col - bottomLeft,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char bottomRight = 1; row + bottomRight < 8 && col + bottomRight < 8; bottomRight++) {
		char square = board[row + bottomRight][col + bottomRight];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row + bottomRight,
				col + bottomRight,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
}

void Position::generateKnightMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	if (
		row >= 2 &&
		col >= 1 &&
		!(whiteMove ? isupper(board[row - 2][col - 1]) : islower(board[row - 2][col - 1]))
	) {
		generateMove(
			row,
			col,
			row - 2,
			col - 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row >= 2 &&
		col <= 6 &&
		!(whiteMove ? isupper(board[row - 2][col + 1]) : islower(board[row - 2][col + 1]))
	) {
		generateMove(
			row,
			col,
			row - 2,
			col + 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row >= 1 &&
		col <= 5 &&
		!(whiteMove ? isupper(board[row - 1][col + 2]) : islower(board[row - 1][col + 2]))
	) {
		generateMove(
			row,
			col,
			row - 1,
			col + 2,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row <= 6 &&
		col <= 5 &&
		!(whiteMove ? isupper(board[row + 1][col + 2]) : islower(board[row + 1][col + 2]))
	) {
		generateMove(
			row,
			col,
			row + 1,
			col + 2,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row <= 5 &&
		col <= 6 &&
		!(whiteMove ? isupper(board[row + 2][col + 1]) : islower(board[row + 2][col + 1]))
	) {
		generateMove(
			row,
			col,
			row + 2,
			col + 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row <= 5 &&
		col >= 1 &&
		!(whiteMove ? isupper(board[row + 2][col - 1]) : islower(board[row + 2][col - 1]))
	) {
		generateMove(
			row,
			col,
			row + 2,
			col - 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row <= 6 &&
		col >= 2 &&
		!(whiteMove ? isupper(board[row + 1][col - 2]) : islower(board[row + 1][col - 2]))
	) {
		generateMove(
			row,
			col,
			row + 1,
			col - 2,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row >= 1 &&
		col >= 2 &&
		!(whiteMove ? isupper(board[row - 1][col - 2]) : islower(board[row - 1][col - 2]))
	) {
		generateMove(
			row,
			col,
			row - 1,
			col - 2,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
}

void Position::generateRookMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	for (unsigned char top = 1; row - top >= 0; top++) {
		char square = board[row - top][col];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row - top,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char bottom = 1; row + bottom < 8; bottom++) {
		char square = board[row + bottom][col];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row + bottom,
				col,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char right = 1; col + right < 8; right++) {
		char square = board[row][col + right];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row,
				col + right,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
	for (unsigned char left = 1; col - left >= 0; left++) {
		char square = board[row][col - left];
		if (!(whiteMove ? isupper(square) : islower(square))) {
			if (!generateMove(
				row,
				col,
				row,
				col - left,
				pseudoLegalMoves,
				pinnedPieces,
				kingRow,
				kingCol
			)) {
				break;
			}
		}
		if (square != '-') {
			break;
		}
	}
}

void Position::generatePawnMoves(
	unsigned char row,
	unsigned char col,
	vector<string>* pseudoLegalMoves,
	unordered_set<unsigned char>* pinnedPieces,
	unsigned char kingRow,
	unsigned char kingCol
) {
	signed char direction = ((islower(board[row][col]) > 0) * 2) - 1;
	if (
		col <= 6 &&
		(whiteMove ? islower(board[row + direction][col + 1]) : isupper(board[row + direction][col + 1]))
		) {
		generateMove(
			row,
			col,
			row + direction,
			col + 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		col >= 1 &&
		(whiteMove ? islower(board[row + direction][col - 1]) : isupper(board[row + direction][col - 1]))
		) {
		generateMove(
			row,
			col,
			row + direction,
			col - 1,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (board[row + direction][col] == '-') {
		generateMove(
			row,
			col,
			row + direction,
			col,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (
		row == (whiteMove ? 6 : 1) &&
		board[row + direction][col] == '-' &&
		board[row + direction + direction][col] == '-'
	) {
		generateMove(
			row,
			col,
			row + direction + direction,
			col,
			pseudoLegalMoves,
			pinnedPieces,
			kingRow,
			kingCol
		);
	}
	if (ep != "-") {
		signed char epSquare = ((56 - ep.at(1)) * 8) + (ep.at(0) - 97);
		unsigned char epRow = 56 - ep.at(1);
		unsigned char epCol = ep.at(0) - 97;
		if (row + direction == epRow) {
			if (col + 1 == epCol) {
				generateMove(
					row,
					col,
					row + direction,
					col + 1,
					pseudoLegalMoves,
					pinnedPieces,
					kingRow,
					kingCol,
					'-',
					true
				);
			}
			else if (col - 1 == epCol) {
				generateMove(
					row,
					col,
					row + direction,
					col - 1,
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

string Position::translateMove(string move) {
	string out = "";
	for (unsigned char i = 0; i < move.size(); i++) {
		out += to_string(move.at(i));
	}
	return out;
}

#pragma endregion