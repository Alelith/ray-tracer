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
		Camera(double aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, double vfov, const Point3 &center, const Vector3 &normal);
		Camera(const Camera &other) = default;
		~Camera() = default;

		void	render(const GeometryData *geoms, const MaterialData *materials, int geomCount);
	private:
		double	aspectRatio;
		double	pixelSamplesScale;
		double	vfov;
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
		Ray		getRay(double u, double v) const;
		Color	rayColor(const Ray &ray, const GeometryData *geoms, const MaterialData *materials, int geomCount, int depth);
};

#endif /* CAMERA_HPP */
