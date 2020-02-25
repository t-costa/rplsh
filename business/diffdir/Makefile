
CXX	= g++ -std=c++11
CFLAGS 	= `pkg-config --cflags opencv` #-pg
RPL	= ../../
FF	= ../../fastflow/
OPT     = -O3 #-DNDEBUG -g
LIBS 	= `pkg-config --libs opencv`

SRC=$(wildcard ff*.cpp)
BIN=$(patsubst %.cpp,%,$(SRC))

ff:	$(BIN)

sequential: sequential.cpp
	$(CXX) $(OPT) -I$(RPL) -o $@ $< $(LIBS)

$(BIN): %: %.cpp
	$(CXX) $(OPT) -I$(FF) -I$(RPL) -o $@ $< $(LIBS) -pthread
