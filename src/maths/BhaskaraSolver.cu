/**
 * @file BhaskaraSolver.cu
 * @brief Implementation of Bhaskara (quadratic formula) root solver
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Solver.cuh"

/**
 * @brief Attempts to solve a quadratic equation ax^2 + bx + c = 0
 * 
 * @details Evaluates the discriminant to find real and complex roots. 
 * Falls back to linear solver if the squared coefficient is zero.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Quadratic scale
 * @param[in] b Linear scale
 * @param[in] c Constant
 * @return Struct with solutions (up to two distinct roots)
 */
RT_CALLABLE Solutions	solveBhaskara(float a, float b, float c)
{
	Solutions	sols;
	if (a == 0.0)
		return solveLinear(b, c);

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant > 0.0f)
	{
		float sqrtDisc = sqrtf(discriminant);
		sols.push((-b + sqrtDisc) / (2.0f * a), 0.0, false);
		sols.push((-b - sqrtDisc) / (2.0f * a), 0.0, false);
	}
	else if (discriminant == 0.0f)
		sols.push(-b / (2.0f * a), 0.0, false);
	else
	{
		float sqrtDisc = sqrtf(-discriminant);
		sols.push(-b / (2.0 * a),  sqrtDisc / (2.0 * a), true);
		sols.push(-b / (2.0 * a), -sqrtDisc / (2.0 * a), true);
	}
	return sols;
}
