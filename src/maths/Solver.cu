/**
 * @file Solver.cu
 * @brief Common data structures for root solving algorithms
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Solver.cuh"

/**
 * @brief Default constructor for a zero real solution
 * 
 * @ingroup math_module
 */
RT_CALLABLE Solution::Solution() : real(0.0), imaginary(0.0), isComplex(false) {}

/**
 * @brief Constructor for a complex or real solution
 * 
 * @ingroup math_module
 * 
 * @param[in] real Real part
 * @param[in] imaginary Imaginary part
 * @param[in] isComplex True if it's explicitly considered complex
 */
RT_CALLABLE Solution::Solution(float real, float imaginary, bool isComplex)
	: real(real), imaginary(imaginary), isComplex(isComplex) {}

/**
 * @brief Default constructor initializing an empty collection
 * 
 * @ingroup math_module
 */
RT_CALLABLE Solutions::Solutions() : count(0) {}

/**
 * @brief Pushes a new constructed root into the collected solutions array
 * 
 * @ingroup math_module
 * 
 * @param[in] real The real part
 * @param[in] imaginary The imaginary part
 * @param[in] isComplex true if complex
 */
RT_CALLABLE void	Solutions::push(float real, float imaginary, bool isComplex)
{
	if (count < 4)
		data[count++] = Solution(real, imaginary, isComplex);
}

/**
 * @brief Submits a root struct into the solutions array
 * 
 * @ingroup math_module
 * 
 * @param[in] sol Target solution
 */
RT_CALLABLE void	Solutions::push(const Solution &sol)
{
	if (count < 4)
		data[count++] = sol;
}
