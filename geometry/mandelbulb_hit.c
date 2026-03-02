/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mandelbulb_hit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvarea-k <bvarea-k@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 13:22:55 by acesteve          #+#    #+#             */
/*   Updated: 2026/03/02 10:38:22 by bvarea-k         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

double	mandelbulb_sdf(t_point3 p, void *attributes)
{
	t_mandelbulb	*mb;
	t_vec3			z;
	double			dr;
	double			r;
	double			theta;
	double			phi;
	double			zr;
	int				i;

	mb = (t_mandelbulb *)attributes;
	z = (t_vec3){p.x, p.y, p.z};
	dr = 1.0;
	r = 0.0;
	i = 0;
	while (i < mb->iterations)
	{
		r = sqrt(z.x * z.x + z.y * z.y + z.z * z.z);
		if (r > mb->bailout)
			break ;
		dr = pow(r, mb->power - 1.0) * mb->power * dr + 1.0;
		theta = acos(fmax(-1.0, fmin(1.0, z.z / r)));
		phi = atan2(z.y, z.x);
		zr = pow(r, mb->power);
		theta = theta * mb->power;
		phi = phi * mb->power;
		z.x = zr * sin(theta) * cos(phi) + p.x;
		z.y = zr * sin(theta) * sin(phi) + p.y;
		z.z = zr * cos(theta) + p.z;
		i++;
	}
	return (0.1 * log(r) * r / dr);
}

static short	mandelbulb_hit_internal(const t_ray r, t_limits limits,
			t_hit_record *rec, t_hittable mandelbulb_ptr)
{
	t_ray_marching	rm;
	short			best_index;

	rm.ray = r;
	rm.sdf = mandelbulb_sdf;
	rm.attributes = mandelbulb_ptr.attributes;
	ray_marching(&rm);
	best_index = get_best_solution_index(rm.solutions, limits, rec, 1);
	if (best_index == -1)
		return (0);
	rec->t = rm.solutions[best_index].real;
	return (1);
}

short	mandelbulb_hit(const t_ray r, t_limits limits,
			t_hit_record *rec, t_hittable mandelbulb_ptr)
{
	t_point3	local_p;
	t_vec3		normal;
	t_ray		local_ray;

	local_ray.dir = matrix_vector_multiply
		(mandelbulb_ptr.transform.inverse, r.dir, 0);
	local_ray.orig = matrix_vector_multiply
		(mandelbulb_ptr.transform.inverse, r.orig, 1);
	if (!mandelbulb_hit_internal(local_ray, limits, rec, mandelbulb_ptr))
		return (0);
	rec->material = mandelbulb_ptr.material;
	rec->p = at(r, rec->t);
	local_p = at(local_ray, rec->t);
	normal = get_sdf_normal(local_p, mandelbulb_sdf, mandelbulb_ptr.attributes);
	normal = matrix_vector_multiply
		(mandelbulb_ptr.transform.inverse_t, normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
