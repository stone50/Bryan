#pragma once

#include "Position.h"
#include "Evaluation.h"

class Bryan {
	Bryan();

	Evaluation analyzePosition(
		Position position,
		unsigned short int depth
	);
};