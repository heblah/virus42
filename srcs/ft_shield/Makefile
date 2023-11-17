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
NAME			= ft_shield
SRC_DIR			= src/
OBJ_DIR			= obj/

#Buildings objects
MAIN_DIR		= ${SRC_DIR}
MAIN			= main.cpp Ft_Shield.cpp
SRC				+= $(addprefix ${MAIN_DIR}, ${MAIN})

OBJ				= ${SRC:%.cpp=${OBJ_DIR}%.o}
DEP				= ${OBJ:.o=.d}

CXX				= clang++
RM				= rm -rf

#Conditionnal flags depending on the building version
cxxflags.release	:= -Wall -Wextra -Werror -Wpedantic -pedantic-errors -MMD -MP
cxxflags.gdb		:= -g3
cxxflags.san		:= -g3 -fsanitize=address
CXXFLAGS		= ${cxxflags.release} ${cxxflags.${build}}
export			CXXFLAGS

${OBJ_DIR}%.o : %.cpp
		mkdir -p $(dir $@)
		${CXX} ${CXXFLAGS} -c $< -o $@

#Mandatory rules
all:    ${NAME}
		chown root:root ${NAME}
		chmod 744 ${NAME}

${NAME}:${OBJ}
		${CXX} ${CXXFLAGS} ${OBJ} -o ${NAME}

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
