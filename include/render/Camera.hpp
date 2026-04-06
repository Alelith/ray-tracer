#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Material.hpp"
# include "LinearBVH.hpp"
# include <cmath>
# include <thread>

class Camera
{
	public:
		Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal);
		Camera(const Camera &other) = default;
		~Camera() = default;

		void			renderSample(const LinearBVH &geometries, const Materials &materials);
		vector<int>		getImage();
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

		Color	*buffer;
		int		iteration;

		void	initialize();
		Ray		getRay(float u, float v) const;
		Color	rayColor(const Ray &ray, const LinearBVH &geometries, const Materials &materials, int depth);
		void	renderLine(int start, int end, const LinearBVH &geometries, const Materials &materials);

};

#endif /* CAMERA_HPP */
