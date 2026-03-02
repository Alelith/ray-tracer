/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kiss_hit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 00:00:00 by acesteve          #+#    #+#             */
/*   Updated: 2026/02/20 09:47:05 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static void	calculate_coefficients(t_ferrari *coeffs, t_kiss kiss, t_ray r)
{
	coeffs->a = -kiss.k * r.dir.y * r.dir.y * r.dir.y * r.dir.y;
	coeffs->b = -kiss.k * 4.0 * r.orig.y * r.dir.y * r.dir.y * r.dir.y;
	coeffs->c = r.dir.x * r.dir.x
		+ r.dir.z * r.dir.z
		- kiss.k * 6.0 * r.orig.y * r.orig.y * r.dir.y * r.dir.y
		+ (kiss.k + 1.0) * r.dir.y * r.dir.y;
	coeffs->d = 2.0 * r.orig.x * r.dir.x + 2.0 * r.orig.z * r.dir.z
		- kiss.k * 4.0 * r.orig.y * r.orig.y * r.orig.y * r.dir.y
		+ (kiss.k + 1.0) * 2.0 * r.orig.y * r.dir.y;
	coeffs->e = r.orig.x * r.orig.x + r.orig.z * r.orig.z
		- kiss.k * r.orig.y * r.orig.y * r.orig.y * r.orig.y
		+ (kiss.k + 1.0) * r.orig.y * r.orig.y - 1.0;
}

static short	find_best_solution(t_solution *sols, t_limits lim, t_ray r)
{
	int			i;
	double		best_t;
	short		best_idx;
	t_point3	p;

	best_t = -1.0;
	best_idx = -1;
	i = -1;
	while (++i < 4)
	{
		if (sols[i].is_real && sols[i].real > lim.min && sols[i].real < lim.max)
		{
			p = at(r, sols[i].real);
			if (p.y >= -1.0 && p.y <= 1.0)
			{
				if (best_t < 0 || sols[i].real < best_t)
				{
					best_t = sols[i].real;
					best_idx = i;
				}
			}
		}
	}
	return (best_idx);
}

static short	kiss_hit_internal(const t_ray r, t_limits limits,
										t_hit_record *rec, t_hittable kiss)
{
	t_ferrari	coeffs;
	short		best_idx;

	calculate_coefficients(&coeffs, *((t_kiss *)kiss.attributes), r);
	ferrari(&coeffs);
	best_idx = find_best_solution(coeffs.solutions, limits, r);
	if (best_idx == -1)
		return (0);
	rec->t = coeffs.solutions[best_idx].real;
	return (1);
}

static t_vec3	calculate_normal(t_point3 local_p, double k)
{
	t_vec3	grad;

	grad.x = 2.0 * local_p.x;
	grad.y = -k * 4.0 * local_p.y * local_p.y * local_p.y
		+ (k + 1.0) * 2.0 * local_p.y;
	grad.z = 2.0 * local_p.z;
	return (normalized(grad));
}

short	kiss_hit(const t_ray r, t_limits limits,
					t_hit_record *rec, t_hittable kiss_ptr)
{
	t_point3	local_p;
	t_vec3		normal;
	t_ray		local_ray;

	local_ray.dir = matrix_vector_multiply(kiss_ptr.transform.inverse,
			r.dir, 0);
	local_ray.orig = matrix_vector_multiply(kiss_ptr.transform.inverse,
			r.orig, 1);
	if (!kiss_hit_internal(local_ray, limits, rec, kiss_ptr))
		return (0);
	rec->p = at(r, rec->t);
	local_p = at(local_ray, rec->t);
	normal = calculate_normal(local_p, ((t_kiss *)kiss_ptr.attributes)->k);
	normal = matrix_vector_multiply(kiss_ptr.transform.inverse_t, normal, 0);
	set_face_normal(r, normalized(normal), rec);
	return (1);
}
