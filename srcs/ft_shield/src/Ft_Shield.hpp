/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 17:31:46 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/17 09:27:04 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SHIELD_HPP
#define FT_SHIELD_HPP 

#include <sys/socket.h>

/*
 * Daemon class
 */

class Ft_Shield
{
	public:
	/*---------- Constructors ----------------------*/
					Ft_Shield(void);
					Ft_Shield(const Ft_Shield & shield);
					~Ft_Shield(void);

		Ft_Shield &	operator=(const Ft_Shield & shield);

	/*---------- Public function members -----------*/
		void		daemonize(void);

	private:
	/*---------- Private attributes ----------------*/
		const int		_port;
		const int		_MaxClients;
		struct sockaddr	_addr;
		int				_socket;
		int				_clients[3]__attribute__((unused));
		//std::string		_buffer;

	/*---------- Private function members ----------*/
		int			_mkSrv(void);
		void		_runSrv(void);
};

#endif /* FT_SHIELD_HPP */
