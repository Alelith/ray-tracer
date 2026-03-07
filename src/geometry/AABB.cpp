#include "AABB.hpp"

AABB::AABB() : xLimits(Limits()), yLimits(Limits()), zLimits(Limits()) {}

AABB::AABB(const Limits &xLimits, const Limits &yLimits, const Limits &zLimits) :
	xLimits(xLimits), yLimits(yLimits), zLimits(zLimits) {}

AABB::AABB(const Point3 &a, const Point3 &b) :
	xLimits(Limits(std::min(a[0], b[0]), std::max(a[0], b[0]))),
	yLimits(Limits(std::min(a[1], b[1]), std::max(a[1], b[1]))),
	zLimits(Limits(std::min(a[2], b[2]), std::max(a[2], b[2]))) {}

AABB::AABB(const AABB &box0, const AABB &box1) : xLimits(getInterval(box0.getXLimits(), box1.getXLimits())),
												yLimits(getInterval(box0.getYLimits(), box1.getYLimits())),
												zLimits(getInterval(box0.getZLimits(), box1.getZLimits())) {}

AABB::AABB(const Point3 &localMin, const Point3 &localMax, const Transform &transform)
{
	Point3	corners[8];
	corners[0] = Point3(localMin[0], localMin[1], localMin[2], 1.0);
	corners[1] = Point3(localMin[0], localMin[1], localMax[2], 1.0);
	corners[2] = Point3(localMin[0], localMax[1], localMin[2], 1.0);
	corners[3] = Point3(localMin[0], localMax[1], localMax[2], 1.0);
	corners[4] = Point3(localMax[0], localMin[1], localMin[2], 1.0);
	corners[5] = Point3(localMax[0], localMin[1], localMax[2], 1.0);
	corners[6] = Point3(localMax[0], localMax[1], localMin[2], 1.0);
	corners[7] = Point3(localMax[0], localMax[1], localMax[2], 1.0);

	Point3	wmin = transform.getMatrix() * corners[0];
	Point3	wmax = wmin;
	
	for (int i = 1; i < 8; i++)
	{
		Point3 wc = transform.getMatrix() * corners[i];
		wmin[0] = std::min(wmin[0], wc[0]);
		wmin[1] = std::min(wmin[1], wc[1]);
		wmin[2] = std::min(wmin[2], wc[2]);
		wmax[0] = std::max(wmax[0], wc[0]);
		wmax[1] = std::max(wmax[1], wc[1]);
		wmax[2] = std::max(wmax[2], wc[2]);
	}

	xLimits = Limits(wmin[0], wmax[0]);
	yLimits = Limits(wmin[1], wmax[1]);
	zLimits = Limits(wmin[2], wmax[2]);
}


const Limits	&AABB::getXLimits() const { return xLimits; }
const Limits	&AABB::getYLimits() const { return yLimits; }
const Limits	&AABB::getZLimits() const { return zLimits; }

static const Limits& getLimits(int axis, const AABB &box)
{
	if (axis == 0)
		return box.getXLimits();
	else if (axis == 1)
		return box.getYLimits();
	else
		return box.getZLimits();
}

bool	AABB::hit(const Ray &ray, const Limits &limits) const
{
	double	tMin = limits.getMin();
	double	tMax = limits.getMax();

	for (int axis = 0; axis < 3; axis++)
	{
		double	invD = 1.0 / ray.getDirection()[axis];
		double	t0   = (getLimits(axis, *this).getMin() - ray.getOrigin()[axis]) * invD;
		double	t1   = (getLimits(axis, *this).getMax() - ray.getOrigin()[axis]) * invD;
		if (invD < 0.0)
			std::swap(t0, t1);
		// Intersect: narrow the running [tMin, tMax] window with the slab [t0, t1]
		tMin = std::max(tMin, t0);
		tMax = std::min(tMax, t1);
		if (tMax <= tMin)
			return false;
	}
	return true;
}
