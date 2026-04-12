#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

#include "maths/Point3.cuh"
#include "maths/Vector3.cuh"
#include "geometry/Geometry.cuh"
#include "render/Material.cuh"

// Estructura temporal para guardar los datos parseados de la cámara
struct ParsedCameraData {
	Point3 position;
	Vector3 normal;
	float aspectRatio;
	int imageWidth;
	int samplesPerPixel;
	int maxDepth;
	float fov;
	float normalWeight;
	float positionWeight;
	float albedoWeight;
	float colorWeight;
	bool isParsed = false;
};

class Parser {
	public:
		// Constructor que recibe la ruta del archivo
		Parser(const std::string& filepath);
		
		// Destructor para asegurar que el archivo se cierra
		~Parser();

		// Método principal para iniciar el parseo
		bool parse();

		ParsedCameraData getCameraData() const { return cameraData; }
		const WorldGeometries& getWorldGeometries() const { return worldGeometries; }
		const Materials& getMaterials() const { return materialsList; }

	private:
		std::string filepath;
		std::ifstream file;
		
		ParsedCameraData cameraData; // Datos de la cámara leídos del archivo
		WorldGeometries worldGeometries = {};
		Materials materialsList = {};

		// Método auxiliar para procesar una línea individual
		void parseLine(const std::string& line, int lineNumber);

		// Método auxiliar para parsear archivos .obj e inyectarlos como triángulos
		void parseOBJ(const std::string& objFilePath, 
					  const Point3& pos, const Vector3& norm, const Vector3& scale, 
					  const Color& albedo, float roughness, float metalness, float transmission, float refraction_idx, 
					  const Color& emissionColor, float emissionStrength, 
					  char texturesPaths[5][255]);
};
