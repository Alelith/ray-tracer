#include "Solver.cuh"

RT_CALLABLE Solutions	solveFerrari(float a, float b, float c, float d, float e)
{
	Solutions	sols;
	if (a == 0.0)
		return solveCardano(b, c, d, e);

	float	bn = b / a;
	float	cn = c / a;
	float	dn = d / a;
	float	en = e / a;

	Solutions	resolvent = solveCardano(1.0, -cn, bn * dn - 4.0 * en,
									4.0 * cn * en - bn * bn * en - dn * dn);
	float	y = 0.0;
	for (int i = 0; i < resolvent.count; ++i)
	{
		if (!resolvent.data[i].isComplex)
		{
			y = resolvent.data[i].real;
			break;
		}
	}

	float	m  = sqrtf(fmaxf(0.0f, bn * bn / 4.0 - cn + y));

	float	n;
	if (m > 1e-10)
		n = (dn - bn * y) / (2.0 * m);
	else
		n = sqrtf(fmaxf(0.0f, y * y / 4.0 - en));

	Solutions	q1 = solveBhaskara(1.0, bn / 2.0 + m, y / 2.0 - n);
	Solutions	q2 = solveBhaskara(1.0, bn / 2.0 - m, y / 2.0 + n);

	for (int i = 0; i < q1.count; ++i) sols.push(q1.data[i]);
	for (int i = 0; i < q2.count; ++i) sols.push(q2.data[i]);

	return sols;
}
