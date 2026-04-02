#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <cmath>

struct Solution
{
	float	real;
	float	imaginary;
	bool	isComplex;
	Solution();
	Solution(float real, float imaginary, bool isComplex);
};

struct Solutions
{
	Solution	data[4];
	int			count;

	Solutions();
	void	push(float real, float imaginary, bool isComplex);
	void	push(const Solution &sol);
};

Solutions	solveLinear  (float a, float b);
Solutions	solveBhaskara(float a, float b, float c);
Solutions	solveCardano (float a, float b, float c, float d);
Solutions	solveFerrari (float a, float b, float c, float d, float e);

#endif /* SOLVER_HPP */
