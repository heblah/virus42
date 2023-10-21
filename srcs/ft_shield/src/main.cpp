/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:20:01 by halvarez          #+#    #+#             */
/*   Updated: 2023/10/20 18:05:59 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <string>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define DAEMON_LOG_PATH "/var/log/matt_daemon"
#define DAEMON_LOCK_FILE "/var/lock/matt_daemon"

void daemonize(void)
{
	int	pid		= 0;
	int	fd		= 0;
	char buf[9] = {'\0'};

	// First fork to detach from the current processus
	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
		//fail
	else if (pid > 0)
		exit(EXIT_SUCCESS);
	else if (pid == 0)
	{
		// Create a new sessio
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
			// Infinite loop to do whatever
			while(1);
		}
	}
	return;
}

int main(void)
{
	daemonize();
	return 0;
}
