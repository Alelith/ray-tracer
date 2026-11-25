#ifndef MESH_HPP
# define MESH_HPP

# include <vector>
# include <cstdint>
# include <string>
# include "Maths/Vec3.hpp"

struct Mesh {
	std::vector<Point3>		vertex;
	std::vector<uint32_t>	indexes;
};

inline bool		loadOBJ(const std::string& path, Mesh& mesh);

# include "detail/Objects/Mesh.ipp"

#endif // MESH_HPP
