#include "Solver.cuh"

namespace maths
{

	RT_CALLABLE Solution::Solution() : real(0.0), imaginary(0.0), isComplex(false) {}
	RT_CALLABLE Solution::Solution(float real, float imaginary, bool isComplex) : real(real), imaginary(imaginary), isComplex(isComplex) {}

	RT_CALLABLE Solutions::Solutions() : count(0) {}

	RT_CALLABLE void	Solutions::push(float real, float imaginary, bool isComplex) { if (count < 4) data[count++] = Solution(real, imaginary, isComplex); }
	RT_CALLABLE void	Solutions::push(const Solution &sol) { if (count < 4) data[count++] = sol; }

} // namespace maths
