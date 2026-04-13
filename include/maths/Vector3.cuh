/**
 * @file Vector3.cuh
 * @brief 3D Vector class and mathematical operations
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef VECTOR3_HPP
# define VECTOR3_HPP

# include <cmath>
# include <ostream>

/**
 * @def RT_CALLABLE
 * @brief Macro for device/host callable functions
 * 
 * Defines whether a function should be compiled for both host (CPU) and device (GPU).
 * 
 * @ingroup math_module
 */
# ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
# else
#  define RT_CALLABLE
# endif

/**
 * @def RT_KERNEL
 * @brief Macro for device kernels
 * 
 * Defines whether a function should be compiled as a CUDA kernel.
 * 
 * @ingroup math_module
 */
# ifdef __CUDACC__
#  define RT_KERNEL __global__
# else
#  define RT_KERNEL
# endif

/**
 * @def M_PI_F
 * @brief Float representation of Pi
 * 
 * Used for trigonometric computations and angular conversions.
 * 
 * @ingroup math_module
 */
# ifndef M_PI_F
#  define M_PI_F 3.14159265358979323846f
# endif

/**
 * @class Vector3
 * @brief Mathematical 3D vector and color representation
 * 
 * This class provides standard vector operations such as addition, 
 * subtraction, multiplication, dot product, cross product, reflection,
 * and refraction. It is used for spatial directions, displacements, and colors.
 * 
 * @ingroup math_module
 */
class Vector3
{
	private:
		float	v[3]; ///< Array storing x, y, and z components
	public:
		RT_CALLABLE Vector3();
		RT_CALLABLE Vector3(float x, float y, float z);
		RT_CALLABLE Vector3(const Vector3 &other) = default;
		RT_CALLABLE ~Vector3() = default;

		RT_CALLABLE Vector3			&operator=(const Vector3 &other) = default;

		inline RT_CALLABLE Vector3	operator-() const { return Vector3(-v[0], -v[1], -v[2]); }
		inline RT_CALLABLE float	&operator[](int index) { return v[index]; }
		inline RT_CALLABLE float	operator[](int index) const { return v[index]; }

		inline RT_CALLABLE Vector3 	operator+=(const Vector3 &other) { v[0] += other[0]; v[1] += other[1]; v[2] += other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator-=(const Vector3 &other) { v[0] -= other[0]; v[1] -= other[1]; v[2] -= other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator*=(const Vector3 &other) { v[0] *= other[0]; v[1] *= other[1]; v[2] *= other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator*=(float scalar) { v[0] *= scalar; v[1] *= scalar; v[2] *= scalar; return *this; }
		inline RT_CALLABLE Vector3 	operator/=(float scalar) { v[0] /= scalar; v[1] /= scalar; v[2] /= scalar; return *this; }

		/**
		 * @brief Calculates the squared length of the vector
		 * @return The squared length (magnitude)
		 */
		inline RT_CALLABLE float	lengthSqrd() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
		
		/**
		 * @brief Calculates the actual length of the vector
		 * @return The length (magnitude)
		 */
		inline RT_CALLABLE float	length() const { return sqrtf(lengthSqrd()); }
		
		/**
		 * @brief Returns a normalized copy of the vector
		 * @return A new vector with length 1, or zero vector if length is 0
		 */
		inline RT_CALLABLE Vector3	normalized() const { float len = length(); return (len != 0.0f) ? Vector3(v[0] / len, v[1] / len, v[2] / len) : Vector3(0.0f, 0.0f, 0.0f); }

		/**
		 * @brief Checks if the vector is close to zero
		 * @return true if vector is near zero, false otherwise
		 */
		inline RT_CALLABLE bool		isNearZero() const { return lengthSqrd() < 1e-6f; }

		/**
		 * @brief Clamps vector components between a min and max value
		 * 
		 * @param[in] minVal Minimum boundary value
		 * @param[in] maxVal Maximum boundary value
		 * @return A new clamped vector
		 */
		inline RT_CALLABLE Vector3	clamp(float minVal, float maxVal) const
		{
			return Vector3(fmaxf(minVal, fminf(maxVal, v[0])),
						 fmaxf(minVal, fminf(maxVal, v[1])),
						 fmaxf(minVal, fminf(maxVal, v[2])));
		}

		inline RT_CALLABLE float	x() const { return v[0]; }
		inline RT_CALLABLE float	y() const { return v[1]; }
		inline RT_CALLABLE float	z() const { return v[2]; }
};

using Color = Vector3;

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec);

/**
 * @brief Adds two vectors
 * 
 * @ingroup math_module
 * 
 * @param[in] vec1 First vector
 * @param[in] vec2 Second vector
 * @return Sum of the vectors
 */
inline RT_CALLABLE Vector3	operator+(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() + vec2.x(), vec1.y() + vec2.y(), vec1.z() + vec2.z()); }

/**
 * @brief Subtracts two vectors
 * 
 * @ingroup math_module
 * 
 * @param[in] vec1 First vector
 * @param[in] vec2 Second vector
 * @return Difference between the vectors
 */
inline RT_CALLABLE Vector3	operator-(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() - vec2.x(), vec1.y() - vec2.y(), vec1.z() - vec2.z()); }

