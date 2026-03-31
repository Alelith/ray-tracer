#include "LinearBVH.hpp"

int	LinearBVH::buildNode(vector<BVHData> &data, int start, int end)
{
	AABB nodeBox = data[start].box;
	for (int i = start + 1; i < end; i++)
		nodeBox = AABB(nodeBox, data[i].box);
	if (end - start == 1)
	{
		bvhData.push_back(data[start]);
		return bvhData.size() - 1;
	}
	else
	{
		int axis = 0;
		float xlen = nodeBox.getXLimits().getLength();
		float ylen = nodeBox.getYLimits().getLength();
		float zlen = nodeBox.getZLimits().getLength();
		if (ylen > xlen && ylen > zlen)
			axis = 1;
		else if (zlen > xlen && zlen > ylen)
			axis = 2;
		
		std::sort(data.begin() + start, data.begin() + end, [axis](const BVHData &a, const BVHData &b) {
			float aCenter, bCenter;
			if (axis == 0)
			{
				aCenter = a.box.getXLimits().getCenter();
				bCenter = b.box.getXLimits().getCenter();
			}
			else if (axis == 1)
			{
				aCenter = a.box.getYLimits().getCenter();
				bCenter = b.box.getYLimits().getCenter();
			}
			else
			{
				aCenter = a.box.getZLimits().getCenter();
				bCenter = b.box.getZLimits().getCenter();
			}
			return aCenter < bCenter;
		});

		int mid = start + (end - start) / 2;
		
		BVHData nodeData;
		nodeData.geomIdx = -1; // Internal node
		nodeData.type = NONE; // Internal node
		nodeData.box = nodeBox;
		bvhData.push_back(nodeData);

		int nodeIdx = bvhData.size() - 1;

		buildNode(data, start, mid);
		bvhData[nodeIdx].rightChildIdx = buildNode(data, mid, end);
		return nodeIdx;
	}
}

LinearBVH::LinearBVH(const WorldGeometries &worldGeoms) : worldGeoms(worldGeoms)
{
	vector<BVHData>	tmpData;
	for (size_t i = 0; i < worldGeoms.spheres.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = SPHERE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.spheres.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.planes.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = PLANE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.planes.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.cylinders.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = CYLINDER;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.cylinders.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.cones.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = CONE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.cones.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.hyperboloids.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = HYPERBOLOID;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.hyperboloids.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.tori.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = TORUS;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.tori.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.kisses.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = KISS;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.kisses.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.triangles.geomIdx.size(); i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = TRIANGLE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.triangles.boundingBox[i];
		tmpData.push_back(data);
	}
	buildNode(tmpData, 0, tmpData.size());
}

static bool hitGeometry(const WorldGeometries &worldGeoms, const Ray &ray, const Limits &limits, HitRecord &record, const BVHData &geomData)
{
	bool hit = false;
	switch (geomData.type)
	{
		case SPHERE:
			hit = sphereHits(worldGeoms.spheres.inverseTransform[geomData.geomIdx], ray, limits, record);
			if (hit) record.geomIdx = worldGeoms.spheres.geomIdx[geomData.geomIdx];
			return hit;
		case PLANE:
			// Implement plane hit logic
			break;
		case CYLINDER:
			// Implement cylinder hit logic
			break;
		case CONE:
			// Implement cone hit logic
			break;
		case HYPERBOLOID:
			// Implement hyperboloid hit logic
			break;
		case TORUS:
			// Implement torus hit logic
			break;
		case KISS:
			// Implement kiss hit logic
			break;
		case TRIANGLE:
			// Implement triangle hit logic
			break;
		default:
			return false;
	}
	return false;
}

bool	LinearBVH::hit(const Ray &ray, const Limits &limits, HitRecord &record) const
{
	bool hitAnything = false;

	Limits currentLimits = limits;

	stack<int> nodesToVisit;
	nodesToVisit.push(0); // Start with the root node

	while (!nodesToVisit.empty())
	{
		int currentIdx = nodesToVisit.top();
		nodesToVisit.pop();

		const BVHData &currentNode = bvhData[currentIdx];

		if (currentNode.box.hit(ray, currentLimits))
		{
			if (currentNode.geomIdx != -1)
			{
				if (hitGeometry(worldGeoms, ray, currentLimits, record, currentNode))
				{
					currentLimits = Limits(currentLimits.getMin(), record.t); // Update limits to find closest hit
					hitAnything = true;
				}
			}
			else
			{
				nodesToVisit.push(currentNode.rightChildIdx);
				nodesToVisit.push(currentIdx + 1);
			}
		}
	}
	return hitAnything;
}
