/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 16:44:39 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/20 17:47:45 by halvarez         ###   ########.fr       */
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
#include <sys/wait.h>
#include <arpa/inet.h>

#include "Ft_Shield.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_LOG_FILE "/var/log/.matt_daemon"
#define DAEMON_LOCK_FILE "/var/lock/.matt_daemon"

/* Constructors ============================================================= */
Ft_Shield::Ft_Shield(void) : _port(4242), _MaxClients(3), _run(true), _lockFile(-1), _logFile(-1), _nClients(0)
{
	sockaddr_in		&addrIn 	= *reinterpret_cast<sockaddr_in *>(&this->_addr);

	/* Define the server parameters */
	addrIn.sin_family = AF_INET;
	//addrIn.sin_addr.s_addr = INADDR_ANY;
	addrIn.sin_addr.s_addr = inet_addr("127.147.6.1");
	addrIn.sin_port = htons(this->_port);

	/* Map the commands into the command-mapper t_commands */
	this->_cmdMap["shutdown\n"] = &Ft_Shield::_shutdown;
	return;
}

Ft_Shield::Ft_Shield(const Ft_Shield &shield) : _port(shield._port), _MaxClients(shield._MaxClients)
{
	return;
}

/* Destructor =============================================================== */
Ft_Shield::~Ft_Shield(void)
{
	/*
	 * Close the lock file
	 * Delete the lock file
	 */
	close(this->_lockFile);
	remove(DAEMON_LOCK_FILE);
	close(this->_logFile);
	//remove(DAEMON_LOG_FILE);
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

	std::cout << "halvarez" << std::endl;
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
			exit(EXIT_SUCCESS);
		else if (pid == 0)
		{
			/* Set default permisions */
			umask(0000);
			/* Open the log file and indentify the beginning of this instance */
			this->_logFile = open(DAEMON_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);
			if (this->_logFile != -1)
				write(this->_logFile, "New log instance:\n", 18);
			/* 
			 * Create a lock file
			 * Open fails if the file already exists using this flags combination: O_CREAT | O_EXCL
			 * Fail if another instance of ft_shield is running
			 */
			//this->_checkInstance();
			this->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);
			if (this->_run == true && this->_lockFile != -1)
			{
				/* Write the daemon pid in the lock file */
				sprintf(buf, "%d\n", getpid());
				write(this->_lockFile, buf, strlen(buf));
				/* _mkSrv returns -1 on error, 0 otherwise */
				 if (this->_mkSrv() != -1)
					 this->_runSrv();
			}
			else if (this->_run == false && this->_lockFile != -1)
				close(this->_lockFile);
		}
	}
	return;
}

/* Private member functions ================================================= */
/*
 * Check if another instance is already running, quit if yes, continue otherwise
 * Check by process name instead of get a lock file : more stealth
 * it's buggy
 */
void	Ft_Shield::_checkInstance(void)
{
	int		fd[2];
	int		pid;
	char	char_buf[100] = {'\0'};

	//this->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);
	if (pipe(fd) != -1)
	{
			/*
			 * Check return value of ps axco | grep ft_shield
			 * Set this->_run to false if another ft_shield process is running
			 */
		pid = fork();
		if (pid == 0)
		{
			close(fd[0]);
			close(STDOUT_FILENO);
			dup2(fd[1], STDOUT_FILENO);
			system("ps axco pid,command | grep ft_shield");
		}
		else if (pid != 0)
		{
			close(fd[1]);
			waitpid(pid, NULL, 0);
			read(fd[0], char_buf, 99);
			this->_buffer = char_buf;
			close(fd[0]);
		}
	}
	return;
}

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
	int			maxfd = this->_socket;
	socklen_t	lenaddr = sizeof(this->_addr);
	int			client = -1;
	fd_set		master_set, read_set, write_set;
	char		char_buf[100] = {'\0'};
	int			res = 0;

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
				if (client != -1 && this->_nClients < this->_MaxClients)
				{
					FD_SET(client, &master_set);
					maxfd = client > maxfd ? client : maxfd;
					this->_nClients++;
					send(client, "Connected to ft_shield\n", 24, 0);
				}
				else if (client != -1)
				{
					send(client, "Sorry, too much connexions\n", 28, 0);
					close(client);
				}
			}
			if (FD_ISSET(fd, &read_set) && fd != this->_socket)
			{
				/* if res == 0 : disconnection case */
				res = recv(fd, char_buf, 99, MSG_PEEK);
				if (res <= 0)
				{
					close(fd);
					maxfd = (fd == maxfd) ? maxfd - 1 : maxfd;
					this->_nClients--;
				}
				else
				{
					res = recv(fd, char_buf, 99, 0);
					this->_buffer = char_buf;
					bzero(char_buf, 100);
					if (this->_cmdMap.find(this->_buffer) != this->_cmdMap.end())
						( this->*( this->_cmdMap[this->_buffer] ) )();
				}
			}
		}
	}
	return;
}

void	Ft_Shield::_shutdown(void)
{
	this->_run = false;
	return;
}
