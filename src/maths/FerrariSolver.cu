#include "Solver.cuh"
struct ComplexF
{
	float re; 
	float im; 
	RT_CALLABLE ComplexF() : re(0.0f), im(0.0f) {}
	RT_CALLABLE ComplexF(float real, float imag) : re(real), im(imag) {}
};
RT_CALLABLE static ComplexF cAdd(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re + b.re, a.im + b.im);
}
RT_CALLABLE static ComplexF cSub(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re - b.re, a.im - b.im);
}
RT_CALLABLE static ComplexF cMul(const ComplexF &a, const ComplexF &b)
{
	return ComplexF(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}
RT_CALLABLE static ComplexF cDiv(const ComplexF &a, const ComplexF &b)
{
	float den = b.re * b.re + b.im * b.im;
	if (den < 1e-20f)
		return ComplexF(0.0f, 0.0f);
	return ComplexF((a.re * b.re + a.im * b.im) / den,
					(a.im * b.re - a.re * b.im) / den);
}
RT_CALLABLE static float cAbs(const ComplexF &z)
{
	return sqrtf(z.re * z.re + z.im * z.im);
}
RT_CALLABLE static ComplexF evalQuarticMonic(float b, float c, float d, float e, const ComplexF &x)
{
	ComplexF acc(1.0f, 0.0f);
	acc = cAdd(cMul(acc, x), ComplexF(b, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(c, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(d, 0.0f));
	acc = cAdd(cMul(acc, x), ComplexF(e, 0.0f));
	return acc;
}
RT_CALLABLE Solutions	solveFerrari(float a, float b, float c, float d, float e)
{
	Solutions	sols;
	if (a == 0.0)
		return solveCardano(b, c, d, e);
	float bn = b / a;
	float cn = c / a;
	float dn = d / a;
	float en = e / a;
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