#include "Solver.cuh"

namespace maths
{

	RT_CALLABLE Solutions	solveCardano(float a, float b, float c, float d)
	{
		Solutions	sols;
		if (a == 0.0)
			return solveBhaskara(b, c, d);

		float	bn = b / a;
		float	cn = c / a;
		float	dn = d / a;

		float	p = (3.0 * cn - bn * bn) / 3.0;
		float	q = (2.0 * bn * bn * bn - 9.0 * bn * cn + 27.0 * dn) / 27.0;

		float	discriminant = q * q / 4.0 + p * p * p / 27.0;
		float	offset = -bn / 3.0;
		if (discriminant < 0.0)
		{
			float	r = sqrtf(-p * p * p / 27.0f);
			float	acosArg = -q / (2.0f * r);
			acosArg = fmaxf(-1.0f, fminf(1.0f, acosArg));

			float	theta = acosf(acosArg) / 3.0f;
			float	rho = 2.0f * cbrtf(r);

			sols.push(rho * cosf(theta) + offset, 0.0, false);
			sols.push(rho * cosf(theta + 2.0f * M_PI_F / 3.0f) + offset, 0.0, false);
			sols.push(rho * cosf(theta + 4.0f * M_PI_F / 3.0f) + offset, 0.0, false);
		}
		else
		{
			float	alpha = cbrtf(-q / 2.0f + sqrtf(discriminant));
			float	beta = cbrtf(-q / 2.0f - sqrtf(discriminant));
			sols.push(alpha + beta + offset, 0.0, false);
			
			float	realPart = -0.5f * (alpha + beta) + offset;
			float	imagPart =  0.5f * sqrtf(3.0f) * (alpha - beta);
			sols.push(realPart,  imagPart, fabsf(imagPart) > 1e-10f);
			sols.push(realPart, -imagPart, fabsf(imagPart) > 1e-10f);
		}
		return sols;
	}

} // namespace maths
