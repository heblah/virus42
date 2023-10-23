/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 16:44:39 by halvarez          #+#    #+#             */
/*   Updated: 2023/10/23 18:31:42 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "src/Ft_Shield.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_LOG_PATH "/var/log/matt_daemon"
#define DAEMON_LOCK_FILE "/var/lock/matt_daemon"

/* Constructors ============================================================= */
Ft_Shield::Ft_Shield(void) : _port(4242), _MaxClients(3)
{
	return;
}

Ft_Shield::Ft_Shield(const Ft_Shield &shield) : _port(shield._port), _MaxClients(shield._MaxClients)
{
	return;
}

/* Destructor =============================================================== */
Ft_Shield::~Ft_Shield(void)
{
	return;
}

/* Operators ================================================================ */
Ft_Shield & Ft_Shield::operator=(const Ft_Shield & shield)
{
	//Close the server
	//Remove lock file
	//Remove log directory
	return;
}

/* Public member functions ================================================== */
void Ft_Shield::daemonize(void)
{
	int	pid		= 0;
	int	fd		= 0;
	char buf[9] = {'\0'};

	// First fork to detach from the current processus
	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
	else if (pid > 0)
		exit(EXIT_SUCCESS);
	else if (pid == 0)
	{
		// Create a new session
		setsid();
		// Second fork to detach from terminal
		pid = fork();
		if (pid == -1)
			exit(EXIT_FAILURE);
		else if (pid > 0)
		{
			std::cout << "halvarez" << std::endl;
			exit(EXIT_SUCCESS);
		}
		else if (pid == 0)
		{
			// Redirect stdin, stdout and stderr to /dev/null
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);
			// Set default permisions
			umask(0000);
			// Test if log path exists, creates it and moves in otherwise
			if (chdir(DAEMON_LOG_PATH) == -1)
			{
				if (mkdir(DAEMON_LOG_PATH, 0755) == -1)
					exit(EXIT_FAILURE);
				if (chdir(DAEMON_LOG_PATH) == -1)
					exit(EXIT_FAILURE);
			}
			// Create a lock file == check if an instance of this daemon is running
			fd = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT);
			if (fd == -1)
				exit(EXIT_FAILURE);
			// Lock the file, exit if the file is already locked
			if (lockf(fd, F_TLOCK, 0) == -1)
				exit(EXIT_SUCCESS);
			// Write the daemon pid in the lock file
			sprintf(buf, "%d\n", getpid());
			write(fd, buf, strlen(buf));
			// Create server
			if (this->_mksrv() == -1)
				exit(EXIT_FAILURE);
			// Infinite loop to do whatever
			while(1);
		}
	}
	return;
}

/* Private member functions ================================================= */
/* 
 * Configure a server listening on this->_port (4242)
 *   - do not block
 *   - Can reuse the address and the port right after closing the server
 *   - return -1 on error and 0 otherwise
 */
int	Ft_Shield::_mksrv(void)
{
	int				socket	= 0;
	int				opt		= 1;
	t_sockaddr_in	addrIn;
	t_sockaddr		*addr	= reinterpret_cast<t_sockaddr *>(&addrIn);

	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.s_addr = INADDR_ANY;
	addrIn.sin_port = htons(this->_port);

	socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socket == -1)
		return -1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_RESUSEPORT, &opt, sizeof(opt)) == -1)
		return -1;
	if (bind(socket, addr, sizeof(*addr)) == -1)
		return -1;
	return 0;
}
