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

#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

int main()
{
	WorldGeometries worldGeoms;
	Materials materials;

	/*//geoms[0] = makeCylinder(0.5, 0.75, Point3( 0.0,   0.0,  -1.2), Vector3(0.75, 1, 0));   // centre*/
	makeSphere(Point3( 0.0,   0.0,  -1.2), 0.5, worldGeoms);   // centre
	makeSphere(Point3( 1.0,   0.0,  -1),  0.5, worldGeoms);   // right
	makeSphere(Point3( -1.0,   0.0,  -1),  0.5, worldGeoms);   // left
	makeSphere(Point3( 0.0, -100.5, -1), 100.0, worldGeoms);  // ground

	//materials[0] = makeMetallic(Color(1, 1, 1), 0);
	/*materials[0] = makeDielectric(Color(0.8, 0.6, 0.2), 4.0, 0.0);
	materials[1] = makeDielectric(Color(1, 1, 1), 1.5, 0.0);
	materials[2] = makeLambertian(Color(0.8, 0.8, 0.8));
	materials[3] = makeLambertian(Color(0.8, 0.8, 0.0));*/

	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, materials); // centre
	addMaterial(Color(1, 0, 1), 1.0, 0.0, 0.0, 0.0, materials); // right
	addMaterial(Color(0.8, 0.8, 0.8), 0.0, 1.0, 0.0, 0.0, materials); // left
	addMaterial(Color(0.8, 0.8, 0.0), 0.0, 1.0, 0.0, 0.0, materials); // ground

	Camera camera(16.0 / 9.0, 960, 25, 25, 90, Point3(-2, 2, 1), Vector3(1, -1, -1) / std::sqrt(3));
	camera.render(worldGeoms, materials);
	
	return 0;
}

