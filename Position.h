#pragma once

#include <string>
#include <vector>

class Position {
	char layout[8][8] = {	// board layout as a 2d array
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'},
			{'-', '-', '-', '-', '-', '-', '-', '-'}
	};
	bool whiteMove = true; // true if it is white's turn
	std::string castle = "-"; // available castle moves
	std::string ep = "-"; // en passant square || is "-" if no en passant
	int fiftyMoveRule = 0; // counting the number of moves without a pawn move or a capture
	int moveCount = 1; // move number
	std::string FEN = "8/8/8/8/8/8/8/8 w - - 0 1"; // FEN
	std::vector<std::string> legalMoves = std::vector<std::string>(); // list of legal moves -- note: generateLegalMoves() must be called to set this variable
	float SE = 0; // on the spot evaluation
	bool inCheck = false; // whether or not the king is in check -- note: inCheck() must be called to set this variable
	std::string bestMove = ""; // what Bryan considers to be the best move
	float bestEval = 0; // position evaluation after analysis

public:
	Position();

	Position(std::string tFEN);

	Position(char tlayout[8][8], bool twhiteMove, std::string tcastle, std::string tep, int tfiftyMoveRule, int tmoveCount);

	// generates an FEN based on the board position
	std::string FEN() {

		// adds the piece layout
		std::string out = "";
		for (int row = 0; row != 8; row += 1) {
			int count = 0;
			for (int col = 0; col != 8; col += 1) {
				char thisSquare = layout[row][col];
				if (thisSquare == '-') {
					count++;
				}
				else {
					if (count != 0) {
						out += std::to_string(count);
						count = 0;
					}
					out += thisSquare;
				}
			}
			if (count != 0) {
				out += std::to_string(count);
			}
			out += "/";
		}
		out = out.substr(0, out.length() - 1);

		// adds a 'w' or 'b' depending on whose move it is
		out += " ";
		if (whiteMove) {
			out += "w";
		}
		else {
			out += "b";
		}

		// adds the castle moves, en passant move, fifty move rule counter and overall
		// move counter
		out += " " + castle + " " + ep + " " + std::to_string(fiftyMoveRule) + " " + std::to_string(moveCount);
		FEN = out;
		return out;
	}

	// creates a board position based on the FEN
	void layout() {
		int row = 0;
		int col = 0;
		int count = 0;
		String item = FEN.substring(0, 1);

		// sets the board layout
		while (!item.equals(" ")) {
			if (Character.isDigit(item.charAt(0))) {
				int num = Integer.parseInt(FEN.substring(count, count + 1));
				for (int i = 0; i < num; i++) {
					layout[row][col + i] = '-';
				}
				col += num;
			}
			else if (item.equals("/")) {
				row++;
				col = 0;
			}
			else {
				layout[row][col] = item.charAt(0);
				col++;
			}
			count++;
			item = FEN.substring(count, count + 1);
		}

		// sets whose move it is
		count++;
		item = FEN.substring(count, count + 1);
		whiteMove = item.equals("w");

		// sets the available castle moves
		count += 2;
		int num = count;
		while (!FEN.substring(num, num + 1).equals(" ")) {
			num++;
		}
		castle = FEN.substring(count, num);
		count = num + 1;

		// sets the en passant square
		if (FEN.substring(count, count + 1).equals("-")) {
			ep = "-";
		}
		else {
			ep = FEN.substring(count, count + 2);
			count++;
		}
		count += 2;

		// sets the fifty move rule
		num = count;
		while (!FEN.substring(num, num + 1).equals(" ")) {
			num++;
		}
		fiftyMoveRule = Integer.parseInt(FEN.substring(count, num));
		count = num + 1;

		// sets the move count
		moveCount = Integer.parseInt(FEN.substring(count, FEN.length()));
	}

