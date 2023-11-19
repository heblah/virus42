/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 16:44:39 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/19 00:36:45 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <csignal>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "Ft_Shield.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_LOG_PATH "/var/log/matt_daemon"
#define DAEMON_LOCK_FILE "/var/lock/matt_daemon"

/* Constructors ============================================================= */
Ft_Shield::Ft_Shield(void) : _port(4242), _MaxClients(3), _run(true)
{
	sockaddr_in		&addrIn 	= *reinterpret_cast<sockaddr_in *>(&this->_addr);

	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.s_addr = INADDR_ANY;
	addrIn.sin_port = htons(this->_port);

	/* Add a sginal handler for interrution */
	return;
}

Ft_Shield::Ft_Shield(const Ft_Shield &shield) : _port(shield._port), _MaxClients(shield._MaxClients)
{
	/*
	 * Has to delete the lock file
	 * Maybe clean the log file
	 */
	return;
}

/* Destructor =============================================================== */
Ft_Shield::~Ft_Shield(void)
{
	/*
	* Close the server
	* Remove lock file
	* Remove log directory
	*/
	return;
}

/* Operators ================================================================ */
Ft_Shield & Ft_Shield::operator=(const Ft_Shield & shield __attribute__((unused)))
{
	return *this;
}

/* Public member functions ================================================== */
void Ft_Shield::daemonize(void)
{
	int	pid		= 0;
	char buf[9] = {'\0'};

	/* First fork to detach from the current processus */
	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
	else if (pid > 0)
		exit(EXIT_SUCCESS);
	else if (pid == 0)
	{
		/* Create a new session */
		setsid();
		/* Second fork to detach from terminal */
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
			/* Redirect stdin, stdout and stderr to /dev/null */
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);
			/* Set default permisions */
			umask(0000);
			/* Test if log path exists, creates it and moves in otherwise */
			if (chdir(DAEMON_LOG_PATH) == -1)
			{
				if (mkdir(DAEMON_LOG_PATH, 0755) == -1)
					this->_run = false;
				if (chdir(DAEMON_LOG_PATH) == -1)
					this->_run = false;
			}
			/* 
			 * Create a lock file
			 * Open fails if the file already exists using this flags combination: O_CREAT | O_EXCL
			 * Fail if another instance of ft_shield is running
			 */
			this->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);
			if (this->_lockFile != -1)
			{
				/* Write the daemon pid in the lock file */
				sprintf(buf, "%d\n", getpid());
				write(this->_lockFile, buf, strlen(buf));
				/* _mkSrv returns -1 on error, 0 otherwise */
				 if (this->_mkSrv() != -1)
					 this->_runSrv();
			}
		}
	}
	return;
}

/* Private member functions ================================================= */
/* 
 * Configure a server listening on this->_port (4242)
 *   - Do not block
 *   - Can reuse the address and the port right after closing the server
 *   - Return -1 on error and 0 otherwise
 */
int	Ft_Shield::_mkSrv(void)
{
	int				opt			= 1;

	this->_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_socket == -1)
		return -1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		return -1;
	if (bind(this->_socket, &this->_addr, sizeof(this->_addr)) == -1)
		return -1;
	if (listen(this->_socket, 10) == -1)
		return -1;
	return 0;
}

/*
 * Run a simple server using select api
 * Close the client socket if it is disconnected or sends a 'quit' string
 */
void	Ft_Shield::_runSrv(void)
{
	int			maxfd = 4;
	socklen_t	lenaddr = sizeof(this->_addr);
	int			client __attribute__((unused)) = -1;
	fd_set		master_set, read_set, write_set;

	FD_ZERO( &master_set );
	FD_SET(this->_socket, &master_set);

	while(this->_run)
	{
		read_set = master_set;
		write_set = master_set;
		if (select(maxfd + 1, &read_set, &write_set, NULL, NULL) < 0)
			continue;
		for(int fd = 0; fd <= maxfd; fd++)
		{
			if (FD_ISSET(fd, &read_set) && fd == this->_socket)
			{
				client = accept(this->_socket, &this->_addr, &lenaddr);
				if (client == -1)
					continue;
				FD_SET(client, &master_set);
				maxfd = client > maxfd ? client : maxfd;
				send(client, "Connected to ft_shield\n", 25, 0);

			}
			if (FD_ISSET(fd, &read_set) && fd != this->_socket)
			{

			}
		}
	}
	return;
}
