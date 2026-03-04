#include "BVH.hpp"
#include <algorithm>
#include <cmath>
#include <new>

// ── Helpers ────────────────────────────────────────────────────────────────────

static double	aabbCentroid(const AABB &aabb, int axis)
{
	if (axis == 0) return (aabb.getXLimits().getMin() + aabb.getXLimits().getMax()) * 0.5;
	if (axis == 1) return (aabb.getYLimits().getMin() + aabb.getYLimits().getMax()) * 0.5;
	return           (aabb.getZLimits().getMin() + aabb.getZLimits().getMax()) * 0.5;
}

static double	aabbSpan(const AABB &aabb, int axis)
{
	if (axis == 0) return aabb.getXLimits().getMax() - aabb.getXLimits().getMin();
	if (axis == 1) return aabb.getYLimits().getMax() - aabb.getYLimits().getMin();
	return           aabb.getZLimits().getMax() - aabb.getZLimits().getMin();
}

// ── geometryAABB ───────────────────────────────────────────────────────────────

AABB	geometryAABB(const GeometryData &g)
{
	switch (g.type)
	{
		case GeometryType::Sphere:
			// Unit sphere [-1,-1,-1] to [1,1,1] scaled/translated by transform
			return AABB(Point3(-1, -1, -1), Point3(1, 1, 1), g.transform);

		case GeometryType::Plane:
			// Planes are infinite; use a wide, thin local box in the Y-normal direction.
			// The transform aligns Y to the plane's actual normal.
			return AABB(Point3(-1e6, -1e-3, -1e6), Point3(1e6, 1e-3, 1e6), g.transform);

		case GeometryType::Cylinder:
		case GeometryType::Cone:
		case GeometryType::Hyperboloid:
			// All three have local bounds [-1,-0.5,-1] to [1,0.5,1]
			return AABB(Point3(-1, -0.5, -1), Point3(1, 0.5, 1), g.transform);

		case GeometryType::Torus:
		{
			// In local (unit majorRadius) space the torus reaches ±(1+thickness)
			// in XZ and ±thickness in Y.
			double r = g.thickness;
			return AABB(Point3(-(1 + r), -r, -(1 + r)),
						Point3( (1 + r),  r,  (1 + r)), g.transform);
		}

		case GeometryType::Kiss:
			// Kiss surface fits inside the unit cube
			return AABB(Point3(-1, -1, -1), Point3(1, 1, 1), g.transform);

		case GeometryType::Triangle:
			// World-space vertices — no transform needed
			return AABB(
				Point3(std::min({g.v0[0], g.v1[0], g.v2[0]}),
					   std::min({g.v0[1], g.v1[1], g.v2[1]}),
					   std::min({g.v0[2], g.v1[2], g.v2[2]})),
				Point3(std::max({g.v0[0], g.v1[0], g.v2[0]}),
					   std::max({g.v0[1], g.v1[1], g.v2[1]}),
					   std::max({g.v0[2], g.v1[2], g.v2[2]}))
			);

		default:
			return AABB(Limits(-1, 1), Limits(-1, 1), Limits(-1, 1));
	}
}

// ── bvhHit ─────────────────────────────────────────────────────────────────────

bool	bvhHit(const BVHView &bvh, const Ray &ray, const Limits &limits, HitRecord &record)
{
	if (bvh.nodeCount == 0 || bvh.rootIdx < 0)
		return false;

	int		stack[BVH_MAX_DEPTH];
	int		stackTop = 0;
	stack[stackTop++] = bvh.rootIdx;

	// Shrink the limit as we find closer hits, so AABB tests reject more nodes.
	Limits	current(limits.getMin(), limits.getMax());
	bool	anyHit = false;

	while (stackTop > 0)
	{
		const BVHNode	&node = bvh.nodes[stack[--stackTop]];

		if (!node.aabb.hit(ray, current))
			continue;

		if (node.geomIdx >= 0)
		{
			// Leaf — test the actual geometry
			if (geometryHit(bvh.geoms[node.geomIdx], ray, current, record))
			{
				anyHit  = true;
				current = Limits(current.getMin(), record.t);
			}
		}
		else
		{
			// Internal — push children (right first so left is popped first)
			if (node.right >= 0) stack[stackTop++] = node.right;
			if (node.left  >= 0) stack[stackTop++] = node.left;
		}
	}
	return anyHit;
}

