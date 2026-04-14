#include "Solver.cuh"

namespace maths
{

	RT_CALLABLE Solutions	solveBhaskara(float a, float b, float c)
	{
		Solutions	sols;
		if (a == 0.0)
			return solveLinear(b, c);

		float	discriminant = b * b - 4.0f * a * c;
		if (discriminant > 0.0f)
		{
			float	sqrtDisc = sqrtf(discriminant);
			sols.push((-b + sqrtDisc) / (2.0f * a), 0.0, false);
			sols.push((-b - sqrtDisc) / (2.0f * a), 0.0, false);
		}
		else if (discriminant == 0.0f)
			sols.push(-b / (2.0f * a), 0.0, false);
		else
		{
			float	sqrtDisc = sqrtf(-discriminant);
			sols.push(-b / (2.0 * a),  sqrtDisc / (2.0 * a), true);
			sols.push(-b / (2.0 * a), -sqrtDisc / (2.0 * a), true);
		}
		return sols;
	}

} // namespace maths
