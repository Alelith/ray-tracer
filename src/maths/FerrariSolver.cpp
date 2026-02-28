#include "Solver.hpp"

FerrariSolver::FerrariSolver(double a, double b, double c, double d, double e)
{
	if (a == 0.0)
		throw std::invalid_argument("Coefficient a must be non-zero for a quartic equation.");
	this->a = 1.0;
	this->b = b / a;
	this->c = c / a;
	this->d = d / a;
	this->e = e / a;
}

vector<Solution>	FerrariSolver::getSolutions() const
{
	vector<Solution> solutions;

	double resolventA = 1.0;
	double resolventB = -c;
	double resolventC = b * d - 4.0 * e;
	double resolventD = 4.0 * c * e - b * b * e - d * d;

	vector<Solution> resolventSolutions = CardamoSolver(resolventA, resolventB, resolventC, resolventD).getSolutions();
	double y = 0.0;
	for (const auto &sol : resolventSolutions)
	{
		if (!sol.isComplex)
		{
			y = sol.real;
			break;
		}
	}

	double m = std::sqrt(std::max(0.0, b * b / 4.0 - c + y));
	double n = std::sqrt(std::max(0.0, y * y / 4.0 - e));

	double firstCuadraticB = b / 2 + m;
	double firstCuadraticC = y / 2 - n;

	double secondCuadraticB = b / 2 - m;
	double secondCuadraticC = y / 2 + n;

	vector<Solution> firstCuadratic = BhaskaraSolver(1.0, firstCuadraticB, firstCuadraticC).getSolutions();
	vector<Solution> secondCuadratic = BhaskaraSolver(1.0, secondCuadraticB, secondCuadraticC).getSolutions();

	for (const auto &sol : firstCuadratic)
		solutions.emplace_back(sol);
	for (const auto &sol : secondCuadratic)
		solutions.emplace_back(sol);

	return solutions;
}
