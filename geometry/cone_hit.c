/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone_hit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvarea-k <bvarea-k@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 15:36:55 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/23 16:01:00 by bvarea-k         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static short	check_base(t_ray r, t_limits limits, double *t)
{
	double		t_cap;
	t_point3	p;

	if (fabs(r.dir.y) < 1e-6)
		return (0);
	t_cap = (-0.5 - r.orig.y) / r.dir.y;
	if (t_cap < limits.min || t_cap > limits.max)
		return (0);
	p = at(r, t_cap);
	if (p.x * p.x + p.z * p.z > 1.0)
		return (0);
	*t = t_cap;
	return (1);
}

static t_baskara	calculate_coeff(t_ray r)
{
	t_baskara	quad;

	quad.a = r.dir.x * r.dir.x + r.dir.z * r.dir.z - r.dir.y * r.dir.y;
	quad.b = 2.0 * (r.orig.x * r.dir.x + r.orig.z * r.dir.z
			- (r.orig.y - 0.5) * r.dir.y);
	quad.c = r.orig.x * r.orig.x + r.orig.z * r.orig.z
		- (r.orig.y - 0.5) * (r.orig.y - 0.5);
	return (quad);
}

static short	cone_surface_hit(t_ray r, t_limits limits,
									double *best_t, t_point3 *local_p)
{
	t_baskara	quad;
	int			i;
	double		y_cone;

	quad = calculate_coeff(r);
	if (fabs(quad.a) < 1e-6 && fabs(quad.b) < 1e-6)
		return (0);
	baskara(&quad);
	i = 0;
	while (i < 2)
	{
		if (quad.solutions[i].is_real && quad.solutions[i].real > limits.min
			&& quad.solutions[i].real < limits.max)
		{
			*local_p = at(r, quad.solutions[i].real);
			y_cone = local_p->y - 0.5;
			if (y_cone <= 0.0 && y_cone >= -1.0)
				if (*best_t < 0 || quad.solutions[i].real < *best_t)
					*best_t = quad.solutions[i].real;
		}
		i++;
	}
	return (*best_t > 0);
}

static short	cone_hit_internal(const t_ray r, t_limits limits,
										t_hit_record *rec, t_hittable cone)
{
	double		best_t;
	double		t_base;
	t_point3	local_p;
	short		hit_base;

	best_t = -1.0;
	hit_base = 0;
	cone_surface_hit(r, limits, &best_t, &local_p);
	if (check_base(r, limits, &t_base))
	{
		if (best_t < 0 || t_base < best_t)
		{
			best_t = t_base;
			hit_base = 1;
		}
	}
	if (best_t < 0)
		return (0);
	rec->t = best_t;
	local_p = at(r, rec->t);
	if (hit_base)
		rec->normal = (t_vec3){0, -1, 0};
	else
		rec->normal = vector(local_p.x, -(local_p.y - 0.5), local_p.z);
	return (1);
}

short	cone_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable cone_ptr)
{
	t_ray		local_ray;
	t_vec3		normal;

	local_ray.dir = matrix_vector_multiply(cone_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(cone_ptr.transform.inverse,
			r.orig, 1);
	if (!cone_hit_internal(local_ray, limits, rec, cone_ptr))
		return (0);
	rec->p = at(r, rec->t);
	normal = matrix_vector_multiply(cone_ptr.transform.inverse_t,
			rec->normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
