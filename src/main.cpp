#include "Camera.hpp"
#include "Material.hpp"
#include "Geometry.hpp"
#include "Timer.hpp"
#include "LinearBVH.hpp"
#include "debug/PrintPPM.hpp"

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
	makeSphere(Point3( 0.0,   0.0,  -2), 0.5, worldGeoms);   // centre
	makeSphere(Point3( 1.0,   0.0,  -1),  0.5, worldGeoms);   // right
	makeSphere(Point3( -1.0,   0.0,  -1),  0.5, worldGeoms);   // left
	makeSphere(Point3( 0.0, -100.5, -1), 100.0, worldGeoms);  // ground
	makeSphere(Point3( 0.0,   1.0,  -0.5), 0.25, worldGeoms);   // above

	//materials[0] = makeMetallic(Color(1, 1, 1), 0);
	/*materials[0] = makeDielectric(Color(0.8, 0.6, 0.2), 4.0, 0.0);
	materials[1] = makeDielectric(Color(1, 1, 1), 1.5, 0.0);
	materials[2] = makeLambertian(Color(0.8, 0.8, 0.8));
	materials[3] = makeLambertian(Color(0.8, 0.8, 0.0));*/

	addMaterial(Color(0, 1, 0), 0.0, 0.0, 1.0, 1.5, Color::Black, materials); // centre
	addMaterial(Color(1, 0, 1), 0.5, 1.0, 0.0, 0.0, Color::Black, materials); // right
	addMaterial(Color(1, 1, 1), 1.0, 0.0, 0.0, 1.0, Color::Black, materials); // left
	addMaterial(Color(0.0, 0.8, 0.8), 0.0, 1.0, 0.0, 0.0, Color::Black, materials); // ground
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color::Magenta, materials); // above

	Camera camera(16.0 / 9.0, 960, 25, 25, 90, Point3(0, 0, 0), Vector3::Backward);
	LinearBVH bvh(worldGeoms);
	Timer timer;
	timer.start();
	int spp = 25;
	for (int i = 0; i < spp; i++)
	{
		camera.renderSample(bvh, materials);

		vector<int> imageData = camera.getImage();
		int width = 960;
		int height = imageData.size() / width;
		debug::print_ppm(width, height, imageData);

		std::cerr << "\rSample " << (i + 1) << "/" << spp << " rendered." << std::flush;
	}
	timer.stop();
	timer.printElapsed();

	return 0;
}

