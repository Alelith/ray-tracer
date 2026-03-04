#include "Solver.hpp"

Solutions	solveLinear(double a, double b)
{
	Solutions	sols;
	if (a != 0.0)
		sols.push(-b / a, 0.0, false);
	return sols;
}
