#ifndef SOLVER_HPP
# define SOLVER_HPP

# include "Ray.hpp"

struct Solution
{
	double	real;
	double	imaginary;
	bool	isComplex;
	Solution();
	Solution(double real, double imaginary, bool isComplex);
};

// Fixed-size result container: max 4 roots (quartic equation).
// No heap allocation — safe for GPU device code.
struct Solutions
{
	Solution	data[4];
	int			count;

	Solutions();
	void	push(double real, double imaginary, bool isComplex);
	void	push(const Solution &sol);
};

// ── Free functions ────────────────────────────────────────────────────────────
// No virtual dispatch, no heap, GPU-compatible.
// Returns empty Solutions (count == 0) for degenerate inputs (a == 0).

Solutions	solveLinear  (double a, double b);
Solutions	solveBhaskara(double a, double b, double c);
Solutions	solveCardano (double a, double b, double c, double d);
Solutions	solveFerrari (double a, double b, double c, double d, double e);

#endif /* SOLVER_HPP */
