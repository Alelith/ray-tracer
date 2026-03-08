#include "Solver.hpp"

Solutions	solveCardano(float a, float b, float c, float d)
{
	Solutions	sols;
	if (a == 0.0)
		return solveBhaskara(b, c, d);

	// Normalize to monic form: t³ + pt + q = 0  (substitute t = x - b/3a)
	float	bn = b / a;
	float	cn = c / a;
	float	dn = d / a;

	float	p = (3.0 * cn - bn * bn) / 3.0;
	float	q = (2.0 * bn * bn * bn - 9.0 * bn * cn + 27.0 * dn) / 27.0;
	float	discriminant = q * q / 4.0 + p * p * p / 27.0;
	float	offset = -bn / 3.0;

	if (discriminant < 0.0)
	{
		float r     = std::sqrt(-p * p * p / 27.0);
		float theta = std::acos(-q / (2.0 * r)) / 3.0;
		float rho   = 2.0 * std::cbrt(r);

		sols.push(rho * std::cos(theta) + offset, 0.0, false);
		sols.push(rho * std::cos(theta + 2.0 * M_PI / 3.0) + offset, 0.0, false);
		sols.push(rho * std::cos(theta + 4.0 * M_PI / 3.0) + offset, 0.0, false);
	}
	else
	{
		float alpha = std::cbrt(-q / 2.0 + std::sqrt(discriminant));
		float beta  = std::cbrt(-q / 2.0 - std::sqrt(discriminant));

		sols.push(alpha + beta + offset, 0.0, false);

		float realPart = -0.5 * (alpha + beta) + offset;
		float imagPart =  0.5 * std::sqrt(3.0) * (alpha - beta);

		sols.push(realPart,  imagPart, std::abs(imagPart) > 1e-10);
		sols.push(realPart, -imagPart, std::abs(imagPart) > 1e-10);
	}
	return sols;
}
