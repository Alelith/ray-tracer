#ifndef RENDER_HIT_HPP
# define RENDER_HIT_HPP

# include "Ray.hpp"
# include "Limits.hpp"
# include "HitRecord.hpp"
# include "Geometry.hpp"
# include "Material.hpp"
# include <vector>

using std::vector;

class RenderHit
{
	private:
		shared_ptr<Geometry>	geometry;
		Material				color;
	public:
		RenderHit();
		RenderHit(shared_ptr<Geometry> geometry, Color color);
		virtual ~RenderHit();

		virtual bool hit(const Ray &r, const Limits &limits, HitRecord &hitRecord) const;
};

class RenderHitList : public RenderHit
{
	private:
		vector<RenderHit> objects;
	public:
		RenderHitList();
		virtual ~RenderHitList();

		virtual bool hit(const Ray &r, const Limits &limits, HitRecord &hitRecord) const;
};

#endif /* RENDER_HIT_HPP */
