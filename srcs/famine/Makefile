#
# Makefile with :
# 	- automatic Dependencies
# 	- .o files into obj/
# 	- .d files into obj/
# 	- one directory per project
#

#Default build version called with make rule = release
build			:=gdb

#Compilation of the mandatory part
NAME			= Famine
SRC_DIR			= src/
OBJ_DIR			= obj/

#Buildings objects
MAIN_DIR		= ${SRC_DIR}
MAIN			= main.c famine.c print_headers.c
SRC				+= $(addprefix ${MAIN_DIR}, ${MAIN})

OBJ				= ${SRC:%.c=${OBJ_DIR}%.o}
DEP				= ${OBJ:.o=.d}

CC				= clang
RM				= rm -rf

#Conditionnal flags depending on the building version
cflags.release	:= -Wall -Wextra -Werror -Wpedantic -pedantic-errors -MMD -MP
cflags.gdb		:= -g3 -D DEBUG=1
cflags.san		:= -g3 -fsanitize=address -D DEBUG=1
CFLAGS			= ${cflags.release} ${cflags.${build}}
export			CFLAGS

${OBJ_DIR}%.o : %.c
		mkdir -p $(dir $@)
		${CC} ${CFLAGS} -c $< -o $@

#Mandatory rules
all:    ${NAME}
		#chown root:root ${NAME}
		#chmod 744 ${NAME}

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
