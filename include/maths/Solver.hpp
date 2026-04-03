#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <cmath>

# ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
# else
#  define RT_CALLABLE
# endif

struct Solution
{
	float	real;
	float	imaginary;
	bool	isComplex;
	RT_CALLABLE Solution();
	RT_CALLABLE Solution(float real, float imaginary, bool isComplex);
};

struct Solutions
{
	Solution	data[4];
	int			count;

	RT_CALLABLE Solutions();
	RT_CALLABLE void	push(float real, float imaginary, bool isComplex);
	RT_CALLABLE void	push(const Solution &sol);
};

RT_CALLABLE Solutions	solveLinear  (float a, float b);
RT_CALLABLE Solutions	solveBhaskara(float a, float b, float c);
RT_CALLABLE Solutions	solveCardano (float a, float b, float c, float d);
RT_CALLABLE Solutions	solveFerrari (float a, float b, float c, float d, float e);

#endif /* SOLVER_HPP */
