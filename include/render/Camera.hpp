#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Ray.hpp"
# include "RenderHit.hpp"
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

		void	initCamera(Vector3 rotation = Vector3(0, 0, -1));
	public:
		Camera();
		Camera(double aspectRatio, int width, int samplesPerPixel, int maxDepth, double vfov, Point3 origin, Vector3 rotation, double defocusAngle, double focalDistance, Color backgroundColor);
		virtual ~Camera();

		vector<vector<Color>> render(RenderHitList &world);
};

#endif /* CAMERA_HPP */
