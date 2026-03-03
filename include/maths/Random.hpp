#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.hpp"
# include <chrono>
# include <climits>

class Random
{
	private:
		static unsigned long	seed;

		static void			xorshift64();
	public:
		static double	randomDouble();
		static double	randomDouble(double min, double max);
		static Vector3	randomVector3();
		static Vector3	randomVector3(double min, double max);
		static Vector3	randomUnitVector3();
		static Vector3	randomInHemisphere(const Vector3 &normal);
		static Vector3	randomInUnitSphere();
		static Vector3	randomInUnitDisk();
};

#endif /* RANDOM_HPP */
