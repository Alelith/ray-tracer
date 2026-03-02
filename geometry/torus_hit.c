/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus_hit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 00:01:48 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/20 09:51:18 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static void	calculate_coefficients(t_ferrari *coeffs, t_torus torus, t_ray r)
{
	double		dd;
	double		od;
	double		oo;
	double		sum_r2;
	double		diff_r2;

	dd = dot(r.dir, r.dir);
	od = dot(r.orig, r.dir);
	oo = dot(r.orig, r.orig);
	sum_r2 = 1.0 + torus.rad_relation * torus.rad_relation;
	diff_r2 = 1.0 - torus.rad_relation * torus.rad_relation;
	coeffs->a = dd * dd;
	coeffs->b = 4.0 * dd * od;
	coeffs->c = 2.0 * dd * (oo - sum_r2) + 4.0 * od * od
		+ 4.0 * (r.dir.y * r.dir.y);
	coeffs->d = 4.0 * od * (oo - sum_r2)
		+ 8.0 * (r.dir.y * r.orig.y);
	coeffs->e = (oo + diff_r2) * (oo + diff_r2)
		- 4.0 * (oo - r.orig.y * r.orig.y);
}

static short	torus_hit_internal(const t_ray r, t_limits limits,
										t_hit_record *rec, t_hittable torus)
{
	t_ferrari	coeffs;
	short		best_index;

	calculate_coefficients(&coeffs, *((t_torus *)torus.attributes), r);
	ferrari(&coeffs);
	best_index = get_best_solution_index(coeffs.solutions, limits, rec, 4);
	if (best_index == -1)
		return (0);
	rec->t = coeffs.solutions[best_index].real;
	return (1);
}

short	torus_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable torus_ptr)
{
	t_point3	local_p;
	t_vec3		normal;
	t_ray		local_ray;
	t_point3	center;

	local_ray.dir = matrix_vector_multiply(torus_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(torus_ptr.transform.inverse,
			r.orig, 1);
	if (!torus_hit_internal(local_ray, limits, rec, torus_ptr))
		return (0);
	rec->p = at(r, rec->t);
	local_p = at(local_ray, rec->t);
	center = normalized((t_vec3){local_p.x, 0, local_p.z});
	normal = normalized(substract(local_p, center));
	normal = matrix_vector_multiply(torus_ptr.transform.inverse_t, normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
