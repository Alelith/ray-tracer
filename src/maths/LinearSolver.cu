/**
 * @file LinearSolver.cu
 * @brief Implementation of a linear equation solver
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Solver.cuh"

/**
 * @brief Computes the root for a linear expression ax + b = 0
 * 
 * @details Checks for non-zero slop, avoiding division by zero.
 * Returns up to one real solution.
 * 
 * @ingroup math_module
 * 
 * @param[in] a Sub-linear coefficient
 * @param[in] b Offset
 * @return Solutions with up to one real root
 */
RT_CALLABLE Solutions	solveLinear(float a, float b)
{
	Solutions	sols;
	if (a != 0.0)
		sols.push(-b / a, 0.0, false);
	return sols;
}
