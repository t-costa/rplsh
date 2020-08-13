
CXX	= g++ -std=c++17
#CFLAGS 	= `pkg-config --cflags opencv` #-pg
CFLAGS = -Wall -pedantic
LIBS = -pthread
RPL	= ../../
FF = $(FF_HOME)
#FF	= ../../fastflow/
OPT = -O3
DBG = -O0 -g -DDEBUG
#LIBS 	= `pkg-config --libs opencv`

SRC=$(wildcard ff*.cpp)
BIN=$(patsubst %.cpp,%,$(SRC))

ff:	$(BIN)

sequential_debug: sequential.cpp business.hpp
	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -o $@ $<

sequential: sequential.cpp business.hpp
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $<

# sequential_v2: sequential_v2.cpp business_v2.hpp
# 	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $< $(LIBS)

$(BIN): %: %.cpp business.hpp
	$(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) -o $@ $< $(LIBS)

ff_debug: $(SRC) business.hpp
	$(CXX) $(DBG) $(CFLAGS) -I$(FF) -I$(RPL) -o $@ $< $(LIBS)


clean:
	rm sequential
	# rm sequential_v2
	rm $(BIN)
