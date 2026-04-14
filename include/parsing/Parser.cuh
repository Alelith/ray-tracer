#ifndef PARSER_CUH
# define PARSER_CUH


#include "Point3.cuh"
#include "Vector3.cuh"
#include "Geometry.cuh"
#include "Material.cuh"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

namespace parsing
{

	using namespace maths;
	using namespace geometry;

	struct ParsedCameraData
	{
		Point3	position;
		Vector3	normal;
		float	aspectRatio;
		int		imageWidth;
		int		samplesPerPixel;
		int		maxDepth;
		float	fov;
		float	normalWeight;
		float	positionWeight;
		float	albedoWeight;
		float	colorWeight;
		bool	isParsed = false;
	};

	class Parser
	{
		public:
			Parser(const std::string& filepath);
			~Parser();

			bool parse();

			ParsedCameraData getCameraData() const { return cameraData; }
			WorldGeometries getWorldGeometries() const {
				WorldGeometries wg;
				wg.primitives = const_cast<Primitive*>(primitives.data());
				wg.count = primitives.size();
				return wg;
			}
			const Materials& getMaterials() const { return materialsList; }
		private:
			std::string			filepath;
			std::ifstream		file;

			ParsedCameraData	cameraData;
			PrimitivesList		primitives;
			Materials			materialsList = {};

			void parseLine(const std::string& line, int lineNumber);
			void parseOBJ(const std::string& objFilePath, const Point3& pos, const Vector3& norm, const Vector3& scale, const Color& albedo, float roughness, float metalness, float transmission, float refraction_idx, const Color& emissionColor, float emissionStrength, char texturesPaths[5][255]);
	};

} // namespace parsing

#endif /* PARSER_CUH */
