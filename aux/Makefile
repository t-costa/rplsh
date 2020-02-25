CC = g++
IDIR = .
WFLAGS = -Wall
CFLAGS = -I${IDIR} -I${FF_HOME} -std=c++17 -O3 -finline-functions -DNDEBUG -pthread
DFLAGS = -I${IDIR} -I${FF_HOME} -std=c++17 ${WFLAGS} -g -pthread
OBJ = parallel.hpp sequential.hpp

.PHONY : clean, all

all : test

debug : debug

debug : main.cpp ${OBJ}
	${CC} ${DFLAGS} main.cpp -o debug
test : main.cpp ${OBJ}
	${CC} ${CFLAGS} main.cpp -o test

clean :
	${RM} test debug
