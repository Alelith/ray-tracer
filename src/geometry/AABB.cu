#include "AABB.cuh"

RT_CALLABLE AABB::AABB() : xLimits(Limits()), yLimits(Limits()), zLimits(Limits()) {}

RT_CALLABLE AABB::AABB(const Limits &xLimits, const Limits &yLimits, const Limits &zLimits) :
	xLimits(xLimits), yLimits(yLimits), zLimits(zLimits) {}

RT_CALLABLE AABB::AABB(const Point3 &a, const Point3 &b) :
	xLimits(Limits(fminf(a[0], b[0]), fmaxf(a[0], b[0]))),
	yLimits(Limits(fminf(a[1], b[1]), fmaxf(a[1], b[1]))),
	zLimits(Limits(fminf(a[2], b[2]), fmaxf(a[2], b[2]))) {}

RT_CALLABLE AABB::AABB(const AABB &box0, const AABB &box1) : xLimits(getInterval(box0.getXLimits(), box1.getXLimits())),
																yLimits(getInterval(box0.getYLimits(), box1.getYLimits())),
																zLimits(getInterval(box0.getZLimits(), box1.getZLimits())) {}

RT_CALLABLE AABB::AABB(const Point3 &localMin, const Point3 &localMax, const Matrix4x4 &masterMat)
{
	Point3	corners[8];
	corners[0] = Point3(localMin[0], localMin[1], localMin[2]);
	corners[1] = Point3(localMin[0], localMin[1], localMax[2]);
	corners[2] = Point3(localMin[0], localMax[1], localMin[2]);
	corners[3] = Point3(localMin[0], localMax[1], localMax[2]);
	corners[4] = Point3(localMax[0], localMin[1], localMin[2]);
	corners[5] = Point3(localMax[0], localMin[1], localMax[2]);
	corners[6] = Point3(localMax[0], localMax[1], localMin[2]);
	corners[7] = Point3(localMax[0], localMax[1], localMax[2]);

	Point3	wmin = masterMat * corners[0];
	Point3	wmax = wmin;
	
	for (int i = 1; i < 8; i++)
	{
		Point3 wc = masterMat * corners[i];
		wmin[0] = fminf(wmin[0], wc[0]);
		wmin[1] = fminf(wmin[1], wc[1]);
		wmin[2] = fminf(wmin[2], wc[2]);
		wmax[0] = fmaxf(wmax[0], wc[0]);
		wmax[1] = fmaxf(wmax[1], wc[1]);
		wmax[2] = fmaxf(wmax[2], wc[2]);
	}

	xLimits = Limits(wmin[0], wmax[0]);
	yLimits = Limits(wmin[1], wmax[1]);
	zLimits = Limits(wmin[2], wmax[2]);
}


const RT_CALLABLE Limits	&AABB::getXLimits() const { return xLimits; }
const RT_CALLABLE Limits	&AABB::getYLimits() const { return yLimits; }
const RT_CALLABLE Limits	&AABB::getZLimits() const { return zLimits; }

RT_CALLABLE float	AABB::getArea() const
{
	return 2 * (xLimits.getLength() * yLimits.getLength() + xLimits.getLength() * zLimits.getLength() + yLimits.getLength() * zLimits.getLength());
}

static const RT_CALLABLE Limits& getLimits(int axis, const AABB &box)
{
	if (axis == 0)
		return box.getXLimits();
	else if (axis == 1)
		return box.getYLimits();
	else
		return box.getZLimits();
}

RT_CALLABLE bool	AABB::hit(const Ray &ray, const Limits &limits) const
{
	float	tMin = limits.getMin();
	float	tMax = limits.getMax();

	for (int axis = 0; axis < 3; axis++)
	{
		float	invD = 1.0 / ray.getDirection()[axis];
		float	t0   = (getLimits(axis, *this).getMin() - ray.getOrigin()[axis]) * invD;
		float	t1   = (getLimits(axis, *this).getMax() - ray.getOrigin()[axis]) * invD;
		if (invD < 0.0)
		{
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}
		// Intersect: narrow the running [tMin, tMax] window with the slab [t0, t1]
		tMin = fmaxf(tMin, t0);
		tMax = fminf(tMax, t1);
		if (tMax <= tMin)
			return false;
	}
	return true;
}
