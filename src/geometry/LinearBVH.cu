#include "LinearBVH.cuh"

RT_CALLABLE int	LinearBVH::buildNode(vector<BVHData> &data, int start, int end)
{
	AABB nodeBox = data[start].box;
	for (int i = start + 1; i < end; i++)
		nodeBox = AABB(nodeBox, data[i].box);
	if (end - start == 1)
	{
		BVHData* newData = new BVHData[count + 1];
		for (size_t k = 0; k < count; k++)
			newData[k] = bvhData[k];
		newData[count] = data[start];
		delete[] bvhData;
		bvhData = newData;
		count++;

		return count - 1;
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

		int mid = bestSplit(data, start, end);
		
		BVHData nodeData;
		nodeData.geomIdx = -1; // Internal node
		nodeData.type = NONE;
		nodeData.rightChildIdx = 0;
		nodeData.box = nodeBox;

		BVHData* newData = new BVHData[count + 1];
		for (size_t k = 0; k < count; k++)
			newData[k] = bvhData[k];
		newData[count] = nodeData;
		delete[] bvhData;
		bvhData = newData;
		count++;

		int nodeIdx = count - 1;

		buildNode(data, start, mid);
		int rIdx = buildNode(data, mid, end);
		bvhData[nodeIdx].rightChildIdx = rIdx;
		return nodeIdx;
	}
}

RT_CALLABLE int	LinearBVH::bestSplit(vector<BVHData> &data, int start, int end)
{
	float minCost = INFINITY;
	int res = start + (end - start) / 2;

	for (int i = start; i < end - 1; i++)
	{
		AABB leftBox = data[start].box;
		for (int j = start + 1; j <= i; j++)
			leftBox = AABB(leftBox, data[j].box);
		
		AABB rightBox = data[i + 1].box;
		for (int j = i + 2; j < end; j++)
			rightBox = AABB(rightBox, data[j].box);
		
		float cost = leftBox.getArea() * (i - start + 1) + rightBox.getArea() * (end - i - 1);
		if (cost < minCost)
		{
			minCost = cost;
			res = i + 1;
		}
	}
	return res;
}

RT_CALLABLE LinearBVH::LinearBVH(const WorldGeometries &worldGeoms) 
	: bvhData(nullptr), count(0), worldGeoms(worldGeoms)
{
	vector<BVHData>	tmpData;

	for (size_t i = 0; i < worldGeoms.spheres.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = SPHERE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.spheres.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.planes.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = PLANE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.planes.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.cylinders.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = CYLINDER;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.cylinders.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.cones.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = CONE;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.cones.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.hyperboloids.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = HYPERBOLOID;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.hyperboloids.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.tori.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = TORUS;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.tori.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.kisses.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = KISS;
		data.rightChildIdx = -1; // No children in a linear BVH
		data.box = worldGeoms.kisses.boundingBox[i];
		tmpData.push_back(data);
	}
	for (size_t i = 0; i < worldGeoms.triangles.count; i++)
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

RT_CALLABLE static bool hitGeometry(const WorldGeometries &worldGeoms, const Ray &ray, const Limits &limits, HitRecord &record, const BVHData &geomData)
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
			hit = cylinderHits(worldGeoms.cylinders.inverseTransform[geomData.geomIdx], ray, limits, record);
			if (hit) record.geomIdx = worldGeoms.cylinders.geomIdx[geomData.geomIdx];
			return hit;
		case CONE:
			hit = coneHits(worldGeoms.cones.inverseTransform[geomData.geomIdx], ray, limits, record);
			if (hit) record.geomIdx = worldGeoms.cones.geomIdx[geomData.geomIdx];
			return hit;
		case HYPERBOLOID:
			// Implement hyperboloid hit logic
			break;
		case TORUS:
			hit = torusHits(worldGeoms.tori.inverseTransform[geomData.geomIdx], worldGeoms.tori.param[geomData.geomIdx], ray, limits, record);
			if (hit) record.geomIdx = worldGeoms.tori.geomIdx[geomData.geomIdx];
			return hit;
		case KISS:
			// Implement kiss hit logic
			break;
		case TRIANGLE:
			hit = triangleHits(
				worldGeoms.triangles.v0[geomData.geomIdx],
				worldGeoms.triangles.v1[geomData.geomIdx],
				worldGeoms.triangles.v2[geomData.geomIdx],
				worldGeoms.triangles.triNormal[geomData.geomIdx],
				ray,
				limits,
				record
			);
			if (hit) record.geomIdx = worldGeoms.triangles.geomIdx[geomData.geomIdx];
			return hit;
		default:
			return false;
	}
	return false;
}

RT_CALLABLE bool	LinearBVH::hit(const Ray &ray, const Limits &limits, HitRecord &record) const
{
	bool hitAnything = false;

	Limits currentLimits = limits;

	int nodesToVisit[64];
	int top = 0;
	nodesToVisit[top++] = 0; // Start with the root node

	while (top > 0)
	{
		int currentIdx = nodesToVisit[--top];

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
				nodesToVisit[top++] = currentNode.rightChildIdx;
				nodesToVisit[top++] = currentIdx + 1;
			}
		}
	}
	return hitAnything;
}

LinearBVH	LinearBVH::cudaCopy() const
{
	WorldGeometries cudaWorldGeoms;
	
	cudaWorldGeoms = worldGeoms.cudaCopy();

	LinearBVH cudaBVH;
	
	cudaBVH.count = count;
	cudaMalloc(&cudaBVH.bvhData, count * sizeof(BVHData));

	cudaMemcpy(cudaBVH.bvhData, bvhData, count * sizeof(BVHData), cudaMemcpyHostToDevice);

	cudaBVH.worldGeoms = cudaWorldGeoms;

	return cudaBVH;
}

void	LinearBVH::cudaFreeBVH()
{
	cudaFree(bvhData);
	worldGeoms.cudaFreeWorld();
}
