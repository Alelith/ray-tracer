#include "Solver.hpp"

Solutions	solveFerrari(double a, double b, double c, double d, double e)
{
	Solutions	sols;
	if (a == 0.0)
		return sols;

	// Normalize to monic form
	double	bn = b / a;
	double	cn = c / a;
	double	dn = d / a;
	double	en = e / a;

	// Resolvent cubic: y³ - cn*y² + (bn*dn - 4*en)*y - (bn²*en - 4*cn*en + dn²) = 0
	Solutions	resolvent = solveCardano(1.0, -cn, bn * dn - 4.0 * en,
									4.0 * cn * en - bn * bn * en - dn * dn);
	double	y = 0.0;
	for (int i = 0; i < resolvent.count; ++i)
	{
		if (!resolvent.data[i].isComplex)
		{
			y = resolvent.data[i].real;
			break;
		}
	}

	double	m  = std::sqrt(std::max(0.0, bn * bn / 4.0 - cn + y));
	double	n  = std::sqrt(std::max(0.0, y  * y  / 4.0 - en));

	Solutions	q1 = solveBhaskara(1.0, bn / 2.0 + m, y / 2.0 - n);
	Solutions	q2 = solveBhaskara(1.0, bn / 2.0 - m, y / 2.0 + n);

	for (int i = 0; i < q1.count; ++i) sols.push(q1.data[i]);
	for (int i = 0; i < q2.count; ++i) sols.push(q2.data[i]);

	return sols;
}
