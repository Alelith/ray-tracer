#ifndef BVH_HPP
# define BVH_HPP

# include <cstdint>
# include "AABB.hpp"
# include "Geometry.hpp"

// ── BVH node ───────────────────────────────────────────────────────────────────
//
//  Flat struct — uses integer indices instead of pointers.
//  GPU-compatible: trivially copyable, no heap, no virtual dispatch.
//
//  Internal node  →  left >= 0, right >= 0, geomIdx == -1
//  Leaf node      →  left == -1, right == -1, geomIdx >= 0

struct BVHNode
{
	AABB	aabb;
	int		left;		// index into BVHView::nodes; -1 = leaf
	int		right;		// index into BVHView::nodes; -1 = leaf
	int		geomIdx;	// index into BVHView::geoms; -1 = internal node
};

// ── BVH view ───────────────────────────────────────────────────────────────────
//
//  Non-owning slice. Passed to hit functions and GPU kernels.
//  For SYCL kernels: reconstruct from device accessors before calling bvhHit().

struct BVHView
{
	const BVHNode*		nodes;
	const GeometryData*	geoms;
	int					nodeCount;
	int					geomCount;
	int					rootIdx;
};

// ── Hit traversal ──────────────────────────────────────────────────────────────
//
//  Iterative depth-first traversal using a fixed-size stack.
//  No recursion, no heap — safe to call inside a GPU kernel.
//  Returns true if any geometry was hit; fills record with the closest.
//  The stack supports trees up to BVH_MAX_DEPTH levels deep
//  (covers scenes with up to 2^64 leaves in practice).

static const int	BVH_MAX_DEPTH = 64;

bool	bvhHit(const BVHView &bvh, const Ray &ray, const Limits &limits, HitRecord &record);

// ── AABB-only broad-phase (physics / culling) ──────────────────────────────────
//
//  Traverses only the AABB tree without testing any geometry.
//  Returns true on the first AABB leaf hit.
//  Useful for collision broad-phase, shadow rays with early-out, etc.

bool	bvhAABBHit(const BVHView &bvh, const Ray &ray, const Limits &limits);

// ── Per-geometry AABB ──────────────────────────────────────────────────────────
//
//  Computes a world-space AABB for a single GeometryData.
//  Used by the builder; also useful for physics integration.

AABB	geometryAABB(const GeometryData &g);

// ── CPU-side builder ───────────────────────────────────────────────────────────
//
//  BVHTree owns the node array. GPU-hostile (uses new[]/delete[]) —
//  build on CPU, upload nodes + geoms to device buffers, then use BVHView.

struct BVHTree
{
	BVHNode*	nodes;		// flat node array (heap-allocated)
	int			nodeCount;
	int			capacity;	// allocated capacity (always 2 * geomCount - 1)
};

// Build a BVH over geoms[0..geomCount-1] using median-split on the longest axis.
// The tree stores indices into the original geoms array — it does NOT copy geoms.
BVHTree	bvhBuild(const GeometryData *geoms, int geomCount);

// Release the node array allocated by bvhBuild().
void	bvhFree(BVHTree &tree);

// Construct a BVHView from a built tree (tree + geoms must outlive the view).
BVHView	bvhMakeView(const BVHTree &tree, const GeometryData *geoms, int geomCount);

#endif /* BVH_HPP */
