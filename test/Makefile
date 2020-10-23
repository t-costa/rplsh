
CXX	= g++ -std=c++17
#CFLAGS 	= `pkg-config --cflags opencv` #-pg
CFLAGS = -Wall -pedantic
LIBS = -pthread
RPL	= ../
FF = $(FF_HOME)
#FF	= ../../fastflow/
OPT = -O3
DBG = -O0 -g -DDEBUG
#LIBS 	= `pkg-config --libs opencv`

HEADERS=$(wildcard *.hpp)

FFSRC=$(wildcard *ff.cpp)
SEQSRC=$(wildcard *seq.cpp)
SRC=$(wildcard *.cpp)

SEQBIN=$(patsubst %seq.cpp,%seq,$(SEQSRC))
FFBIN=$(patsubst %ff.cpp,%ff,$(FFSRC))
SEQBINDEB=$(patsubst %seq.cpp,debug_%seq,$(SEQSRC))
FFBINDEB=$(patsubst %ff.cpp,debug_%ff,$(FFSRC))


all: ff sequential

ff:	$(FFBIN)

sequential: $(SEQBIN)

$(FFBIN): $(FFSRC) $(HEADERS)
	$(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) -o $@ $< $(LIBS)

$(SEQBIN): $(SEQSRC) $(HEADERS)
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $<

ff_debug: $(FFBINDEB)

sequential_debug: $(SEQBINDEB)

$(SEQBINDEB): $(SRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -o $@ $<

$(FFBINDEB): $(FFSRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(FF) -I$(RPL) -o $@ $< $(LIBS)

test_seq_debug: $(SEQBINDEB)
	$(foreach exec, $(SEQBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_seq: $(SEQBIN)
	$(foreach exec, $(SEQBIN), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_ff_debug: $(FFBINDEB)
	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_ff: $(FFBIN)
	$(foreach exec, $(FFBIN), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_all_debug: $(SEQBINDEB) $(FFBINDEB)
	$(foreach exec, $(SEQBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")
	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_all: $(SEQBIN) $(FFBIN)
	$(foreach exec, $(SEQBIN), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")
	$(foreach exec, $(FFBIN), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")



clean:
	$(RM) $(SEQBIN)
	$(RM) $(SEQBINDEB)
	$(RM) $(FFBIN)
	$(RM) $(FFBINDEB)