/**
 * @brief Multiplies two vectors piece-wise
 * 
 * @ingroup math_module
 * 
 * @param[in] vec1 First vector
 * @param[in] vec2 Second vector
 * @return Piece-wise product vector
 */
inline RT_CALLABLE Vector3	operator*(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() * vec2.x(), vec1.y() * vec2.y(), vec1.z() * vec2.z()); }

/**
 * @brief Multiplies vector by a scalar
 * 
 * @ingroup math_module
 * 
 * @param[in] scalar The scaling factor
 * @param[in] vec The vector
 * @return Scaled vector
 */
inline RT_CALLABLE Vector3	operator*(float scalar, const Vector3 &vec) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }

/**
 * @brief Multiplies vector by a scalar
 * 
 * @ingroup math_module
 * 
 * @param[in] vec The vector
 * @param[in] scalar The scaling factor
 * @return Scaled vector
 */
inline RT_CALLABLE Vector3	operator*(const Vector3 &vec, float scalar) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }

/**
 * @brief Divides vector by a scalar
 * 
 * @ingroup math_module
 * 
 * @param[in] vec The vector
 * @param[in] scalar The scaling factor
 * @return Scaled vector
 */
inline RT_CALLABLE Vector3	operator/(const Vector3 &vec, float scalar) { return Vector3(vec.x() / scalar, vec.y() / scalar, vec.z() / scalar); }

/**
 * @brief Calculates dot product of two vectors
 * 
 * @ingroup math_module
 * 
 * @param[in] vec1 First vector
 * @param[in] vec2 Second vector
 * @return Dot product result
 */
inline RT_CALLABLE float	dot(const Vector3 &vec1, const Vector3 &vec2) { return vec1.x() * vec2.x() + vec1.y() * vec2.y() + vec1.z() * vec2.z(); }

/**
 * @brief Calculates cross product of two vectors
 * 
 * @ingroup math_module
 * 
 * @param[in] vec1 First vector
 * @param[in] vec2 Second vector
 * @return Cross product result
 */
inline RT_CALLABLE Vector3	cross(const Vector3 &vec1, const Vector3 &vec2)
{
	return Vector3(vec1.y() * vec2.z() - vec1.z() * vec2.y(),
					vec1.z() * vec2.x() - vec1.x() * vec2.z(),
					vec1.x() * vec2.y() - vec1.y() * vec2.x());
}

/**
 * @brief Calculates vector reflection
 * 
 * @ingroup math_module
 * 
 * @param[in] vec Incoming vector
 * @param[in] normal Standard normal vector
 * @return Reflected vector
 */
inline RT_CALLABLE Vector3	reflect(const Vector3 &vec, const Vector3 &normal) { return vec - 2 * dot(vec, normal) * normal; }

/**
 * @brief Calculates vector refraction using Snell's law
 * 
 * @ingroup math_module
 * 
 * @param[in] uv Normalized incoming vector
 * @param[in] normal Standard normal vector
 * @param[in] eta_over_eta_prime Refraction index ratio
 * @return Refracted vector
 */
inline RT_CALLABLE Vector3	refract(const Vector3 &uv, const Vector3 &normal, float eta_over_eta_prime)
{
	float cos_theta = fminf(dot(-uv, normal), 1.0f);
	Vector3 r_out_perp = eta_over_eta_prime * (uv + cos_theta * normal);
	Vector3 r_out_parallel = -sqrtf(fabsf(1.0f - r_out_perp.lengthSqrd())) * normal;
	return r_out_perp + r_out_parallel;
}

/**
 * @brief Approximates reflectance using Schlick's approximation
 * 
 * @ingroup math_module
 * 
 * @param[in] cosine The cosine of the incoming angle
 * @param[in] ref_idx The refraction index
 * @return Reflection coefficient
 */
inline RT_CALLABLE float	reflectance(float cosine, float ref_idx)
{
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}

#endif /* VECTOR3_HPP */
