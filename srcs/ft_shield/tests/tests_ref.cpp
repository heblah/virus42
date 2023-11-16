/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests_ref.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 11:24:24 by halvarez          #+#    #+#             */
/*   Updated: 2023/11/16 11:26:46 by halvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

int	main(void)
{
	int a = 147;
	int &ref = a;
	
	std::cout << "addr(a)   = " << &a << std::endl;
	std::cout << "addr(ref) = " << &ref << std::endl;
	return 0;
}
