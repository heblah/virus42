/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:20:01 by halvarez          #+#    #+#             */
/*   Updated: 2023/10/19 18:13:01 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void daemonize(void)
{
	int pid = 0;

	// First fork to detach from current processus
	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
		//fail
	else if (pid > 0)
	{
		std::cout << "halvarez" << std::endl;
		exit(EXIT_SUCCESS);
	}
	else if (pid == 0)
	{
		setpgid(0, 0);
		setsid();
		// Second fork to detach from terminal
		pid = fork();
		if (pid == -1)
			exit(EXIT_FAILURE);
		else if (pid > 0)
			exit(EXIT_SUCCESS);
		else if (pid == 0)
		{
			// Close all file discriptors
			for (int i = getdtablesize(); i >= 3, i--)
				close(i);
			// Redirect stdin, stdout and stderr to /dev/null
			freopen("/dev/null", "r", stdin);
			freopen("/dev/null", "w", stdout);
			freopen("/dev/null", "w", stderr);
			umask(0000);

		}
	}
	return;
}

int main(void)
{
	umask(0000);
	seteuid(0);
	int res = mkdir("/test", 0755);
	if ( res == 0 )
		std::cout << "ok" << std::endl;
	else
		std::cout << "fail" << std::endl;
	return 0;
}
