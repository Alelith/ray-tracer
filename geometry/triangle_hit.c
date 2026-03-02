/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle_hit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 15:36:32 by bvarea-k          #+#    #+#             */
/*   Updated: 2026/02/22 23:36:58 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

//To obtain the normal: (B - A) × (C - A):

static t_vec3	calc_normal(t_triangle *triangle)
{
	t_vec3	edge1;
	t_vec3	edge2;
	t_vec3	normal;

	edge1 = substract(triangle->vertex2, triangle->vertex1);
	edge2 = substract(triangle->vertex3, triangle->vertex1);
	normal = cross(edge1, edge2);
	return (normalized(normal));
}

static t_triangle	create_triangle(t_vec3 v1, t_vec3 v2, t_vec3 v3)
{
	t_triangle	tri;

	tri.vertex1 = v1;
	tri.vertex2 = v2;
	tri.vertex3 = v3;
	tri.normal = calc_normal(&tri);
	return (tri);
}

static short	is_inside_triangle(t_triangle *tri, t_vec3 hit_p)
{
	t_vec3	edge;
	t_vec3	vp;
	t_vec3	cp;

	edge = substract(tri->vertex2, tri->vertex1);
	vp = substract(hit_p, tri->vertex1);
	cp = cross(edge, vp);
	if (dot(tri->normal, cp) < 0)
		return (0);
	edge = substract(tri->vertex3, tri->vertex2);
	vp = substract(hit_p, tri->vertex2);
	cp = cross(edge, vp);
	if (dot(tri->normal, cp) < 0)
		return (0);
	edge = substract(tri->vertex1, tri->vertex3);
	vp = substract(hit_p, tri->vertex3);
	cp = cross(edge, vp);
	if (dot(tri->normal, cp) < 0)
		return (0);
	return (1);
}

static short	triangle_hit_internal(const t_ray r, t_limits limits,
								t_hit_record *rec, t_triangle *tri)
{
	double	t;
	t_vec3	hit;
	double	denom;

	denom = dot(tri->normal, r.dir);
	if (fabs(denom) < 1e-6)
		return (0);
	t = dot(tri->normal, substract(tri->vertex1, r.orig)) / denom;
	if (t < limits.min || t > limits.max)
		return (0);
	hit = add(r.orig, multiply(t, r.dir));
	if (!is_inside_triangle(tri, hit))
		return (0);
	rec->t = t;
	rec->p = hit;
	set_face_normal(r, tri->normal, rec);
	return (1);
}

short	triangle_hit(const t_ray r, t_limits limits,
			t_hit_record *rec, t_hittable triangle_ptr)
{
	return (triangle_hit_internal(r, limits, rec,
			(t_triangle *)triangle_ptr.attributes));
}
