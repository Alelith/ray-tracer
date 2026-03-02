/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hyperboloid_hit.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvarea-k <bvarea-k@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 15:44:31 by bvarea-k          #+#    #+#             */
/*   Updated: 2026/02/24 09:05:52 by bvarea-k         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static short	check_cap(t_ray r, double y_plane, t_limits lim,
	t_hyp_data *data)
{
	t_point3	p;
	double		cap_radius_squared;

	if (fabs(r.dir.y) < 1e-6)
		return (0);
	data->t_cap = (y_plane - r.orig.y) / r.dir.y;
	if (data->t_cap < lim.min || data->t_cap > lim.max)
		return (0);
	p = at(r, data->t_cap);
	cap_radius_squared = y_plane * y_plane + data->waist;
	if (p.x * p.x + p.z * p.z > cap_radius_squared)
		return (0);
	return (0);
}

static void	check_caps_for_best(t_ray r, t_limits limits, t_hyp_data *data)
{
	data->hit_cap = 0;
	if (check_cap(r, -0.5, limits, data))
	{
		if (data->best_t < 0 || data->t_cap < data->best_t)
		{
			data->best_t = data->t_cap;
			data->hit_cap = 1;
		}
	}
	if (check_cap(r, 0.5, limits, data))
	{
		if (data->best_t < 0 || data->t_cap < data->best_t)
		{
			data->best_t = data->t_cap;
			data->hit_cap = 2;
		}
	}
}

static short	hyperboloid_side_hit(t_ray r, t_limits limits,
	t_hyp_data *data, t_point3 *local_p)
{
	t_baskara	quad;
	int			i;

	quad.a = r.dir.x * r.dir.x + r.dir.z * r.dir.z - r.dir.y * r.dir.y;
	quad.b = 2.0 * (r.orig.x * r.dir.x + r.orig.z * r.dir.z
			- r.orig.y * r.dir.y);
	quad.c = r.orig.x * r.orig.x + r.orig.z * r.orig.z
		- r.orig.y * r.orig.y - data->waist;
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
			if (local_p->y >= -0.5 && local_p->y <= 0.5)
				if (data->best_t < 0 || quad.solutions[i].real < data->best_t)
					data->best_t = quad.solutions[i].real;
		}
		i++;
	}
	return (data->best_t > 0);
}

static short	hyperboloid_hit_internal(t_ray r, t_limits limits,
	t_hit_record *rec, t_hittable hyperboloid)
{
	t_point3	local_p;
	t_hyp_data	data;

	data.waist = ((t_hyperboloid *)hyperboloid.attributes)->waist;
	data.best_t = -1.0;
	data.hit_cap = 0;
	hyperboloid_side_hit(r, limits, &data, &local_p);
	check_caps_for_best(r, limits, &data);
	if (data.best_t < 0)
		return (0);
	rec->t = data.best_t;
	local_p = at(r, rec->t);
	if (data.hit_cap == 1)
		rec->normal = (t_vec3){0, -1, 0};
	else if (data.hit_cap == 2)
		rec->normal = (t_vec3){0, 1, 0};
	else
		rec->normal = normalized((t_vec3){local_p.x, -local_p.y, local_p.z});
	return (1);
}

short	hyperboloid_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable hyperboloid_ptr)
{
	t_ray		local_ray;
	t_vec3		normal;

	local_ray.dir = matrix_vector_multiply(hyperboloid_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(hyperboloid_ptr.transform.inverse,
			r.orig, 1);
	if (!hyperboloid_hit_internal(local_ray, limits, rec, hyperboloid_ptr))
		return (0);
	rec->p = at(r, rec->t);
	normal = matrix_vector_multiply(hyperboloid_ptr.transform.inverse_t,
			rec->normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
