#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Camera.hpp"
# include "HitRecord.hpp"
# include <memory>

using std::shared_ptr;

class Texture
{
  public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Point3 &p) const = 0;
};

class Material
{
	protected:
		shared_ptr<Texture>	albedo;
	public:
		virtual 		~Material() = default;
		virtual bool	scatter(const Ray &rayIn, const HitRecord &record, Color &attenuation, Ray &scattered) const { return false; }
		virtual Color	emmited(double u, double v, const Point3 &p) const { return Color::Zero; }
};

#endif /* MATERIAL_HPP */
