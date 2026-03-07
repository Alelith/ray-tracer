// ── Test scene ─────────────────────────────────────────────────────────────────
//
// 4 spheres + 1 torus, one material each:
//   [0] Sphere  centre    — Lambertian grey
//   [1] Sphere  left      — Metallic gold  (low fuzz)
//   [2] Sphere  right     — Dielectric glass (n = 1.5)
//   [3] Sphere  ground    — Lambertian green
//   [4] Torus   back/top  — Emissive orange
//
// Output: render.ppm  (PPM P3, sRGB gamma-corrected)

#include "Camera.hpp"
#include "Material.hpp"
#include "Geometry.hpp"
#include "BVH.hpp"

#include <fstream>
#include <cmath>
#include <algorithm>

// ── Helpers ─────────────────────────────────────────────────────────────────────

static TextureData	solidColor(double r, double g, double b)
{
	TextureData t;
	t.type   = TextureType::SolidColor;
	t.albedo = Color(r, g, b);
	return t;
}

static MaterialData	lambertian(double r, double g, double b)
{
	MaterialData m;
	m.type   = MaterialType::Lambertian;
	m.albedo = solidColor(r, g, b);
	m.param  = 0.0;
	return m;
}

static MaterialData	metallic(double r, double g, double b, double fuzz)
{
	MaterialData m;
	m.type   = MaterialType::Metallic;
	m.albedo = solidColor(r, g, b);
	m.param  = fuzz;
	return m;
}

static MaterialData	dielectric(double refIdx)
{
	MaterialData m;
	m.type   = MaterialType::Dielectric;
	m.albedo = solidColor(1, 1, 1);
	m.param  = refIdx;
	return m;
}

static MaterialData	emissive(double r, double g, double b)
{
	MaterialData m;
	m.type   = MaterialType::Emmisive;
	m.albedo = solidColor(r, g, b);
	m.param  = 0.0;
	return m;
}

// Linear -> sRGB gamma-2 approximation (sqrt) + clamp to [0, 255]
static int	toGamma(double linear)
{
	if (linear <= 0.0) return 0;
	double corrected = std::sqrt(linear);
	return static_cast<int>(std::min(corrected * 256.0, 255.0));
}

// ── main ────────────────────────────────────────────────────────────────────────

int main()
{
	// ── Geometry: 4 spheres + 1 torus ─────────────────────────────────────────
	static const int	GEOM_COUNT = 5;
	GeometryData		geoms[GEOM_COUNT];

	geoms[0] = makeSphere(Point3( 0.0,   0.0,  -2.5),  0.25);   // centre
	geoms[1] = makeSphere(Point3(-3,   0.0,  -3),  0.5);   // left
	geoms[2] = makeSphere(Point3( 3,   0.0,  -3),  0.5);   // right
	geoms[3] = makeSphere(Point3( 0.0, -100.5, -2.0), 100.0);  // ground
	geoms[4] = makeTorus (1.25, 0.5,
						  Point3(0.0, 0.0, -2),
						  Vector3(0.0, 1.0, 0.0));              // hole faces camera (Y = rotation axis)

	// ── Materials (order must match geomIdx) ──────────────────────────────────
	MaterialData		materials[GEOM_COUNT];

	materials[0] = lambertian(0.55, 0.55, 0.55);          // grey
	materials[1] = metallic  (0.82, 0.65, 0.18, 0.08);    // gold, slight fuzz
	materials[2] = dielectric(1.5);                        // glass
	materials[3] = lambertian(0.25, 0.55, 0.15);          // green grass
	materials[4] = emissive  (3.0,  1.2,  0.2);           // bright orange glow

	// ── BVH ───────────────────────────────────────────────────────────────────
	BVHTree		tree = bvhBuild(geoms, GEOM_COUNT);
	BVHView		bvh  = bvhMakeView(tree, geoms, GEOM_COUNT);

	// ── Camera ────────────────────────────────────────────────────────────────
	// aspectRatio, width, samplesPerPixel, maxDepth,
	// vfov, origin, lookDirection, defocusAngle, focalDistance, background
	Camera	cam(
		16.0 / 9.0,                // aspect ratio
		800,                       // width  → height = 450
		2000,                        // samples per pixel
		16,                        // max depth (russian roulette kicks in at 3)
		120.0,                      // vertical FOV (degrees)
		Point3(0.0, 0.0, 0.0),    // camera origin
		Vector3(0.0, 0.0, -1.0), // look direction (roughly toward centre sphere)
		0.0,                       // defocus angle (0 = no DoF blur)
		1.0,                       // focal distance (unused when defocus = 0)
		Color(0.50, 1.0, 1.0)    // dark blue-ish background
	);

	// ── Render ────────────────────────────────────────────────────────────────
	vector<vector<Color>>	image = cam.render(bvh, materials);

	// ── Write PPM ─────────────────────────────────────────────────────────────
	std::ofstream	ppm("render.ppm");
	if (!ppm)
	{
		bvhFree(tree);
		return 1;
	}

	const int	height = static_cast<int>(image.size());
	const int	width  = height > 0 ? static_cast<int>(image[0].size()) : 0;

	ppm << "P3\n" << width << " " << height << "\n255\n";

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const Color &c = image[y][x];
			ppm << toGamma(c[0]) << " "
				<< toGamma(c[1]) << " "
				<< toGamma(c[2]) << "\n";
		}
	}

	ppm.close();

	bvhFree(tree);
	return 0;
}

