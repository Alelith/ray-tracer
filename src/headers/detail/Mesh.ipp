#include <fstream>
#include <sstream>

inline bool loadOBJ(const std::string& path, Mesh& mesh) {
	std::ifstream file(path);
	if (!file) return false;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string tag;
		ss >> tag;
		if (tag == "v") {
			Point3 p;
			ss >> p.x >> p.y >> p.z;
			mesh.vertex.push_back(p);
		}
		else if (tag == "f") {
			std::vector<uint32_t> face;
			std::string token;
			while (ss >> token) {
				int idx = std::stoi(token);
				face.push_back(idx > 0 ? static_cast<uint32_t>(idx - 1) : static_cast<uint32_t>(static_cast<int>(mesh.vertex.size()) + idx));
			}
			for (size_t k = 2; k < face.size(); k++) {
				mesh.indexes.push_back(face[0]);
				mesh.indexes.push_back(face[k - 1]);
				mesh.indexes.push_back(face[k]);
			}
		}
	}
	return !mesh.vertex.empty();
}
