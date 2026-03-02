/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_hit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acesteve <acesteve@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 13:34:45 by bvarea-k          #+#    #+#             */
/*   Updated: 2026/02/19 12:49:54 by acesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

short	hittables_hit(const t_ray r, t_limits limits,
						t_hit_record *rec, t_hittable start_ptr)
{
	t_hit_record	tmp;
	short			hit_any;
	t_node			*temp_node;

	hit_any = 0;
	temp_node = (t_node *)start_ptr.attributes;
	while (temp_node)
	{
		tmp.material = temp_node->hittable.material;
		if (temp_node->hittable.hit(r, limits,
				&tmp, temp_node->hittable))
		{
			hit_any = 1;
			limits.max = tmp.t;
			*rec = tmp;
		}
		temp_node = temp_node->next;
	}
	return (hit_any);
}
