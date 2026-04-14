#include "LinearBVH.cuh"
#include <algorithm>
RT_CALLABLE int	LinearBVH::buildNode(vector<BVHData> &data, int start, int end)
{
	AABB nodeBox = data[start].box;
	for (int i = start + 1; i < end; i++)
		nodeBox = AABB(nodeBox, data[i].box);
	if (end - start == 1)
	{
		bvhData[count] = data[start];
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
		std::sort(data.begin() + start, data.begin() + end, [axis](const BVHData &a, const BVHData &b)
		{
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
		nodeData.geomIdx = -1; 
		nodeData.type = NONE;
		nodeData.rightChildIdx = 0;
		nodeData.box = nodeBox;
		int nodeIdx = count;
		bvhData[count] = nodeData;
		count++;
		buildNode(data, start, mid);
		int rIdx = buildNode(data, mid, end);
		bvhData[nodeIdx].rightChildIdx = rIdx;
		return nodeIdx;
	}
}
RT_CALLABLE int	LinearBVH::bestSplit(vector<BVHData> &data, int start, int end)
{
	int n = end - start;
	if (n <= 1) return start;
	AABB* rightBoxes = new AABB[n];
	rightBoxes[n - 1] = data[end - 1].box;
	for (int i = n - 2; i >= 0; i--)
		rightBoxes[i] = AABB(rightBoxes[i + 1], data[start + i].box);
	float minCost = INFINITY;
	int res = start + n / 2;
	AABB leftBox = data[start].box;
	for (int i = 0; i < n - 1; i++)
	{
		leftBox = AABB(leftBox, data[start + i].box);
		float cost = leftBox.getArea() * (i + 1) + rightBoxes[i + 1].getArea() * (n - i - 1);
		if (cost < minCost)
		{
			minCost = cost;
			res = start + i + 1;
		}
	}
	delete[] rightBoxes;
	return res;
}
RT_CALLABLE LinearBVH::LinearBVH(const WorldGeometries &worldGeoms) : bvhData(nullptr), count(0), worldGeoms(worldGeoms)
{
	vector<BVHData> tmpData;
	for (size_t i = 0; i < worldGeoms.count; i++)
	{
		BVHData data;
		data.geomIdx = i;
		data.type = worldGeoms.primitives[i].type;
		data.rightChildIdx = -1; 
		data.box = worldGeoms.primitives[i].boundingBox;
		tmpData.push_back(data);
	}
	if (tmpData.empty()) return;
	bvhData = new BVHData[tmpData.size() * 2];
	buildNode(tmpData, 0, tmpData.size());
}
RT_CALLABLE static bool	hitGeometry(const Primitive &primitive, const Ray &ray, const Limits &limits, HitRecord &record, const BVHData &geomData)
{
	bool hit = false;
	switch (primitive.type)
	{
		case SPHERE: hit = sphereHits(primitive.inverseTransform, ray, limits, record); break;
		case CYLINDER: hit = cylinderHits(primitive.inverseTransform, ray, limits, record); break;
		case CONE: hit = coneHits(primitive.inverseTransform, ray, limits, record); break;
		case TORUS: hit = torusHits(primitive.inverseTransform, primitive.param, ray, limits, record); break;
		case TRIANGLE: hit = triangleHits(primitive.data.tris.v0, primitive.data.tris.v1, primitive.data.tris.v2, primitive.data.tris.normal, ray, limits, record); break;
		case QUAD: hit = quadHits(primitive.data.quad.Q, primitive.data.quad.u, primitive.data.quad.v, primitive.data.quad.w, primitive.data.quad.normal, primitive.data.quad.D, ray, limits, record); break;
		default: return false;
	}
	if (hit) record.geomIdx = primitive.geomIdx;
	return hit;
}
RT_CALLABLE bool	LinearBVH::hit(const Ray &ray, const Limits &limits, HitRecord &record) const
{
	bool hitAnything = false;
	Limits currentLimits = limits;
	int nodesToVisit[64];
	int top = 0;
	nodesToVisit[top++] = 0; 
	while (top > 0)
	{
		int currentIdx = nodesToVisit[--top];
		const BVHData &currentNode = bvhData[currentIdx];
		if (currentNode.box.hit(ray, currentLimits))
		{
			if (currentNode.geomIdx != -1)
			{
				if (hitGeometry(worldGeoms.primitives[currentNode.geomIdx], ray, currentLimits, record, currentNode))
				{
					currentLimits = Limits(currentLimits.getMin(), record.t); 
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