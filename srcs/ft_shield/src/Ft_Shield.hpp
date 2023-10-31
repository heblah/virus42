/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 17:31:46 by halvarez          #+#    #+#             */
/*   Updated: 2023/10/31 17:10:55 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SHIELD_HPP
#define FT_SHIELD_HPP 

/*
 * Daemon class
 */

class Ft_Shield
{
	public:
	/*---------- Constructors ----------*/
					Ft_Shield(void);
					Ft_Shield(const Ft_Shield & shield);
					~Ft_Shield(void);

		Ft_Shield &	operator=(const Ft_Shield & shield);

	/*---------- Public function members ----------*/
		void		daemonize(void);

	private:
	/*---------- Private attributes ----------*/
		const int	_port;
		const int	_MaxClients;

	/*---------- Private function members ----------*/
		int			_mksrv(void);
};

#endif /* FT_SHIELD_HPP */
