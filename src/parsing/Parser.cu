#include "Parser.cuh"
#include "Matrix4x4.cuh"
#include <sstream>
#include <cstdio>
#include <cstdlib>

namespace parsing
{
	namespace
	{
		bool parseVector3Token(const std::string& token, float &x, float &y, float &z) { return sscanf(token.c_str(), "%f,%f,%f", &x, &y, &z) == 3; }
	}

	Parser::Parser(const std::string& filepath) : filepath(filepath)
	{
		if (filepath.length() < 5 || filepath.substr(filepath.length() - 5) != ".lrtc")
			std::cerr << "Advertencia: El archivo indicado no tiene extensión '.lrtc': " << filepath << std::endl;
		file.open(filepath);
		if (!file.is_open())
			std::cerr << "Error: No se pudo abrir el archivo " << filepath << std::endl;
	}
	Parser::~Parser() { if (file.is_open()) file.close(); }

	bool	Parser::parse()
	{
		if (!file.is_open())
			return false;

		std::string line;
		int lineNumber = 1;
		while (std::getline(file, line))
		{
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			size_t firstNonSpace = line.find_first_not_of(" \t\v\f\r");
			if (line.empty() || firstNonSpace == std::string::npos || line[firstNonSpace] == '#') {
				lineNumber++;
				continue;
			}

			parseLine(line, lineNumber);
			lineNumber++;
		}
		return true;
	}

