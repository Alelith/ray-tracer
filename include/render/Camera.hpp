#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Ray.hpp"
# include "Random.hpp"
# include "Geometry.hpp"
# include "Material.hpp"
# include <iostream>

class Camera
{
	public:
		Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal);
		Camera(const Camera &other) = default;
		~Camera() = default;

		void	render(const WorldGeometries &geometries, const Materials &materials);
	private:
		float	aspectRatio;
		float	pixelSamplesScale;
		float	vfov;
		int		imageWidth;
		int		imageHeight;
		int		samplesPerPixel;
		int		maxDepth;
		Point3	cameraCenter;
		Point3	pixel00Loc;
		Vector3	normal;
		Vector3	pixelDeltaU;
		Vector3	pixelDeltaV;

		void	initialize();
		Ray		getRay(float u, float v) const;
		Color	rayColor(const Ray &ray, const WorldGeometries &geometries, const Materials &materials, int depth);
};

#endif /* CAMERA_HPP */
