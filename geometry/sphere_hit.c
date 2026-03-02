/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere_hit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 12:08:27 by bvarea-k          #+#    #+#             */
/*   Updated: 2026/02/20 09:50:58 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static short	sphere_hit_internal(const t_ray r, t_limits limits,
										t_hit_record *rec, t_hittable sphere)
{
	t_baskara	quad;
	t_vec3		normal;
	short		best_index;

	quad.a = dot(r.dir, r.dir);
	quad.b = 2 * dot(r.dir, r.orig);
	quad.c = dot(r.orig, r.orig) - 1;
	baskara(&quad);
	best_index = get_best_solution_index(quad.solutions, limits, rec, 2);
	if (best_index == -1)
		return (0);
	rec->t = quad.solutions[best_index].real;
	return (1);
}

short	sphere_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable sphere_ptr)
{
	t_vec3	normal;
	t_ray	local_ray;

	local_ray.dir = matrix_vector_multiply(sphere_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(sphere_ptr.transform.inverse,
			r.orig, 1);
	if (!sphere_hit_internal(local_ray, limits, rec, sphere_ptr))
		return (0);
	rec->p = at(r, rec->t);
	normal = at(local_ray, rec->t);
	normal = matrix_vector_multiply(sphere_ptr.transform.inverse_t, normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
