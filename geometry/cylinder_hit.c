/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_hit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 21:43:45 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/20 12:40:39 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static short	check_cap(t_ray r, double y_plane, t_limits limits, double *t)
{
	double		t_cap;
	t_point3	p;

	if (fabs(r.dir.y) < 1e-6)
		return (0);
	t_cap = (y_plane - r.orig.y) / r.dir.y;
	if (t_cap < limits.min || t_cap > limits.max)
		return (0);
	p = at(r, t_cap);
	if (p.x * p.x + p.z * p.z > 1.0)
		return (0);
	*t = t_cap;
	return (1);
}

static void	check_caps_for_best(t_ray r, t_limits limits,
									double *best_t, short *hit_cap)
{
	double	t_cap;

	*hit_cap = 0;
	if (check_cap(r, -0.5, limits, &t_cap))
	{
		if (*best_t < 0 || t_cap < *best_t)
		{
			*best_t = t_cap;
			*hit_cap = 1;
		}
	}
	if (check_cap(r, 0.5, limits, &t_cap))
	{
		if (*best_t < 0 || t_cap < *best_t)
		{
			*best_t = t_cap;
			*hit_cap = 2;
		}
	}
}

static short	cylinder_side_hit(t_ray r, t_limits limits,
									double *best_t, t_point3 *local_p)
{
	t_baskara	quad;
	int			i;

	quad.a = r.dir.x * r.dir.x + r.dir.z * r.dir.z;
	quad.b = 2.0 * (r.orig.x * r.dir.x + r.orig.z * r.dir.z);
	quad.c = r.orig.x * r.orig.x + r.orig.z * r.orig.z - 1.0;
	if (fabs(quad.a) < 1e-6)
		return (0);
	baskara(&quad);
	i = 0;
	while (i < 2)
	{
		if (quad.solutions[i].is_real && quad.solutions[i].real > limits.min
			&& quad.solutions[i].real < limits.max)
		{
			*local_p = at(r, quad.solutions[i].real);
			if (local_p->y >= -0.5 && local_p->y <= 0.5)
				if (*best_t < 0 || quad.solutions[i].real < *best_t)
					*best_t = quad.solutions[i].real;
		}
		i++;
	}
	return (*best_t > 0);
}

static short	cylinder_hit_internal(t_ray r, t_limits limits,
	t_hit_record *rec, t_hittable cylinder)
{
	double		best_t;
	short		hit_cap;
	t_point3	local_p;

	best_t = -1.0;
	hit_cap = 0;
	cylinder_side_hit(r, limits, &best_t, &local_p);
	check_caps_for_best(r, limits, &best_t, &hit_cap);
	if (best_t < 0)
		return (0);
	rec->t = best_t;
	local_p = at(r, rec->t);
	if (hit_cap == 1)
		rec->normal = (t_vec3){0, -1, 0};
	else if (hit_cap == 2)
		rec->normal = (t_vec3){0, 1, 0};
	else
		rec->normal = normalized((t_vec3){local_p.x, 0, local_p.z});
	return (1);
}

short	cylinder_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable cylinder_ptr)
{
	t_ray		local_ray;
	t_vec3		normal;

	local_ray.dir = matrix_vector_multiply(cylinder_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(cylinder_ptr.transform.inverse,
			r.orig, 1);
	if (!cylinder_hit_internal(local_ray, limits, rec, cylinder_ptr))
		return (0);
	rec->p = at(r, rec->t);
	normal = matrix_vector_multiply(cylinder_ptr.transform.inverse_t,
			rec->normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
