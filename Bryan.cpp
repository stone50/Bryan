#include <iostream>
#include <string>
#include "Position.h"

int main() {
	clock_t start;
	double duration;
	start = clock();

	//Position pos = Position::StartingPosition();
	Position pos = Position("r3k2r/1bpq3p/1p1p4/pQ1Pp1p1/P3Pp2/1PP2P2/5BPP/R3K2R b KQkq - 1 19");
	pos.legalMoves();
	pos.printBoard();
	cout << pos.FEN() << endl;

	duration = (double(clock()) - start) / (double)CLOCKS_PER_SEC;
	cout << "Time Elapsed: " << duration << '\n';
}