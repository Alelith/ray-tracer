/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taubin_hit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvarea-k <bvarea-k@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 20:49:42 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/27 13:00:07 by bvarea-k         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

double	taubin_sdf(t_point3 p, void *params)
{
	t_taubin	tb;

	tb.x2 = p.x * p.x;
	tb.y2 = p.y * p.y;
	tb.z2 = p.z * p.z;
	tb.z3 = tb.z2 * p.z;
	tb.a = tb.x2 + 2.25 * tb.y2 + tb.z2 - 1;
	tb.a2 = tb.a * tb.a;
	tb.a3 = tb.a2 * tb.a;
	tb.f = tb.a3 - tb.x2 * tb.z3 - 0.1125 * tb.y2 * tb.z3;
	tb.grad.x = 6 * tb.a2 * p.x - 2 * p.x * tb.z3;
	tb.grad.y = 6 * tb.a2 * p.y - 0.225 * p.y * tb.z3;
	tb.grad.z = 6 * tb.a2 * p.z - 3 * tb.x2 * tb.z2 - 0.3375 * tb.y2 * tb.z2;
	tb.grad_len = sqrt(tb.grad.x * tb.grad.x
			+ tb.grad.y * tb.grad.y
			+ tb.grad.z * tb.grad.z);
	if (tb.grad_len > 1e-6)
		return (tb.f / tb.grad_len);
	return (0.001);
}

static short	taubin_hit_internal(const t_ray r, t_limits limits,
			t_hit_record *rec, t_hittable taubin_ptr)
{
	t_ray_marching	rm;
	short			best_index;

	rm.ray = r;
	rm.sdf = taubin_sdf;
	rm.attributes = taubin_ptr.attributes;
	ray_marching(&rm);
	best_index = get_best_solution_index(rm.solutions, limits, rec, 1);
	if (best_index == -1)
		return (0);
	rec->t = rm.solutions[best_index].real;
	return (1);
}

short	taubin_hit(const t_ray r, t_limits limits,
			t_hit_record *rec, t_hittable taubin_ptr)
{
	t_point3	local_p;
	t_vec3		normal;
	t_ray		local_ray;

	local_ray.dir = matrix_vector_multiply
		(taubin_ptr.transform.inverse, r.dir, 0);
	local_ray.orig = matrix_vector_multiply
		(taubin_ptr.transform.inverse, r.orig, 1);
	if (!taubin_hit_internal(local_ray, limits, rec, taubin_ptr))
		return (0);
	rec->material = taubin_ptr.material;
	rec->p = at(r, rec->t);
	local_p = at(local_ray, rec->t);
	normal = get_sdf_normal(local_p, taubin_sdf, taubin_ptr.attributes);
	normal = matrix_vector_multiply(taubin_ptr.transform.inverse_t, normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
