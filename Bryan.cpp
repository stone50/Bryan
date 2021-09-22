#include "Position.h"

#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int main() {
	Position currentPos = Position(starting_position);
	currentPos.printLayout();
	currentPos.generateLegalMoves();
	long startTime = System.nanoTime();
	currentPos.think(depthThreshold, 4, true);
	System.out.println();
	System.out.println();
	System.out.println("Final Evaluation: " + currentPos.bestEval);
	System.out.println("Best Move: " + currentPos.translateMove(currentPos.bestMove));
	long endTime = System.nanoTime();
	System.out.println("Time Elapsed: " + ((endTime - startTime) / 1000000000.0) + " seconds");
}