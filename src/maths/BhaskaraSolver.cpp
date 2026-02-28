#include "Solver.hpp"

BhaskaraSolver::BhaskaraSolver(double a, double b, double c)
{
	if (a == 0.0)
		throw std::invalid_argument("Coefficient a must be non-zero for a quadratic equation.");
	this->a = a;
	this->b = b;
	this->c = c;
}

vector<Solution>	BhaskaraSolver::getSolutions() const
{
	vector<Solution> solutions;

	double discriminant = b * b - 4.0 * a * c;
	if (discriminant > 0.0)
	{
		double sqrtDisc = std::sqrt(discriminant);

		solutions.emplace_back(Solution((-b + sqrtDisc) / (2.0 * a), 0.0, false));
		solutions.emplace_back(Solution((-b - sqrtDisc) / (2.0 * a), 0.0, false));
	}
	else if (discriminant == 0.0)
		solutions.emplace_back(Solution(-b / (2.0 * a), 0.0, false));
	else
	{
		double sqrtDisc = std::sqrt(-discriminant);
		
		solutions.emplace_back(Solution(-b / (2.0 * a), sqrtDisc / (2.0 * a), true));
		solutions.emplace_back(Solution(-b / (2.0 * a), -sqrtDisc / (2.0 * a), true));
	}
	return solutions;
}
