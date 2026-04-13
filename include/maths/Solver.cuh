/**
 * @file Solver.cuh
 * @brief Algebraic equation solvers for intersection tests
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef SOLVER_HPP
# define SOLVER_HPP

# include <cmath>

# ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
# else
#  define RT_CALLABLE
# endif

# ifndef M_PI_F
#  define M_PI_F 3.14159265358979323846f
# endif

/**
 * @struct Solution
 * @brief Holds a single mathematical solution
 * 
 * Represents either a real or a complex root returned by an algebraic solver.
 * 
 * @ingroup math_module
 */
struct Solution
{
	float	real;      ///< Real part of the solution
	float	imaginary; ///< Imaginary part of the solution
	bool	isComplex; ///< Flag indicating if the solution has an imaginary part

	RT_CALLABLE Solution();
	RT_CALLABLE Solution(float real, float imaginary, bool isComplex);
};

/**
 * @struct Solutions
 * @brief A bounded collection of equation solutions
 * 
 * Typically used to store up to four roots from linear, quadratic, cubic, 
 * or quartic equation solvers.
 * 
 * @ingroup math_module
 */
struct Solutions
{
	Solution	data[4]; ///< Array storing up to 4 roots
	int			count;   ///< Number of valid roots stored

	RT_CALLABLE Solutions();
	RT_CALLABLE void	push(float real, float imaginary, bool isComplex);
	RT_CALLABLE void	push(const Solution &sol);
};

/**
 * @brief Solves a linear equation: ax + b = 0
 * 
 * @details Evaluates the single unknown by isolating x = -b / a. 
 * If a is remarkably close to zero, it evaluates the constant mathematically by 
 * possibly returning zero valid solutions.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Coefficient of x
 * @param[in] b Constant term
 * @return Struct containing the solution if any
 */
RT_CALLABLE Solutions	solveLinear  (float a, float b);

/**
 * @brief Solves a quadratic equation: ax^2 + bx + c = 0
 * 
 * @details Applies the classic quadratic formula (Bhaskara's method) deriving up to 
 * two distinct real roots. It primarily calculates the discriminant (b^2 - 4ac) 
 * to determine the number and nature of roots. Returns complex roots if requested 
 * by the implementation context, though most ray-casting equations only care for reals.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Quadratic coefficient
 * @param[in] b Linear coefficient
 * @param[in] c Constant term
 * @return Struct containing the resulting roots
 */
RT_CALLABLE Solutions	solveBhaskara(float a, float b, float c);

/**
 * @brief Solves a cubic equation: ax^3 + bx^2 + cx + d = 0
 * 
 * @details Uses Cardano's analytical method by applying a depressed cubic 
 * transformation (x = t - b/3a) to resolve up to three algebraic roots.
 * Handles the calculation of the discriminant and resolving real solutions 
 * for intersections against continuous higher-degree complex geometries.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Cubic coefficient
 * @param[in] b Quadratic coefficient
 * @param[in] c Linear coefficient
 * @param[in] d Constant term
 * @return Struct containing the resulting roots
 */
RT_CALLABLE Solutions	solveCardano (float a, float b, float c, float d);

/**
 * @brief Solves a quartic equation: ax^4 + bx^3 + cx^2 + dx + e = 0
 * 
 * @details Leverages Ferrari's method for resolving 4th-degree polynomials 
 * through resolvent cubics. Transforms the quartic into a product of two 
 * quadratic expressions, subsequently solving them using Bhaskara's solver. 
 * Vital for detecting intersections on torus surfaces or advanced custom 
 * spline geometries.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Quartic coefficient
 * @param[in] b Cubic coefficient
 * @param[in] c Quadratic coefficient
 * @param[in] d Linear coefficient
 * @param[in] e Constant term
 * @return Struct containing the resulting roots
 */
RT_CALLABLE Solutions	solveFerrari (float a, float b, float c, float d, float e);

#endif /* SOLVER_HPP */
