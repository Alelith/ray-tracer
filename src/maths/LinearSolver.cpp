#include "Solver.hpp"

RT_CALLABLE Solutions	solveLinear(float a, float b)
{
	Solutions	sols;
	if (a != 0.0)
		sols.push(-b / a, 0.0, false);
	return sols;
}
