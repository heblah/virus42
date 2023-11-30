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
NAME			= woody_woodpacker
SRC_DIR			= srcs/
OBJ_DIR			= obj/
HEADERS_DIR		= headers

#Buildings objects
MAIN_DIR		= ${SRC_DIR}
MAIN			= main.c copy_binary.c getFileSize.c memcmp.c
SRC				+= $(addprefix ${MAIN_DIR}, ${MAIN})

OBJ				= ${SRC:%.c=${OBJ_DIR}%.o}
DEP				= ${OBJ:.o=.d}

CC				= cc
RM				= rm -rf

#Conditionnal flags depending on the building version
cflags.release	:= -Wall -Wextra -Werror -Wpedantic -pedantic-errors -MMD -MP -I$(HEADERS_DIR)
cflags.gdb		:= -g3
cflags.san		:= -g3 -fsanitize=address
CFLAGS		= ${cflags.release} ${cflags.san} ${cflags.${build}}
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
