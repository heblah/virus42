# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: halvarez <halvarez@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/22 16:55:50 by halvarez          #+#    #+#              #
#    Updated: 2023/09/18 10:47:34 by halvarez         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#Default build version called with make rule = release
build			:= gdb

#Compilation of the mandatory part
COLLEEN    	= Colleen
SRC_DIR_C	= 
SRC_C		= Colleen.c
OBJ_C		= $(addprefix ${SRC_DIR_C}, ${SRC_C:.c=.o})
DEP_C		= $(addprefix ${SRC_DIR_C}, ${SRC_C:.c=.d})

GRACE    	= Grace
SRC_DIR_G	= 
SRC_G		= Grace.c
OBJ_G		= $(addprefix ${SRC_DIR_G}, ${SRC_G:.c=.o})
DEP_G		= $(addprefix ${SRC_DIR_G}, ${SRC_G:.c=.d})

SULLY    	= Sully
SRC_DIR_S	= 
SRC_S		= Sully.c
OBJ_S		= $(addprefix ${SRC_DIR_S}, ${SRC_S:.c=.o})
DEP_S		= $(addprefix ${SRC_DIR_S}, ${SRC_S:.c=.d})

CC			= cc 
RM			= rm -rf

#Conditionnal flags depending on the building version
cflags.release	:= -Wall -Wextra -Werror -MMD -MP
cflags.gdb		:= -g1
cflags.sanitize	:= -g -fsanitize=address
CFLAGS			= ${cflags.release} ${cflags.${build}}

.c.o:	
		${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

all:    ${COLLEEN} ${GRACE} ${SULLY}

colleen:${COLLEEN}
grace:	${GRACE}
sully:	${SULLY}

#Mandatory rules
${COLLEEN}:${OBJ_C}
		${CC} ${CFLAGS} ${OBJ_C} -o ${COLLEEN}
${GRACE}:${OBJ_G}
		${CC} ${CFLAGS} ${OBJ_G} -o ${GRACE}
${SULLY}:${OBJ_S}
		${CC} ${CFLAGS} ${OBJ_S} -o ${SULLY}

#Cleaning rules
clean:
		${RM} ${OBJ_C} ${DEP_C}
		${RM} ${OBJ_G} ${DEP_G}
		${RM} ${OBJ_S} ${DEP_S}
		${RM} Grace_kid.c

fclean: clean
		${RM} ${COLLEEN}
		${RM} ${GRACE}
		${RM} ${SULLY}
		${RM} Sully_*

re:     fclean
		${MAKE} all

#Dependencies list
-include ${DEP_C} ${DEP_G} ${DEP_S}

.PHONY: all clean fclean re
