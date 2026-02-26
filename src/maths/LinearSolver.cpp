#include "Solver.hpp"

LinearSolver::LinearSolver() : a(0.0), b(0.0) {}

LinearSolver::LinearSolver(double a, double b) : a(a), b(b) {}

LinearSolver::LinearSolver(const LinearSolver &other) : Solver(other), a(other.a), b(other.b) {}

LinearSolver::~LinearSolver() {}

LinearSolver	&LinearSolver::operator=(const LinearSolver &other)
{
	if (this != &other)
	{
		Solver::operator=(other);
		a = other.a;
		b = other.b;
	}
	return *this;
}

Solution	LinearSolver::getBestSolution() const
{
	Solution solution;
	if (a != 0.0)
		solution = Solution(-b / a, false);
	else
		solution = Solution(0.0, true);
	return solution;
}
