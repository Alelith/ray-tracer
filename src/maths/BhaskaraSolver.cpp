#include "Solver.hpp"

Solutions	solveBhaskara(double a, double b, double c)
{
	Solutions	sols;
	if (a == 0.0)
		return solveLinear(b, c);

	double discriminant = b * b - 4.0 * a * c;
	if (discriminant > 0.0)
	{
		double sqrtDisc = std::sqrt(discriminant);
		sols.push((-b + sqrtDisc) / (2.0 * a), 0.0, false);
		sols.push((-b - sqrtDisc) / (2.0 * a), 0.0, false);
	}
	else if (discriminant == 0.0)
		sols.push(-b / (2.0 * a), 0.0, false);
	else
	{
		double sqrtDisc = std::sqrt(-discriminant);
		sols.push(-b / (2.0 * a),  sqrtDisc / (2.0 * a), true);
		sols.push(-b / (2.0 * a), -sqrtDisc / (2.0 * a), true);
	}
	return sols;
}
