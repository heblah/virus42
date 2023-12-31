#
# Makefile with :
# 	- automatic Dependencies
# 	- .o files into obj/
# 	- .d files into obj/
# 	- one directory per project
#

#Default build version called with make rule = release
build			:=

#Compilation of the mandatory part
NAME			= virus
SRC_DIR			= srcs/
OBJ_DIR			= obj/

#Buildings objects
MAIN_DIR		= ${SRC_DIR}
MAIN			= main.c
SRC				+= $(addprefix ${MAIN_DIR}, ${MAIN})

PACKER_DIR		= ${SRC_DIR}woody_woodpacker
PACKER			= #list of files
SRC				+= $(addprefix ${PACKER_DIR}, ${PACKER})

FAMINE_DIR		= ${SRC_DIR}famine
FAMINE			= #list of files
SRC				+= $(addprefix ${FAMINE_DIR}, ${FAMINE})

PESTILENCE_DIR	= ${SRC_DIR}pestilence
PESTILENCE		= #list of files
SRC				+= $(addprefix ${PESTILENCE_DIR}, ${PESTILENCE})

WAR_DIR			= ${SRC_DIR}war
WAR				= #list of files
SRC				+= $(addprefix ${WAR_DIR}, ${WAR})

DEATH_DIR		= ${SRC_DIR}death
DEATH			= #list of files
SRC				+= $(addprefix ${DEATH_DIR}, ${DEATH})

OBJ				= ${SRC:%.c=${OBJ_DIR}%.o}
DEP				= ${OBJ:.o=.d}

CC				= cc
RM				= rm -rf

#Conditionnal flags depending on the building version
cflags.release	:= -Wall -Wextra -Werror -Wpedantic -pedantic-errors -MMD -MP
cflags.gdb		:= -g3
cflags.san		:= -g3 -fsanitize=address
CFLAGS			= ${cflags.release} ${cflags.${build}}
export			CFLAGS

${OBJ_DIR}%.o : %.c
		mkdir -p $(dir $@)
		${CC} ${CFLAGS} -c $< -o $@

#Mandatory rules
all:    ${NAME}

${NAME}:${OBJ}
		${CC} ${CFLAGS} ${OBJ} -o ${NAME}

#Compile and run program
run:	all
		./${NAME}

#Cleaning rules
clean:
		${RM} ${OBJ_DIR}

fclean: clean
		${RM} ${NAME}

re:     fclean
		${MAKE} all

#Dependencies list
-include ${DEP}

.PHONY: all clean fclean re bonus run
