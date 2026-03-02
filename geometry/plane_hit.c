/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane_hit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 16:31:43 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/20 09:50:46 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static short	plane_hit_internal(const t_ray r, t_limits limits,
										t_hit_record *rec, t_hittable plane)
{
	double	t;

	if (fabs(r.dir.y) < 1e-6)
		return (0);
	t = -(r.orig.y / r.dir.y);
	if (t < limits.min || t > limits.max)
		return (0);
	rec->t = t;
	return (1);
}

short	plane_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable plane_ptr)
{
	t_ray	local_ray;
	t_vec3	local_normal;
	t_vec3	world_normal;

	local_ray.dir = matrix_vector_multiply(plane_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(plane_ptr.transform.inverse,
			r.orig, 1);
	if (!plane_hit_internal(local_ray, limits, rec, plane_ptr))
		return (0);
	rec->p = at(r, rec->t);
	local_normal = vector(0, 1, 0);
	world_normal = matrix_vector_multiply(plane_ptr.transform.inverse_t,
			local_normal, 0);
	set_face_normal(r, normalized(world_normal), rec);
	return (1);
}
