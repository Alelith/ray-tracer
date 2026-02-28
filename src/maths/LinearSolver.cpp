#include "Solver.hpp"

LinearSolver::LinearSolver(double a, double b) : a(a), b(b) {}

vector<Solution>	LinearSolver::getSolutions() const
{
	vector<Solution> solutions;
	if (a != 0.0)
		solutions.emplace_back(Solution(-b / a, 0.0, false));
	else
		solutions.emplace_back(Solution(0.0, 0.0, true));
	return solutions;
}
