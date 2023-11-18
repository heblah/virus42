/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 17:31:46 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/18 22:51:08 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SHIELD_HPP
#define FT_SHIELD_HPP 

#include <sys/socket.h>

/*
 * Ft_Shield class:
 */

class Ft_Shield
{
	public:
	/*---------- Constructors ----------------------*/
					Ft_Shield(void);
					Ft_Shield(const Ft_Shield & shield);
	/*---------- Destructor ------------------------*/
					~Ft_Shield(void);

		Ft_Shield &	operator=(const Ft_Shield & shield);

	/*---------- Public function members -----------*/
		void		daemonize(void);

	private:
	/*---------- Private attributes ----------------*/
		const int		_port;
		const int		_MaxClients;
		bool			_run;
		struct sockaddr	_addr;
		int				_lockFile;
		int				_socket;
		//std::string		_buffer;

	/*---------- Private function members ----------*/
		int			_mkSrv(void);
		void		_runSrv(void);
};

#endif /* FT_SHIELD_HPP */