	void Parser::parseLine(const std::string& line, int lineNumber)
	{
		std::stringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "C")
		{
			std::string posToken, normToken;
			float px, py, pz, nx, ny, nz;

			if (!(ss >> posToken >> normToken >> cameraData.aspectRatio >> cameraData.imageWidth >> cameraData.samplesPerPixel >> cameraData.maxDepth >> cameraData.fov >> cameraData.normalWeight >> cameraData.positionWeight >> cameraData.albedoWeight >> cameraData.colorWeight))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Cámara): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(normToken, nx, ny, nz))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Cámara): vectores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			cameraData.position = Point3(px, py, pz);
			cameraData.normal = Vector3(nx, ny, nz);
			cameraData.isParsed = true;
			std::cout << "Cámara configurada correctamente (" << cameraData.imageWidth << " px, FOV " << cameraData.fov << ")" << std::endl;
		}
		else if (type == "sp")
		{
			std::string posToken, albedoToken, emissionToken;
			float px, py, pz, radius;
			float r, g, b, roughness, metalness, transmission, refraction_idx;
			float er, eg, eb, emission_strength;

			if (!(ss >> posToken >> radius >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Esfera): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(albedoToken, r, g, b) || !parseVector3Token(emissionToken, er, eg, eb))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Esfera): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			char texturesPaths[5][255];
			for (int i = 0; i < 5; ++i)
				texturesPaths[i][0] = '\0';

			std::string token;
			while (ss >> token)
			{
				if (token.length() > 2 && token[1] == ':')
				{
					int texIndex = -1;
					char prefix = token[0];
					if (prefix == 'a')      texIndex = 0;
					else if (prefix == 'r') texIndex = 1;
					else if (prefix == 'm') texIndex = 2;
					else if (prefix == 'o') texIndex = 3;
					else if (prefix == 'e') texIndex = 4;

					if (texIndex != -1)
					{
						std::string path = token.substr(2);
						strncpy(texturesPaths[texIndex], path.c_str(), 254);
						texturesPaths[texIndex][254] = '\0'; 
					}
				}
			}

			makeSphere(Point3(px, py, pz), radius, primitives);
			addMaterial(Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths, materialsList);
			std::cout << "Esfera creada correctamente en la línea " << lineNumber << std::endl;
		}
		else if (type == "cy" || type == "co")
		{
			std::string posToken, normToken, albedoToken, emissionToken;
			float px, py, pz, nx, ny, nz;
			float radius, height;
			float r, g, b, roughness, metalness, transmission, refraction_idx;
			float er, eg, eb, emission_strength;

			if (!(ss >> posToken >> normToken >> radius >> height >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (" << (type == "cy" ? "Cilindro" : "Cono") << "): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(normToken, nx, ny, nz) || 
				!parseVector3Token(albedoToken, r, g, b) || !parseVector3Token(emissionToken, er, eg, eb))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (" << (type == "cy" ? "Cilindro" : "Cono") << "): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			char texturesPaths[5][255];
			for (int i = 0; i < 5; ++i)
				texturesPaths[i][0] = '\0';

			std::string token;
			while (ss >> token)
			{
				if (token.length() > 2 && token[1] == ':')
				{
					int texIndex = -1;
					char prefix = token[0];
					if (prefix == 'a')      texIndex = 0; 
					else if (prefix == 'r') texIndex = 1; 
					else if (prefix == 'm') texIndex = 2; 
					else if (prefix == 'o') texIndex = 3; 
					else if (prefix == 'e') texIndex = 4;

					if (texIndex != -1)
					{
						std::string path = token.substr(2);
						strncpy(texturesPaths[texIndex], path.c_str(), 254);
						texturesPaths[texIndex][254] = '\0';
					}
				}
			}

			if (type == "cy")
				makeCylinder(radius, height, Point3(px, py, pz), Vector3(nx, ny, nz), primitives);
			else
				makeCone(radius, height, Point3(px, py, pz), Vector3(nx, ny, nz), primitives);

			addMaterial(Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths, materialsList);
			std::cout << (type == "cy" ? "Cilindro" : "Cono") << " creado correctamente en la línea " << lineNumber << std::endl;
		}
		else if (type == "to")
		{
			std::string posToken, normToken, albedoToken, emissionToken;
			float px, py, pz, nx, ny, nz;
			float minor_radius, major_radius;
			float r, g, b, roughness, metalness, transmission, refraction_idx;
			float er, eg, eb, emission_strength;

			if (!(ss >> posToken >> normToken >> minor_radius >> major_radius >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Toroide): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(normToken, nx, ny, nz) || !parseVector3Token(albedoToken, r, g, b) || !parseVector3Token(emissionToken, er, eg, eb))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Toroide): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			char texturesPaths[5][255];
			for (int i = 0; i < 5; ++i)
				texturesPaths[i][0] = '\0';

			std::string token;
			while (ss >> token)
			{
				if (token.length() > 2 && token[1] == ':')
				{
					int texIndex = -1;
					char prefix = token[0];
					if (prefix == 'a')      texIndex = 0; 
					else if (prefix == 'r') texIndex = 1; 
					else if (prefix == 'm') texIndex = 2; 
					else if (prefix == 'o') texIndex = 3; 
					else if (prefix == 'e') texIndex = 4;

					if (texIndex != -1)
					{
						std::string path = token.substr(2);
						strncpy(texturesPaths[texIndex], path.c_str(), 254);
						texturesPaths[texIndex][254] = '\0';
					}
				}
			}

			makeTorus(major_radius, minor_radius, Point3(px, py, pz), Vector3(nx, ny, nz), primitives);
			addMaterial(Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths, materialsList);
			std::cout << "Toroide creado correctamente en la línea " << lineNumber << std::endl;
		}
		else if (type == "qd")
		{
			std::string posToken, uToken, vToken, albedoToken, emissionToken;
			float px, py, pz, ux, uy, uz, vx, vy, vz;
			float r, g, b, roughness, metalness, transmission, refraction_idx;
			float er, eg, eb, emission_strength;

			if (!(ss >> posToken >> uToken >> vToken >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Quad): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(uToken, ux, uy, uz) || !parseVector3Token(vToken, vx, vy, vz) || !parseVector3Token(albedoToken, r, g, b) || !parseVector3Token(emissionToken, er, eg, eb))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (Quad): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			char texturesPaths[5][255];
			for (int i = 0; i < 5; ++i)
				texturesPaths[i][0] = '\0';

			std::string token;
			while (ss >> token)
			{
				if (token.length() > 2 && token[1] == ':')
				{
					int texIndex = -1;
					char prefix = token[0];
					if (prefix == 'a')      texIndex = 0; 
					else if (prefix == 'r') texIndex = 1; 
					else if (prefix == 'm') texIndex = 2; 
					else if (prefix == 'o') texIndex = 3; 
					else if (prefix == 'e') texIndex = 4;

					if (texIndex != -1)
					{
						std::string path = token.substr(2);
						strncpy(texturesPaths[texIndex], path.c_str(), 254);
						texturesPaths[texIndex][254] = '\0';
					}
				}
			}

			makeQuad(Point3(px, py, pz), Vector3(ux, uy, uz), Vector3(vx, vy, vz), primitives);
			addMaterial(Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths, materialsList);
			std::cout << "Quad creado correctamente en la línea " << lineNumber << std::endl;
		}
		else if (type == "obj")
		{
			std::string filePath, posToken, normToken, scaleToken, albedoToken, emissionToken;
			float px, py, pz, nx, ny, nz, sx, sy, sz;
			float r, g, b, roughness, metalness, transmission, refraction_idx;
			float er, eg, eb, emission_strength;

			if (!(ss >> filePath >> posToken >> normToken >> scaleToken >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (OBJ): argumentos insuficientes. " << line << std::endl;
				return;
			}

			if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(normToken, nx, ny, nz) || !parseVector3Token(scaleToken, sx, sy, sz) || !parseVector3Token(albedoToken, r, g, b) ||  !parseVector3Token(emissionToken, er, eg, eb))
			{
				std::cerr << "Error de formato en la línea " << lineNumber << " (OBJ): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
				return;
			}

			char texturesPaths[5][255];
			for (int i = 0; i < 5; ++i)
				texturesPaths[i][0] = '\0';

			std::string token;
			while (ss >> token)
			{
				if (token.length() > 2 && token[1] == ':')
				{
					int texIndex = -1;
					char prefix = token[0];
					if (prefix == 'a')      texIndex = 0; 
					else if (prefix == 'r') texIndex = 1; 
					else if (prefix == 'm') texIndex = 2; 
					else if (prefix == 'o') texIndex = 3; 
					else if (prefix == 'e') texIndex = 4;

					if (texIndex != -1)
					{
						std::string path = token.substr(2);
						strncpy(texturesPaths[texIndex], path.c_str(), 254);
						texturesPaths[texIndex][254] = '\0';
					}
				}
			}

			parseOBJ(filePath, Point3(px, py, pz), Vector3(nx, ny, nz), Vector3(sx, sy, sz), Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths);
			std::cout << "Modelo OBJ parseado e importado correctamente en la línea " << lineNumber << std::endl;
		}
		else
			std::cout << "Línea " << lineNumber << " ignorada (Tipo no implementado: " << type << "): " << line << std::endl;
	}

	void Parser::parseOBJ(const std::string& objFilePath, const Point3& pos, const Vector3& norm, const Vector3& scale, const Color& albedo, float roughness, float metalness, float transmission, float refraction_idx, const Color& emissionColor, float emissionStrength, char texturesPaths[5][255])
	{
		std::ifstream objFile(objFilePath);
		if (!objFile.is_open())
		{
			std::cerr << "Error: No se pudo abrir archivo .obj en la ruta: " << objFilePath << std::endl;
			return;
		}

		Matrix4x4 tMatrix = masterMatrix(Vector3(pos.x(), pos.y(), pos.z()), norm, scale);
		std::vector<Point3> vertices;
		std::string line;
		std::vector<Point3> out_v0, out_v1, out_v2;
		std::vector<Vector3> out_normal;
		std::vector<AABB> out_aabb;

		while (std::getline(objFile, line))
		{
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			size_t firstNonSpace = line.find_first_not_of(" \t\v\f\r");
			if (line.empty() || firstNonSpace == std::string::npos || line[firstNonSpace] == '#')
				continue;

			std::stringstream ss(line);
			std::string token;
			ss >> token;
			if (token == "v")
			{
				float x, y, z;
				if (ss >> x >> y >> z)
				{
					Point3 worldVertex = tMatrix * Point3(x, y, z);
					vertices.push_back(worldVertex);
				}
			}
			else if (token == "f")
			{
				std::vector<int> faceVerts;
				std::string vt_token;
				while (ss >> vt_token)
				{
					size_t slashPos = vt_token.find('/');
					int v_idx = 0;

					if (slashPos != std::string::npos)
						v_idx = std::atoi(vt_token.substr(0, slashPos).c_str());
					else
						v_idx = std::atoi(vt_token.c_str());
					if (v_idx > 0)
						v_idx -= 1; 
					else if (v_idx < 0)
						v_idx = (int)vertices.size() + v_idx;
					faceVerts.push_back(v_idx);
				}
				if (faceVerts.size() >= 3)
				{
					Point3 v0 = vertices[faceVerts[0]];
					for (size_t i = 1; i < faceVerts.size() - 1; ++i)
					{
						Point3 v1 = vertices[faceVerts[i]];
						Point3 v2 = vertices[faceVerts[i+1]];
						Vector3 faceNormal = cross(v1 - v0, v2 - v0).normalized();

						out_v0.push_back(v0);
						out_v1.push_back(v1);
						out_v2.push_back(v2);
						out_normal.push_back(faceNormal);

						Point3 minP(fminf(v0.x(), fminf(v1.x(), v2.x())), fminf(v0.y(), fminf(v1.y(), v2.y())), fminf(v0.z(), fminf(v1.z(), v2.z())));
						Point3 maxP(fmaxf(v0.x(), fmaxf(v1.x(), v2.x())), fmaxf(v0.y(), fmaxf(v1.y(), v2.y())), fmaxf(v0.z(), fmaxf(v1.z(), v2.z())));
						
						out_aabb.push_back(AABB(minP, maxP));
					}
				}
			}
		}
		size_t numNewTriangles = out_v0.size();
		if (numNewTriangles == 0) return;

		Materials tempMatList = {};
		tempMatList.materials = nullptr;
		tempMatList.count = 0;
		addMaterial(albedo, roughness, metalness, transmission, refraction_idx, emissionColor, emissionStrength, texturesPaths, tempMatList);

		Material baseMaterial = tempMatList.materials[0];
		delete[] tempMatList.materials; 
		for (size_t i = 0; i < numNewTriangles; ++i)
			makeTriangle(out_v0[i], out_v1[i], out_v2[i], out_normal[i], primitives);

		size_t curMCount = materialsList.count;
		size_t newMCount = curMCount + numNewTriangles;
		Material* newMats = new Material[newMCount];

		for (size_t i = 0; i < curMCount; ++i)
			newMats[i] = materialsList.materials[i];
		for (size_t i = 0; i < numNewTriangles; ++i)
			newMats[curMCount + i] = baseMaterial;
			
		delete[] materialsList.materials;
		materialsList.materials = newMats;
		materialsList.count = newMCount;
	}

} // namespace parsing
