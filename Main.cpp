#include <iostream>
#include <chrono>
#include "Bryan.h"

int main() {
	Position pos = Position::StartingPosition();

	auto start = chrono::high_resolution_clock::now();

	vector<string> moves = pos.legalMoves();

	auto finish = chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	auto seconds = duration.count() / 1000000.0;

	pos.printBoard();
	cout << "Legal Moves: " << moves.size() << endl;
	for (unsigned char i = 0; i < moves.size(); i++) {
		cout << Position::translateMove(moves.at(i)) << endl;
	}
	cout << endl;
	cout << "Seconds Elapsed: " << seconds << endl;
	cout << "Nodes Per Second: " << moves.size() / seconds << endl;
}