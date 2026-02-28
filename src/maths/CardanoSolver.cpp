#include "Solver.hpp"

CardamoSolver::CardamoSolver(double a, double b, double c, double d)
{
	if (a == 0.0)
		throw std::invalid_argument("Coefficient a must be non-zero for a cubic equation.");
	this->a = 1.0;
	this->b = b / a;
	this->c = c / a;
	this->d = d / a;
}

vector<Solution>	CardamoSolver::getSolutions() const
{
	vector<Solution> solutions;

	double p = (3.0 * c - b * b) / 3.0;
	double q = (2.0 * b * b * b - 9.0 * b * c + 27.0 * d) / 27.0;
	double discriminant = q * q / 4.0 + p * p * p / 27.0;
	double offset = -b / 3.0;

	if (discriminant < 0.0)
	{
		double r = std::sqrt(-p * p * p / 27.0);
		double theta = std::acos(-q / (2.0 * r)) / 3.0;
		double rho = 2.0 * std::cbrt(r);

		solutions.emplace_back(Solution(rho * std::cos(theta) + offset, 0.0, false));
		solutions.emplace_back(Solution(rho * std::cos(theta + 2.0 * M_PI / 3.0) + offset, 0.0, false));
		solutions.emplace_back(Solution(rho * std::cos(theta + 4.0 * M_PI / 3.0) + offset, 0.0, false));
	}
	else
	{
		double alpha = std::cbrt(-q / 2.0 + std::sqrt(discriminant));
		double beta = std::cbrt(-q / 2.0 - std::sqrt(discriminant));

		solutions.emplace_back(Solution(alpha + beta + offset, 0.0, false));

		double realPart = -0.5 * (alpha + beta) + offset;
		double imagPart = 0.5 * std::sqrt(3.0) * (alpha - beta);

		solutions.emplace_back(Solution(realPart, imagPart, std::abs(imagPart) > 1e-10));
		solutions.emplace_back(Solution(realPart, -imagPart, std::abs(imagPart) > 1e-10));
	}
	return solutions;
}
