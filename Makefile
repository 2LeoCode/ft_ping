NAME =			ft_ping

SRC_TYPE =	c
OBJ_TYPE =	o

SRC_DIR =		src
LIB_DIR = 	lib
BUILD_DIR =	.build
INC_DIRS =	inc $(wildcard ${LIB_DIR}/*/inc)

CC =				gcc
CVERSION = 	17
CFLAGS =		-Wall -Wextra -Werror $(addprefix -I, ${INC_DIRS}) --std=gnu${CVERSION}
LDFLAGS = 	-lm

SRC =				$(wildcard ${SRC_DIR}/*.${SRC_TYPE} ${SRC_DIR}/*/*.${SRC_TYPE})
OBJ =				$(patsubst ${SRC_DIR}/%.c, ${BUILD_DIR}/%.o, ${SRC})
LIB = 			$(patsubst ${LIB_DIR}/%, ${LIB_DIR}/lib%.a, $(wildcard ${LIB_DIR}/*/))

all: ${NAME}

${NAME}: ${BUILD_DIR} ${OBJ} ${LIB}
	${CC} ${CFLAGS} ${OBJ} ${LIB} -o $@ ${LDFLAGS}

${BUILD_DIR}:
	@mkdir -p $@/list

${BUILD_DIR}/%.${OBJ_TYPE}: ${SRC_DIR}/%.${SRC_TYPE}
	${CC} ${CFLAGS} -c $< -o $@

debug:
	@echo "src = ${SRC}\nobj = ${OBJ}\nlib = ${LIB}"

clean:
	${RM} -r ${BUILD_DIR}

fclean: clean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re debug
