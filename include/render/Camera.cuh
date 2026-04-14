#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Material.cuh"
# include "LinearBVH.cuh"

struct RenderData
{
	int		width;
	int		height;
	int		maxDepth;
	Point3	cameraCenter;
	Point3	pixel00Loc;
	Vector3	pixelDeltaU;
	Vector3	pixelDeltaV;

	Vector3	*normalBuffer;
	Point3	*positionBuffer;
	Color	*albedoBuffer;
	Color	*buffer;
	Color	*pingPongBuffer;
	Color	*colorBuffer;

	Material	*materials;
	LinearBVH	*linearbvh;

	float		normalWeight;
	float		positionWeight;
	float 		albedoWeight;
	float		colorWeight;

	RT_CALLABLE Ray		getRay(float u, float v, Random &rand, int iteration) const;
};
class Camera
{
	public:
		Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal, const LinearBVH &geometries, const Materials &materials, float normalWeight = 0.25f, float positionWeight = 0.25f, float albedoWeight = 0.25f, float colorWeight = 0.25f);
		Camera(const Camera &other) = default;
		~Camera();

		void			renderSample();
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
		int		iteration;

		RenderData	cudaRenderData;
		RenderData	*allocatedCudaRenderData;

		void	initialize(const LinearBVH &geometries, const Materials &materials, float normalWeight, float positionWeight, float albedoWeight, float colorWeight);
};

#endif /* CAMERA_HPP */
