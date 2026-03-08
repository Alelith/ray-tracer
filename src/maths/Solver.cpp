#include "Solver.hpp"

Solution::Solution() : real(0.0), imaginary(0.0), isComplex(false) {}

Solution::Solution(float real, float imaginary, bool isComplex)
	: real(real), imaginary(imaginary), isComplex(isComplex) {}

Solutions::Solutions() : count(0) {}

void	Solutions::push(float real, float imaginary, bool isComplex)
{
	if (count < 4)
		data[count++] = Solution(real, imaginary, isComplex);
}

void	Solutions::push(const Solution &sol)
{
	if (count < 4)
		data[count++] = sol;
}
