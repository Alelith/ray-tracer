/**
 * @file FerrariSolver.cu
 * @brief Implementation of Ferrari's root solver algorithm
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Solver.cuh"

/**
 * @struct ComplexF
 * @brief A simple internal complex number structure for numeric iterations
 * 
 * Used exclusively by the quartic polynomial solver to approximate roots.
 */
struct ComplexF
{
	float re; ///< Real component
	float im; ///< Imaginary component

	RT_CALLABLE ComplexF() : re(0.0f), im(0.0f) {}
	RT_CALLABLE ComplexF(float real, float imag) : re(real), im(imag) {}
};

/**
 * @brief Adds two internal complex numbers
 * 
 * @param[in] a First complex number
 * @param[in] b Second complex number
 * @return Complex result sum
 */
RT_CALLABLE static ComplexF cAdd(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re + b.re, a.im + b.im);
}

/**
 * @brief Subtracts two internal complex numbers
 * 
 * @param[in] a First complex member
 * @param[in] b Second complex member
 * @return Complex result difference
 */
RT_CALLABLE static ComplexF cSub(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re - b.re, a.im - b.im);
}

/**
 * @brief Multiplies two internal complex numbers
 * 
 * @param[in] a First complex member
 * @param[in] b Second complex member
 * @return Complex result product
 */
RT_CALLABLE static ComplexF cMul(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

/**
 * @brief Divides two internal complex numbers
 * 
 * @param[in] a First complex member
 * @param[in] b Second complex member
 * @return Complex result division
 */
RT_CALLABLE static ComplexF cDiv(const ComplexF &a, const ComplexF &b)
{
	float den = b.re * b.re + b.im * b.im;
	if (den < 1e-20f)
		return ComplexF(0.0f, 0.0f);
	return ComplexF((a.re * b.re + a.im * b.im) / den,
					(a.im * b.re - a.re * b.im) / den);
}

/**
 * @brief Calculates absolute magnitude of internal complex number
 * 
 * @param[in] z Complex number
 * @return float scalar magnitude
 */
RT_CALLABLE static float cAbs(const ComplexF &z)
{
	return sqrtf(z.re * z.re + z.im * z.im);
}

/**
 * @brief Evaluates a monic quartic polynomial on a complex coordinate
 * 
 * @param[in] b Cubic coefficient
 * @param[in] c Quadratic coefficient
 * @param[in] d Linear coefficient
 * @param[in] e Constant term
 * @param[in] x The complex variable
 * @return Complex result evaluation
 */
RT_CALLABLE static ComplexF evalQuarticMonic(float b, float c, float d, float e, const ComplexF &x)
{
	ComplexF acc(1.0f, 0.0f);
	acc = cAdd(cMul(acc, x), ComplexF(b, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(c, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(d, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(e, 0.0f));
	return acc;
}

/**
 * @brief Solves a quartic equation using numerical iterations based on Durand-Kerner method
 * 
 * @ingroup math_module
 * 
 * @param[in] a Quartic coefficient
 * @param[in] b Cubic coefficient
 * @param[in] c Quadratic coefficient
 * @param[in] d Linear coefficient
 * @param[in] e Constant term
 * @return Struct containing the resulting real or imaginary roots
 */
RT_CALLABLE Solutions	solveFerrari(float a, float b, float c, float d, float e)
{
	Solutions	sols;
	if (a == 0.0)
		return solveCardano(b, c, d, e);

	float bn = b / a;
	float cn = c / a;
	float dn = d / a;
	float en = e / a;

	// Robust fallback for difficult quartics: find all complex roots numerically.
	ComplexF roots[4] = {
		ComplexF(1.0f, 0.0f),
		ComplexF(0.35f, 0.90f),
		ComplexF(-0.80f, 0.20f),
		ComplexF(-0.30f, -0.75f)
	};

	const int maxIter = 80;
	const float deltaEps = 1e-6f;

	for (int iter = 0; iter < maxIter; ++iter)
	{
		float maxDelta = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			ComplexF denom(1.0f, 0.0f);
			for (int j = 0; j < 4; ++j)
			{
				if (j == i)
					continue;
				denom = cMul(denom, cSub(roots[i], roots[j]));
			}

			if (cAbs(denom) < 1e-12f)
				denom = cAdd(denom, ComplexF(1e-6f * (i + 1), -1e-6f * (i + 1)));

			ComplexF f = evalQuarticMonic(bn, cn, dn, en, roots[i]);
			ComplexF delta = cDiv(f, denom);
			roots[i] = cSub(roots[i], delta);
			maxDelta = fmaxf(maxDelta, cAbs(delta));
		}
		if (maxDelta < deltaEps)
			break;
	}

	for (int i = 0; i < 4; ++i)
	{
		float imag = roots[i].im;
		bool isComplex = fabsf(imag) > 1e-4f;
		sols.push(roots[i].re, isComplex ? imag : 0.0f, isComplex);
	}

	return sols;
}
