#include <iostream>
#include <string>
#include <chrono>
#include "Position.h"

int main() {
	Position pos = Position::StartingPosition();
	
	auto start = chrono::high_resolution_clock::now();
	
	vector<string> moves = pos.legalMoves();

	auto finish = chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	auto seconds = duration.count() / 1000000.0;

	cout << "Legal Moves: " << moves.size() << endl;
	pos.printBoard();
	cout << "Seconds Elapsed: " << seconds << endl;
	cout << "Nodes Per Second: " << moves.size() / seconds << endl;
}