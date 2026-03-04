#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Ray.hpp"
# include "BVH.hpp"
# include <vector>

using std::vector;

class Camera
{
	private:
		double	aspectRatio;
		double	pixelSamplesScale;
		double 	vfov;
		double	defocusAngle;
		double	focalDistance;
		int		width;
		int		height;
		int		samplesPerPixel;
		int		depth;
		int		maxDepth;
		Point3	origin;
		Point3	pixel00Loc;
		Vector3	pixelDeltaU;
		Vector3	pixelDeltaV;
		Vector3	defocusDiskU;
		Vector3	defocusDiskV;
		Color	backgroundColor;

		void	initCamera(const Vector3 &rotation = Vector3(0, 0, -1));
	public:
		Camera();
		Camera(double aspectRatio, int width, int samplesPerPixel, int maxDepth, double vfov, const Point3 &origin, const Vector3 &rotation, double defocusAngle, double focalDistance, const Color &backgroundColor);
		virtual ~Camera();

		vector<vector<Color>> render(const GeometryData *world);
};

#endif /* CAMERA_HPP */
