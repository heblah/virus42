/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 16:07:58 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/20 16:29:03 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/wait.h>

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	int		fd[2];
	int		pid;
	char	char_buf[100] = {'\0'};
	std::string _buffer;

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
			system("ps axco pid,command | grep wsh");
		}
		else if (pid != 0)
		{
			close(fd[1]);
			waitpid(pid, NULL, 0);
			read(fd[0], char_buf, 99);
			_buffer = char_buf;
			bzero(char_buf, 100);
			std::cout << _buffer;// << std::endl;
			close(fd[0]);
		}
	}
	write(1, argv[0], strlen(argv[0]) );
	argv[0][0] = 'a';
	argv[0][1] = 'b';
	argv[0][2] = 'c';
	argv[0][3] = '\0';

	write(1, argv[0], strlen(argv[0]) );
	return 0;
}