	void generateLegalMoves() {

		// generate pseudo legal moves
		int i = 1;
		// boolean exit = false;
		if (whiteMove) {
			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					switch (layout[row][col]) {

						// white pawn
					case 'P':
						if (layout[row - 1][col] == '-') {
							if (row == 1) {
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col)+"N");
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col)+"B");
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col)+"R");
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col)+"Q");
							}
							else {
								if (row == 6 && layout[row - 2][col] == '-') {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col));
								}
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col));
							}
						}
						if (col > 0) {
							if (layout[row - 1][col - 1] != '-' && Character.isLowerCase(layout[row - 1][col - 1])
								&& layout[row - 1][col - 1] != 'k') {
								if (row == 1) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1) + "N");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1) + "B");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1) + "R");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1) + "Q");
								}
								else {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1));
								}
							}
						}
						if (col < 7) {
							if (layout[row - 1][col + 1] != '-' && Character.isLowerCase(layout[row - 1][col + 1])
								&& layout[row - 1][col + 1] != 'k') {
								if (row == 1) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1) + "N");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1) + "B");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1) + "R");
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1) + "Q");
								}
								else {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1));
								}
							}
						}
						break;

						// white knight
					case 'N':
						if (row > 1) {
							if (col > 1) {
								if (!Character.isUpperCase(layout[row - 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col - 1));
								}
								if (!Character.isUpperCase(layout[row - 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 2));
								}
							}
							else if (col > 0) {
								if (!Character.isUpperCase(layout[row - 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col - 1));
								}
							}
							if (col < 6) {
								if (!Character.isUpperCase(layout[row - 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col + 1));
								}
								if (!Character.isUpperCase(layout[row - 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 2));
								}
							}
							else if (col < 7) {
								if (!Character.isUpperCase(layout[row - 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col + 1));
								}
							}
						}
						else if (row > 0) {
							if (col > 1) {
								if (!Character.isUpperCase(layout[row - 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 2));
								}
							}
							if (col < 6) {
								if (!Character.isUpperCase(layout[row - 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 2));
								}
							}
						}
						if (row < 6) {
							if (col > 1) {
								if (!Character.isUpperCase(layout[row + 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 2));
								}
								if (!Character.isUpperCase(layout[row + 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col - 1));
								}
							}
							else if (col > 0) {
								if (!Character.isUpperCase(layout[row + 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col - 1));
								}
							}
							if (col < 6) {
								if (!Character.isUpperCase(layout[row + 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 2));
								}
								if (!Character.isUpperCase(layout[row + 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col + 1));
								}
							}
							else if (col < 7) {
								if (!Character.isUpperCase(layout[row + 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col + 1));
								}
							}
						}
						else if (row < 7) {
							if (col > 1) {
								if (!Character.isUpperCase(layout[row + 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 2));
								}
							}
							if (col < 6) {
								if (!Character.isUpperCase(layout[row + 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 2));
								}
							}
						}
						break;

						// white bishop
					case 'B':
						int count = Math.min(row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row - i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col - i));
							}
							if (layout[row - i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row - i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col + i));
							}
							if (layout[row - i][col + i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row + i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col - i));
							}
							if (layout[row + i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row + i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col + i));
							}
							if (layout[row + i][col + i] != '-') {
								break;
							}
							i++;
						}
						break;

						// white rook
					case 'R':
						i = 1;
						while (i <= 7 - row) {
							if (!Character.isUpperCase(layout[row + i][col])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col));
							}
							if (layout[row + i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= row) {
							if (!Character.isUpperCase(layout[row - i][col])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col));
							}
							if (layout[row - i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= 7 - col) {
							if (!Character.isUpperCase(layout[row][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + i));
							}
							if (layout[row][col + i] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= col) {
							if (!Character.isUpperCase(layout[row][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - i));
							}
							if (layout[row][col - i] != '-') {
								break;
							}
							i++;
						}
						break;

						// white queen
					case 'Q':
						i = 1;
						while (i <= 7 - row) {
							if (!Character.isUpperCase(layout[row + i][col])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col));
							}
							if (layout[row + i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= row) {
							if (!Character.isUpperCase(layout[row - i][col])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col));
							}
							if (layout[row - i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= 7 - col) {
							if (!Character.isUpperCase(layout[row][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + i));
							}
							if (layout[row][col + i] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= col) {
							if (!Character.isUpperCase(layout[row][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - i));
							}
							if (layout[row][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row - i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col - i));
							}
							if (layout[row - i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row - i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col + i));
							}
							if (layout[row - i][col + i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row + i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col - i));
							}
							if (layout[row + i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isUpperCase(layout[row + i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col + i));
							}
							if (layout[row + i][col + i] != '-') {
								break;
							}
							i++;
						}
						break;

						// white king
					case 'K':
						if (row > 0) {
							if (!Character.isUpperCase(layout[row - 1][col])) {
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col));
							}
							if (col > 0) {
								if (!Character.isUpperCase(layout[row - 1][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1));
								}
							}
							if (col < 7) {
								if (!Character.isUpperCase(layout[row - 1][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1));
								}
							}
						}
						if (row < 7) {
							if (!Character.isUpperCase(layout[row + 1][col])) {
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col));
							}
							if (col > 0) {
								if (!Character.isUpperCase(layout[row + 1][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1));
								}
							}
							if (col < 7) {
								if (!Character.isUpperCase(layout[row + 1][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1));
								}
							}
						}
						if (col > 0) {
							if (!Character.isUpperCase(layout[row][col - 1])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - 1));
							}
						}
						if (col < 7) {
							if (!Character.isUpperCase(layout[row][col + 1])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + 1));
							}
						}
						break;

					}
				}
			}
		}
		else {

			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					switch (layout[row][col]) {

						// black pawn
					case 'p':
						if (layout[row + 1][col] == '-') {
							if (row == 6) {
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col)+"n");
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col)+"b");
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col)+"r");
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col)+"q");
							}
							else {
								if (row == 1 && layout[row + 2][col] == '-') {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col));
								}
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col));
							}
						}
						if (col > 0) {
							if (layout[row + 1][col - 1] != '-' && Character.isUpperCase(layout[row + 1][col - 1])
								&& layout[row + 1][col - 1] != 'K') {
								if (row == 6) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1) + "n");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1) + "b");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1) + "r");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1) + "q");
								}
								else {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1));
								}
							}
						}
						if (col < 7) {
							if (layout[row + 1][col + 1] != '-' && Character.isUpperCase(layout[row + 1][col + 1])
								&& layout[row + 1][col + 1] != 'K') {
								if (row == 6) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1) + "n");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1) + "b");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1) + "r");
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1) + "q");
								}
								else {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1));
								}
							}
						}
						break;

						// black knight
					case 'n':
						if (row > 1) {
							if (col > 1) {
								if (!Character.isLowerCase(layout[row - 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col - 1));
								}
								if (!Character.isLowerCase(layout[row - 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 2));
								}
							}
							else if (col > 0) {
								if (!Character.isLowerCase(layout[row - 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col - 1));
								}
							}
							if (col < 6) {
								if (!Character.isLowerCase(layout[row - 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col + 1));
								}
								if (!Character.isLowerCase(layout[row - 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 2));
								}
							}
							else if (col < 7) {
								if (!Character.isLowerCase(layout[row - 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 2) + (col + 1));
								}
							}
						}
						else if (row > 0) {
							if (col > 1) {
								if (!Character.isLowerCase(layout[row - 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 2));
								}
							}
							if (col < 6) {
								if (!Character.isLowerCase(layout[row - 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 2));
								}
							}
						}
						if (row < 6) {
							if (col > 1) {
								if (!Character.isLowerCase(layout[row + 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 2));
								}
								if (!Character.isLowerCase(layout[row + 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col - 1));
								}
							}
							else if (col > 0) {
								if (!Character.isLowerCase(layout[row + 2][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col - 1));
								}
							}
							if (col < 6) {
								if (!Character.isLowerCase(layout[row + 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 2));
								}
								if (!Character.isLowerCase(layout[row + 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col + 1));
								}
							}
							else if (col < 7) {
								if (!Character.isLowerCase(layout[row + 2][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 2) + (col + 1));
								}
							}
						}
						else if (row < 7) {
							if (col > 1) {
								if (!Character.isLowerCase(layout[row + 1][col - 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 2));
								}
							}
							if (col < 6) {
								if (!Character.isLowerCase(layout[row + 1][col + 2])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 2));
								}
							}
						}
						break;

						// black bishop
					case 'b':
						int count = Math.min(row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row - i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col - i));
							}
							if (layout[row - i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row - i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col + i));
							}
							if (layout[row - i][col + i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row + i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col - i));
							}
							if (layout[row + i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row + i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col + i));
							}
							if (layout[row + i][col + i] != '-') {
								break;
							}
							i++;
						}
						break;

						// black rook
					case 'r':
						i = 1;
						while (i <= 7 - row) {
							if (!Character.isLowerCase(layout[row + i][col])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col));
							}
							if (layout[row + i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= row) {
							if (!Character.isLowerCase(layout[row - i][col])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col));
							}
							if (layout[row - i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= 7 - col) {
							if (!Character.isLowerCase(layout[row][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + i));
							}
							if (layout[row][col + i] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= col) {
							if (!Character.isLowerCase(layout[row][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - i));
							}
							if (layout[row][col - i] != '-') {
								break;
							}
							i++;
						}
						break;

						// black queen
					case 'q':
						i = 1;
						while (i <= 7 - row) {
							if (!Character.isLowerCase(layout[row + i][col])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col));
							}
							if (layout[row + i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= row) {
							if (!Character.isLowerCase(layout[row - i][col])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col));
							}
							if (layout[row - i][col] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= 7 - col) {
							if (!Character.isLowerCase(layout[row][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + i));
							}
							if (layout[row][col + i] != '-') {
								break;
							}
							i++;
						}
						i = 1;
						while (i <= col) {
							if (!Character.isLowerCase(layout[row][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - i));
							}
							if (layout[row][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row - i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col - i));
							}
							if (layout[row - i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row - i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row - i) + (col + i));
							}
							if (layout[row - i][col + i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row + i][col - i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col - i));
							}
							if (layout[row + i][col - i] != '-') {
								break;
							}
							i++;
						}
						count = Math.min(7 - row, 7 - col);
						i = 1;
						while (i <= count) {
							if (!Character.isLowerCase(layout[row + i][col + i])) {
								legalMoves.add(row + "" + col + "-" + (row + i) + (col + i));
							}
							if (layout[row + i][col + i] != '-') {
								break;
							}
							i++;
						}
						break;

						// black king
					case 'k':
						if (row > 0) {
							if (!Character.isLowerCase(layout[row - 1][col])) {
								legalMoves.add(row + "" + col + "-" + (row - 1) + (col));
							}
							if (col > 0) {
								if (!Character.isLowerCase(layout[row - 1][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col - 1));
								}
							}
							if (col < 7) {
								if (!Character.isLowerCase(layout[row - 1][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row - 1) + (col + 1));
								}
							}
						}
						if (row < 7) {
							if (!Character.isLowerCase(layout[row + 1][col])) {
								legalMoves.add(row + "" + col + "-" + (row + 1) + (col));
							}
							if (col > 0) {
								if (!Character.isLowerCase(layout[row + 1][col - 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col - 1));
								}
							}
							if (col < 7) {
								if (!Character.isLowerCase(layout[row + 1][col + 1])) {
									legalMoves.add(row + "" + col + "-" + (row + 1) + (col + 1));
								}
							}
						}
						if (col > 0) {
							if (!Character.isLowerCase(layout[row][col - 1])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col - 1));
							}
						}
						if (col < 7) {
							if (!Character.isLowerCase(layout[row][col + 1])) {
								legalMoves.add(row + "" + col + "-" + (row)+(col + 1));
							}
						}
						break;
					}
				}
			}
		}

		// test for check
		char[][] test = new char[8][8];
		for (i = 0; i < legalMoves.size(); i++) {
			String move = legalMoves.get(i);
			int srow = Integer.parseInt(move.substring(0, 1));
			int scol = Integer.parseInt(move.substring(1, 2));
			int frow = Integer.parseInt(move.substring(3, 4));
			int fcol = Integer.parseInt(move.substring(4, 5));
			for (int index = 0; index < 8; index++) {
				System.arraycopy(layout[index], 0, test[index], 0, 8);
			}
			test[frow][fcol] = test[srow][scol];
			test[srow][scol] = '-';
			Position pos = new Position();
			pos.setLayout(test);
			pos.setWhiteMove(whiteMove);
			if (pos.inCheck()) {
				legalMoves.remove(i);
				i--;
			}
		}

		// test for special cases
		if (whiteMove) {

			// white castle king's side
			if (castle.contains("K")) {
				if (layout[7][5] == '-' && layout[7][6] == '-') {
					for (int index = 0; index < 8; index++) {
						System.arraycopy(layout[index], 0, test[index], 0, 8);
					}
					test[7][5] = 'K';
					test[7][4] = '-';
					Position pos = new Position();
					pos.setLayout(test);
					if (!pos.inCheck()) {
						test[7][6] = 'K';
						test[7][5] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add("O-O");
						}
					}
				}
			}

			// white castle queen's side
			if (castle.contains("Q")) {
				if (layout[7][3] == '-' && layout[7][2] == '-' && layout[7][1] == '-') {
					for (int index = 0; index < 8; index++) {
						System.arraycopy(layout[index], 0, test[index], 0, 8);
					}
					test[7][3] = 'K';
					test[7][4] = '-';
					Position pos = new Position();
					pos.setLayout(layout);
					if (!pos.inCheck()) {
						test[7][2] = 'K';
						test[7][3] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add("O-O-O");
						}
					}
				}
			}

			// white en passant
			if (!ep.equals("-")) {
				String alph = "abcdefgh";
				int col = alph.indexOf(ep.substring(0, 1));
				Position pos = new Position();
				for (int index = 0; index < 8; index++) {
					System.arraycopy(layout[index], 0, test[index], 0, 8);
				}
				if (col > 0) {
					if (layout[3][col - 1] == 'P') {
						test[2][col] = 'P';
						test[3][col] = '-';
						test[3][col - 1] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add((col - 1) + "-2" + col);
						}
					}
				}
				if (col < 7) {
					if (layout[3][col + 1] == 'P') {
						test[2][col] = 'P';
						test[3][col] = '-';
						test[3][col - 1] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add((col + 1) + "-2" + col);
						}
					}
				}
			}
		}
		else {
			// black castle king's side
			if (castle.contains("k")) {
				if (layout[0][5] == '-' && layout[0][6] == '-') {
					for (int index = 0; index < 8; index++) {
						System.arraycopy(layout[index], 0, test[index], 0, 8);
					}
					test[0][5] = 'k';
					test[0][4] = '-';
					Position pos = new Position();
					pos.setLayout(test);
					pos.setWhiteMove(false);
					if (!pos.inCheck()) {
						test[0][6] = 'k';
						test[0][5] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add("O-O");
						}
					}
				}
			}

			// black castle queen's side
			if (castle.contains("q")) {
				if (layout[0][3] == '-' && layout[0][2] == '-' && layout[0][1] == '-') {
					for (int index = 0; index < 8; index++) {
						System.arraycopy(layout[index], 0, test[index], 0, 8);
					}
					test[0][3] = 'k';
					test[0][4] = '-';
					Position pos = new Position();
					pos.setLayout(test);
					pos.setWhiteMove(false);
					if (!pos.inCheck()) {
						test[0][2] = 'k';
						test[0][3] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add("O-O-O");
						}
					}
				}
			}

			// black en passant
			if (!ep.equals("-")) {
				String alph = "abcdefgh";
				int col = alph.indexOf(ep.substring(0, 1));
				Position pos = new Position();
				for (int index = 0; index < 8; index++) {
					System.arraycopy(layout[index], 0, test[index], 0, 8);
				}
				if (col > 0) {
					if (layout[4][col - 1] == 'p') {
						test[5][col] = 'p';
						test[4][col] = '-';
						test[4][col - 1] = '-';
						pos.setLayout(test);
						pos.setWhiteMove(false);
						if (!pos.inCheck()) {
							legalMoves.add((col - 1) + "-5" + col);
						}
					}
				}
				if (col < 7) {
					if (layout[4][col + 1] == 'p') {
						test[5][col] = 'p';
						test[4][col] = '-';
						test[4][col - 1] = '-';
						pos.setLayout(test);
						if (!pos.inCheck()) {
							legalMoves.add((col + 1) + "-5" + col);
						}
					}
				}
			}
		}
	}

	boolean inCheck() {
		int row = 0;
		int col = 0;
		int i = -1;
		boolean exit = false;

		if (whiteMove) {

			// find the white king
			while (row < 8 && !exit) {
				while (col < 8 && !exit) {
					if (layout[row][col] == 'K') {
						exit = true;
					}
					else {
						col++;
					}
				}
				if (!exit) {
					row++;
					col = 0;
				}
			}

			// check for knight (white)
			if (row > 1) {
				if (col > 1) {
					if (layout[row - 2][col - 1] == 'n') {
						return true;
					}
					if (layout[row - 1][col - 2] == 'n') {
						return true;
					}
				}
				else if (col > 0) {
					if (layout[row - 2][col - 1] == 'n') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row - 2][col + 1] == 'n') {
						return true;
					}
					if (layout[row - 1][col + 2] == 'n') {
						return true;
					}
				}
				else if (col < 7) {
					if (layout[row - 2][col + 1] == 'n') {
						return true;
					}
				}
			}
			else if (row > 0) {
				if (col > 1) {
					if (layout[row - 1][col - 2] == 'n') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row - 1][col + 2] == 'n') {
						return true;
					}
				}
			}
			if (row < 6) {
				if (col > 1) {
					if (layout[row + 1][col - 2] == 'n') {
						return true;
					}
					if (layout[row + 2][col - 1] == 'n') {
						return true;
					}
				}
				else if (col > 0) {
					if (layout[row + 2][col - 1] == 'n') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row + 1][col + 2] == 'n') {
						return true;
					}
					if (layout[row + 2][col + 1] == 'n') {
						return true;
					}
				}
				else if (col < 7) {
					if (layout[row + 2][col + 1] == 'n') {
						return true;
					}
				}
			}
			else if (row < 7) {
				if (col > 1) {
					if (layout[row + 1][col - 2] == 'n') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row + 1][col + 2] == 'n') {
						return true;
					}
				}
			}

			// check for pawn (white)
			if (col > 0) {
				if (row > 0) {
					if (layout[row - 1][col - 1] == 'p') {
						return true;
					}
				}
			}
			if (col < 7) {
				if (row > 0) {
					if (layout[row - 1][col + 1] == 'p') {
						return true;
					}
				}
			}

			// check for bishop/queen (white)
			int count = Math.min(row, col);
			i = 1;
			while (i <= count) {
				if (layout[row - i][col - i] != '-') {
					if (layout[row - i][col - i] == 'b' || layout[row - i][col - i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(row, 7 - col);
			i = 1;
			while (i <= count) {
				if (layout[row - i][col + i] != '-') {
					if (layout[row - i][col + i] == 'b' || layout[row - i][col + i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(7 - row, col);
			i = 1;
			while (i <= count) {
				if (layout[row + i][col - i] != '-') {
					if (layout[row + i][col - i] == 'b' || layout[row + i][col - i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(7 - row, 7 - col);
			i = 1;
			while (i <= count) {
				if (layout[row + i][col + i] != '-') {
					if (layout[row + i][col + i] == 'b' || layout[row + i][col + i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}

			// check for rook/queen (white)
			i = 1;
			while (i <= 7 - row) {
				if (layout[row + i][col] != '-') {
					if (layout[row + i][col] == 'r' || layout[row + i][col] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= row) {
				if (layout[row - i][col] != '-') {
					if (layout[row - i][col] == 'r' || layout[row - i][col] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= 7 - col) {
				if (layout[row][col + i] != '-') {
					if (layout[row][col + i] == 'r' || layout[row][col + i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= col) {
				if (layout[row][col - i] != '-') {
					if (layout[row][col - i] == 'r' || layout[row][col - i] == 'q') {
						return true;
					}
					break;
				}
				i++;
			}

			// check for king (white)
			if (row > 0) {
				if (layout[row - 1][col] == 'k') {
					return true;
				}
				if (col > 0) {
					if (layout[row - 1][col - 1] == 'k') {
						return true;
					}
				}
				if (col < 7) {
					if (layout[row - 1][col + 1] == 'k') {
						return true;
					}
				}
			}
			if (row < 7) {
				if (layout[row + 1][col] == 'k') {
					return true;
				}
				if (col > 0) {
					if (layout[row + 1][col - 1] == 'k') {
						return true;
					}
				}
				if (col < 7) {
					if (layout[row + 1][col + 1] == 'k') {
						return true;
					}
				}
			}
			if (col > 0) {
				if (layout[row][col - 1] == 'k') {
					return true;
				}
			}
			if (col < 7) {
				if (layout[row][col + 1] == 'k') {
					return true;
				}
			}

		}
		else {

			// find the black king
			while (row < 8 && !exit) {
				while (col < 8 && !exit) {
					if (layout[row][col] == 'k') {
						exit = true;
					}
					else {
						col++;
					}
				}
				if (!exit) {
					row++;
					col = 0;
				}
			}

			// check for knight (black)
			if (row > 1) {
				if (col > 1) {
					if (layout[row - 2][col - 1] == 'N') {
						return true;
					}
					if (layout[row - 1][col - 2] == 'N') {
						return true;
					}
				}
				else if (col > 0) {
					if (layout[row - 2][col - 1] == 'N') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row - 2][col + 1] == 'N') {
						return true;
					}
					if (layout[row - 1][col + 2] == 'N') {
						return true;
					}
				}
				else if (col < 7) {
					if (layout[row - 2][col + 1] == 'N') {
						return true;
					}
				}
			}
			else if (row > 0) {
				if (col > 1) {
					if (layout[row - 1][col - 2] == 'N') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row - 1][col + 2] == 'N') {
						return true;
					}
				}
			}
			if (row < 6) {
				if (col > 1) {
					if (layout[row + 1][col - 2] == 'N') {
						return true;
					}
					if (layout[row + 2][col - 1] == 'N') {
						return true;
					}
				}
				else if (col > 0) {
					if (layout[row + 2][col - 1] == 'N') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row + 1][col + 2] == 'N') {
						return true;
					}
					if (layout[row + 2][col + 1] == 'N') {
						return true;
					}
				}
				else if (col < 7) {
					if (layout[row + 2][col + 1] == 'N') {
						return true;
					}
				}
			}
			else if (row < 7) {
				if (col > 1) {
					if (layout[row + 1][col - 2] == 'N') {
						return true;
					}
				}
				if (col < 6) {
					if (layout[row + 1][col + 2] == 'N') {
						return true;
					}
				}
			}

			// check for pawn (black)
			if (col > 0) {
				if (row < 7) {
					if (layout[row + 1][col - 1] == 'P') {
						return true;
					}
				}
			}
			if (col < 7) {
				if (row < 7) {
					if (layout[row + 1][col + 1] == 'P') {
						return true;
					}
				}
			}

			// check for bishop/queen (black)
			int count = Math.min(row, col);
			i = 1;
			while (i <= count) {
				if (layout[row - i][col - i] != '-') {
					if (layout[row - i][col - i] == 'B' || layout[row - i][col - i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(row, 7 - col);
			i = 1;
			while (i <= count) {
				if (layout[row - i][col + i] != '-') {
					if (layout[row - i][col + i] == 'B' || layout[row - i][col + i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(7 - row, col);
			i = 1;
			while (i <= count) {
				if (layout[row + i][col - i] != '-') {
					if (layout[row + i][col - i] == 'B' || layout[row + i][col - i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			count = Math.min(7 - row, 7 - col);
			i = 1;
			while (i <= count) {
				if (layout[row + i][col + i] != '-') {
					if (layout[row + i][col + i] == 'B' || layout[row + i][col + i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}

			// check for rook/queen (black)
			i = 1;
			while (i <= 7 - row) {
				if (layout[row + i][col] != '-') {
					if (layout[row + i][col] == 'R' || layout[row + i][col] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= row) {
				if (layout[row - i][col] != '-') {
					if (layout[row - i][col] == 'R' || layout[row - i][col] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= 7 - col) {
				if (layout[row][col + i] != '-') {
					if (layout[row][col + i] == 'R' || layout[row][col + i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}
			i = 1;
			while (i <= col) {
				if (layout[row][col - i] != '-') {
					if (layout[row][col - i] == 'R' || layout[row][col - i] == 'Q') {
						return true;
					}
					break;
				}
				i++;
			}

			// check for king (black)
			if (row > 0) {
				if (layout[row - 1][col] == 'K') {
					return true;
				}
				if (col > 0) {
					if (layout[row - 1][col - 1] == 'K') {
						return true;
					}
				}
				if (col < 7) {
					if (layout[row - 1][col + 1] == 'K') {
						return true;
					}
				}
			}
			if (row < 7) {
				if (layout[row + 1][col] == 'K') {
					return true;
				}
				if (col > 0) {
					if (layout[row + 1][col - 1] == 'K') {
						return true;
					}
				}
				if (col < 7) {
					if (layout[row + 1][col + 1] == 'K') {
						return true;
					}
				}
			}
			if (col > 0) {
				if (layout[row][col - 1] == 'K') {
					return true;
				}
			}
			if (col < 7) {
				if (layout[row][col + 1] == 'K') {
					return true;
				}
			}

		}

		inCheck = false;
		return false;
	}

	double SE() {
		double out = 0;

		if (whiteMove) {
			out += legalMoves.size() / 100.0;
			Position pos = new Position();
			pos.setLayout(layout);
			pos.setWhiteMove(false);
			pos.generateLegalMoves();
			out -= pos.getLegalMoves().size() / 100.0;
			if (pos.inCheck()) {
				out -= 0.3;
			}
		}
		else {
			out -= legalMoves.size() / 100.0;
			Position pos = new Position();
			pos.setLayout(layout);
			pos.setWhiteMove(true);
			pos.generateLegalMoves();
			out += pos.getLegalMoves().size() / 100.0;
			if (pos.inCheck()) {
				out += 0.3;
			}
		}

		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				switch (layout[row][col]) {
				case 'P':
					out += 1 + ((8 - row) / 100.0);
					break;
				case 'p':
					out -= 1 + ((row + 1) / 100.0);
					break;
				case 'R':
					out += 5 + ((8 - row) / 100.0);
					break;
				case 'r':
					out -= 5 + ((row + 1) / 100.0);
					break;
				case 'N':
					out += 3 + ((8 - row) / 100.0);
					break;
				case 'n':
					out -= 3 + ((row + 1) / 100.0);
					break;
				case 'B':
					out += 3.1 + ((8 - row) / 100.0);
					break;
				case 'b':
					out -= 3.1 + ((row + 1) / 100.0);
					break;
				case 'Q':
					out += 9 + ((8 - row) / 100.0);
					break;
				case 'q':
					out -= 9 + ((row + 1) / 100.0);
					break;
				}
			}
		}

		SE = out;
		return out;
	}

	double think(double depthThreshold, int depth, boolean printProgress) {
		if (SE == 0) {
			SE();
		}
		if (depth < 1) {
			bestEval = SE;
			return SE;
		}
		if (fiftyMoveRule == 50) {
			bestEval = 0;
			return 0;
		}
		char[][] test = new char[8][8];
		if (whiteMove) {
			if (legalMoves.isEmpty()) {
				if (inCheck()) {
					bestEval = -999;
					return -999;
				}
				else {
					bestEval = 0;
					return 0;
				}
			}
			for (int i = 0; i < legalMoves.size(); i++) {
				Position nextPos = new Position();
				String castleTemp = castle;
				String epTemp = ep;
				int fiftyTemp = fiftyMoveRule;
				if (printProgress) {
					System.out.println(i + "/" + legalMoves.size() + " complete");
				}
				for (int index = 0; index < 8; index++) {
					System.arraycopy(layout[index], 0, test[index], 0, 8);
				}
				String currentMove = legalMoves.get(i);
				if (currentMove.substring(0, 1).equals("O")) { // if the current move is a castle
					if (currentMove.length() == 3) { // castle king's side
						test[7][4] = '-';
						test[7][5] = 'R';
						test[7][6] = 'K';
						test[7][7] = '-';
						castleTemp = castleTemp.replace("K", "");
						castleTemp = castleTemp.replace("Q", "");
					}
					else { // castle queen's side
						test[7][0] = '-';
						test[7][1] = '-';
						test[7][2] = 'K';
						test[7][3] = 'R';
						test[7][4] = '-';
						castleTemp = castleTemp.replace("K", "");
						castleTemp = castleTemp.replace("Q", "");
					}
					if (castleTemp.length() == 0) {
						castleTemp = "-";
					}
					epTemp = "-";
					fiftyTemp++;
					if (fiftyMoveRule == fiftyTemp) {
						System.out.println("ERROR");
					}
				}
				else if (currentMove.substring(1, 2).equals("-")) { // if the current move is en passant

				}
				else { // if the current move is a non-special move
					int srow = Integer.parseInt(currentMove.substring(0, 1));
					int scol = Integer.parseInt(currentMove.substring(1, 2));
					int frow = Integer.parseInt(currentMove.substring(3, 4));
					int fcol = Integer.parseInt(currentMove.substring(4, 5));

					if (test[srow][scol] == 'P' || test[frow][fcol] != '-') { // update the fifty move rule
						fiftyTemp = 0;
					}
					else {
						fiftyTemp++;
					}

					if (test[srow][scol] == 'K') { // update the available castle moves
						castleTemp = castleTemp.replace("K", "");
						castleTemp = castleTemp.replace("Q", "");
					}
					else if (test[srow][scol] == 'R') {
						if (srow == 7 && scol == 0) {
							castleTemp = castleTemp.replace("Q", "");
						}
						else if (srow == 7 && scol == 7) {
							castleTemp = castleTemp.replace("K", "");
						}
					}
					if (test[frow][fcol] == 'r') {
						if (frow == 0 && fcol == 0) {
							castleTemp = castleTemp.replace("q", "");
						}
						else if (frow == 0 && fcol == 7) {
							castleTemp = castleTemp.replace("k", "");
						}
					}
					if (castleTemp.length() == 0) {
						castleTemp = "-";
					}

					if (currentMove.length() == 6) { // promotion
						test[frow][fcol] = currentMove.charAt(5);
					}
					else {
						test[frow][fcol] = test[srow][scol];
					}
					test[srow][scol] = '-';
				}

				nextPos = new Position(test, !whiteMove, castleTemp, epTemp, fiftyTemp, moveCount);
				nextPos.generateLegalMoves();

				double nextEval = nextPos.think(Bryan.depthThreshold, depth - 1, false);
				//		double nextEval;
				//		double diff = Math.abs(SE - nextPos.SE());
				//		if (diff > depthThreshold) {
				//		    nextEval = nextPos.think(diff, depth, false);
				//		} else {
				//		    nextEval = nextPos.think(Bryan.depthThreshold, depth - 1, false);
				//		}
				if (nextEval == 999) {
					bestMove = currentMove;
					bestEval = 999;
					return 999;
				}
				if (i == 0 || bestEval < nextEval) {
					bestEval = nextEval;
					bestMove = currentMove;
					if (printProgress) {
						System.out.println("Current Evaluation: " + bestEval);
						System.out.println("Current Best Move: " + translateMove(bestMove));
					}
				}
			}
			return bestEval;
		}
		else {
			if (legalMoves.isEmpty()) {
				if (inCheck()) {
					bestEval = 999;
					return 999;
				}
				else {
					bestEval = 0;
					return 0;
				}
			}
			for (int i = 0; i < legalMoves.size(); i++) {
				Position nextPos = new Position();
				String castleTemp = castle;
				String epTemp = ep;
				int fiftyTemp = fiftyMoveRule;
				if (printProgress) {
					System.out.println(i + "/" + legalMoves.size() + " complete");
				}
				for (int index = 0; index < 8; index++) {
					System.arraycopy(layout[index], 0, test[index], 0, 8);
				}
				String currentMove = legalMoves.get(i);
				if (currentMove.substring(0, 1).equals("O")) { // if the current move is a castle move
					if (currentMove.length() == 3) { // castle king's side
						test[0][4] = '-';
						test[0][5] = 'r';
						test[0][6] = 'k';
						test[0][7] = '-';
						castleTemp = castleTemp.replace("k", "");
						castleTemp = castleTemp.replace("q", "");
					}
					else { // castle queen's side
						test[0][0] = '-';
						test[0][1] = '-';
						test[0][2] = 'k';
						test[0][3] = 'r';
						test[0][4] = '-';
						castleTemp = castleTemp.replace("k", "");
						castleTemp = castleTemp.replace("q", "");
					}
					if (castleTemp.length() == 0) {
						castleTemp = "-";
					}
					epTemp = "-";
					fiftyTemp++;
				}
				else if (currentMove.substring(1, 2).equals("-")) { // if the current move is en passant

				}
				else { // if the current move is a non-special move
					int srow = Integer.parseInt(currentMove.substring(0, 1));
					int scol = Integer.parseInt(currentMove.substring(1, 2));
					int frow = Integer.parseInt(currentMove.substring(3, 4));
					int fcol = Integer.parseInt(currentMove.substring(4, 5));

					if (test[srow][scol] == 'p' || test[frow][fcol] != '-') { // update the fifty move rule
						fiftyTemp = 0;
					}
					else {
						fiftyTemp++;
					}

					if (test[srow][scol] == 'k') { // update the available castle moves
						castleTemp = castleTemp.replace("k", "");
						castleTemp = castleTemp.replace("q", "");
					}
					else if (test[srow][scol] == 'r') {
						if (srow == 0 && scol == 0) {
							castleTemp = castleTemp.replace("q", "");
						}
						else if (srow == 0 && scol == 7) {
							castleTemp = castleTemp.replace("k", "");
						}
					}
					if (test[frow][fcol] == 'R') {
						if (frow == 7 && fcol == 0) {
							castleTemp = castleTemp.replace("Q", "");
						}
						else if (frow == 7 && fcol == 7) {
							castleTemp = castleTemp.replace("K", "");
						}
					}
					if (castleTemp.length() == 0) {
						castleTemp = "-";
					}

					if (currentMove.length() == 6) { // promotion
						test[frow][fcol] = currentMove.charAt(5);
					}
					else {
						test[frow][fcol] = test[srow][scol];
					}
					test[srow][scol] = '-';
				}

				nextPos = new Position(test, !whiteMove, castleTemp, epTemp, fiftyTemp, moveCount + 1);
				nextPos.generateLegalMoves();

				double nextEval = nextPos.think(Bryan.depthThreshold, depth - 1, false);
				//		double nextEval;
				//		double diff = Math.abs(SE - nextPos.SE());
				//		if (diff > depthThreshold) {
				//		    nextEval = nextPos.think(diff, depth, false);
				//		} else {
				//		    nextEval = nextPos.think(Bryan.depthThreshold, depth - 1, false);
				//		}
				if (nextEval == -999) {
					bestMove = currentMove;
					bestEval = -999;
					return -999;
				}
				if (i == 0 || bestEval > nextEval) {
					bestEval = nextEval;
					bestMove = currentMove;
					if (printProgress) {
						System.out.println("Current Evaluation: " + bestEval);
						System.out.println("Current Best Move: " + translateMove(bestMove));
					}
				}
			}
			return bestEval;
		}
	}

	// translates the computer-understood move format into a human-understood move
	// format
	String translateMove(String move) {
		String[] alph = new String[]{ "a", "b", "c", "d", "e", "f", "g", "h" };
		String out;
		if (move.substring(0, 1).equals("O")) {
			return move;
		}
		else if (move.substring(1, 2).equals("-")) {
			out = alph[Integer.parseInt(move.substring(0, 1))] + "x" + alph[Integer.parseInt(move.substring(3, 4))]
				+ (8 - Integer.parseInt(move.substring(2, 3))) + " ep";
		}
		else {
			out = alph[Integer.parseInt(move.substring(1, 2))] + (8 - Integer.parseInt(move.substring(0, 1))) + "-"
				+ alph[Integer.parseInt(move.substring(4, 5))] + (8 - Integer.parseInt(move.substring(3, 4)));
			if (move.length() == 6) {
				out += "=" + move.substring(5, 6);
			}
		}
		return out;
	}

	void printLayout() {
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				System.out.print(layout[row][col] + " ");
			}
			System.out.println();
		}
	}

	void printLegalMoves() {
		System.out.println("Legal Moves:");
		if (!legalMoves.isEmpty()) {
			for (int i = 0; i < legalMoves.size(); i++) {
				System.out.println(legalMoves.get(i));
			}
		}
	}

	char[][] getLayout() {
		return layout;
	}

	void setLayout(char[][] tlayout) {
		layout = tlayout;
	}

	boolean getWhiteMove() {
		return whiteMove;
	}

	void setWhiteMove(boolean twhiteMove) {
		whiteMove = twhiteMove;
	}

	String getCastle() {
		return castle;
	}

	void setCastle(String tcastle) {
		castle = tcastle;
	}

	String getEp() {
		return ep;
	}

	void setEp(String tep) {
		ep = tep;
	}

	String getFEN() {
		return FEN;
	}

	void setFEN(String tFEN) {
		FEN = tFEN;
	}

	ArrayList<String> getLegalMoves() {
		return legalMoves;
	}

	void setLegalMoves(ArrayList<String> tlegalMoves) {
		legalMoves = tlegalMoves;
	}

	double getSE() {
		return SE;
	}

	void setSE(double tSE) {
		SE = tSE;
	}

	boolean getInCheck() {
		return inCheck;
	}

	void setInCheck(boolean tinCheck) {
		inCheck = tinCheck;
	}

	String getBestMove() {
		return bestMove;
	}

	void setBestMove(String tbestMove) {
		bestMove = tbestMove;
	}

	double getBestEval() {
		return bestEval;
	}

	void setBestEval(double tbestEval) {
		bestEval = tbestEval;
	}

};