
CXX	= g++ -std=c++11
#CFLAGS 	= `pkg-config --cflags opencv` #-pg
RPL	= ../../
FF = $(FF_HOME)
#FF	= ../../fastflow/
OPT     = -O3 #-DNDEBUG -g
#LIBS 	= `pkg-config --libs opencv`

SRC=$(wildcard ff*.cpp)
BIN=$(patsubst %.cpp,%,$(SRC))

ff:	$(BIN)

sequential: sequential.cpp business.hpp
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $< $(LIBS)

sequential_v2: sequential_v2.cpp business_v2.hpp
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $< $(LIBS)

$(BIN): %: %.cpp business.hpp
	$(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) -o $@ $< $(LIBS) -pthread

clean:
	rm sequential
	rm sequential_v2
	rm $(BIN)
