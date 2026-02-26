#include "Solver.hpp"

BhaskaraSolver::BhaskaraSolver() : a(0.0), b(0.0), c(0.0) {}

BhaskaraSolver::BhaskaraSolver(double a, double b, double c) : a(a), b(b), c(c) {}

BhaskaraSolver::BhaskaraSolver(const BhaskaraSolver &other) : Solver(other), a(other.a), b(other.b), c(other.c) {}

BhaskaraSolver::~BhaskaraSolver() {}

BhaskaraSolver	&BhaskaraSolver::operator=(const BhaskaraSolver &other)
{
	if (this != &other)
	{
		Solver::operator=(other);
		a = other.a;
		b = other.b;
		c = other.c;
	}
	return *this;
}

Solution	BhaskaraSolver::getBestSolution() const
{
	Solution solution;
	double discriminant = b * b - 4.0 * a * c;
	if (discriminant > 0.0)
	{
		double sqrtDiscriminant = sqrt(discriminant);
		double root1 = (-b + sqrtDiscriminant) / (2.0 * a);
		double root2 = (-b - sqrtDiscriminant) / (2.0 * a);
		if (root1 < root2)
			solution = Solution(root1, false);
		else
			solution = Solution(root2, false);
	}
	else if (discriminant == 0.0)
	{
		double root = -b / (2.0 * a);
		solution = Solution(root, false);
	}
	else
		solution = Solution(0.0, true);
	return solution;
}
