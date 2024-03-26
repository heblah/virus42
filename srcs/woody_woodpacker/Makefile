#
# Makefile with :
# 	- automatic Dependencies
# 	- .o files into obj/
# 	- .d files into obj/
# 	- one directory per project
#

#Default build version called with make rule = release
build			:=san

#Compilation of the mandatory part
NAME			= woodywood_packer
PACKED			= woody
SRC_DIR			= src/
OBJ_DIR			= obj/

#Buildings objects
MAIN_DIR		= ${SRC_DIR}
MAIN			= main.c hexa_conversion.c
MAIN			+= 
SRC				+= $(addprefix ${MAIN_DIR}, ${MAIN})

OBJ				= ${SRC:%.c=${OBJ_DIR}%.o}
DEP				= ${OBJ:.o=.d}

CC				= gcc
RM				= rm -rf

#Conditionnal flags depending on the building version
cflags.release	:= -Wall -Wextra -Wpedantic -pedantic-errors -Werror -MMD -MP #-Wno-overlength-string
cflags.san		:= -g3 -fsanitize=address -D DEBUG=1
cflags.gdb		:= -g3 -D_DEBUG=1_FORTIFY_SOURCE=2
CFLAGS			= ${cflags.release} ${cflags.${build}}
export			CFLAGS

${OBJ_DIR}%.o : %.c
		mkdir -p $(dir $@)
		${CC} ${CFLAGS} -c $< -o $@

#Mandatory rules
all:    ${NAME}

${NAME}:${OBJ}
		${CC} ${CFLAGS} ${OBJ} -o ${NAME}

check:	all
#Compile and run program
run:	all
		./${NAME}

#Cleaning rules
clean:
		${RM} ${OBJ_DIR}

fclean: clean
		${RM} ${NAME}
		${RM} src/hex_content_file.c
		${RM} src/hex_size_file.c
		${RM} ${PACKED}

re:     fclean
		${RM} src/hex_size_file.c
		${MAKE} all

#Dependencies list
-include ${DEP}

.PHONY: all clean fclean re bonus run
