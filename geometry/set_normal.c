/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_normal.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:09:58 by bvarea-k          #+#    #+#             */
/*   Updated: 2026/02/08 14:35:19 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	set_face_normal(const t_ray r, const t_vec3 outward_normal,
							t_hit_record *rec)
{
	rec->front_face = dot(r.dir, outward_normal) < 0;
	if (rec->front_face)
		rec->normal = outward_normal;
	else
		rec->normal = invert(outward_normal);
}