// ── bvhAABBHit ────────────────────────────────────────────────────────────────

bool	bvhAABBHit(const BVHView &bvh, const Ray &ray, const Limits &limits)
{
	if (bvh.nodeCount == 0 || bvh.rootIdx < 0)
		return false;

	int	stack[BVH_MAX_DEPTH];
	int	stackTop = 0;
	stack[stackTop++] = bvh.rootIdx;

	while (stackTop > 0)
	{
		const BVHNode	&node = bvh.nodes[stack[--stackTop]];

		if (!node.aabb.hit(ray, limits))
			continue;

		if (node.geomIdx >= 0)
			return true;	// early-out on first leaf AABB hit

		if (node.right >= 0) stack[stackTop++] = node.right;
		if (node.left  >= 0) stack[stackTop++] = node.left;
	}
	return false;
}

// ── CPU builder ────────────────────────────────────────────────────────────────

// Recursive median-split builder.
// Nodes are written into tree.nodes[] at tree.nodeCount++ order.
static int	buildNode(BVHTree &tree, const GeometryData *geoms,
					  int *indices, int start, int end)
{
	int	nodeIdx = tree.nodeCount++;

	if (end - start == 1)
	{
		// Leaf
		tree.nodes[nodeIdx] = { geometryAABB(geoms[indices[start]]), -1, -1, indices[start] };
		return nodeIdx;
	}

	// Compute the combined AABB of all geometries in this range
	AABB	combined = geometryAABB(geoms[indices[start]]);
	for (int i = start + 1; i < end; ++i)
		combined = AABB(combined, geometryAABB(geoms[indices[i]]));

	// Find the longest axis
	int		axis = 0;
	double	bestSpan = aabbSpan(combined, 0);
	if (aabbSpan(combined, 1) > bestSpan) { axis = 1; bestSpan = aabbSpan(combined, 1); }
	if (aabbSpan(combined, 2) > bestSpan) { axis = 2; }

	// Sort indices by centroid along chosen axis
	std::sort(indices + start, indices + end, [&](int a, int b) {
		return aabbCentroid(geometryAABB(geoms[a]), axis)
			 < aabbCentroid(geometryAABB(geoms[b]), axis);
	});

	// Median split
	int	mid = (start + end) / 2;

	// Build subtrees — nodeIdx is reserved, children get higher indices
	int	leftIdx  = buildNode(tree, geoms, indices, start, mid);
	int	rightIdx = buildNode(tree, geoms, indices, mid, end);

	tree.nodes[nodeIdx] = { combined, leftIdx, rightIdx, -1 };
	return nodeIdx;
}

BVHTree	bvhBuild(const GeometryData *geoms, int geomCount)
{
	BVHTree	tree;
	tree.nodeCount = 0;
	tree.capacity  = (geomCount > 0) ? (2 * geomCount - 1) : 0;
	tree.nodes     = (tree.capacity > 0)
					 ? new (std::nothrow) BVHNode[tree.capacity]
					 : nullptr;

	if (!tree.nodes || geomCount == 0)
		return tree;

	// Temporary index array so original geoms[] stays untouched
	int	*indices = new (std::nothrow) int[geomCount];
	if (!indices) { tree.nodeCount = 0; return tree; }
	for (int i = 0; i < geomCount; ++i)
		indices[i] = i;

	buildNode(tree, geoms, indices, 0, geomCount);

	delete[] indices;
	return tree;
}

void	bvhFree(BVHTree &tree)
{
	delete[] tree.nodes;
	tree.nodes     = nullptr;
	tree.nodeCount = 0;
	tree.capacity  = 0;
}

BVHView	bvhMakeView(const BVHTree &tree, const GeometryData *geoms, int geomCount)
{
	return { tree.nodes, geoms, tree.nodeCount, geomCount, 0 };
}
