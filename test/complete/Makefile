
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

HEADERS=$(wildcard *.hpp)

FFSRC=$(wildcard *ff.cpp)
SEQSRC=$(wildcard *seq.cpp)
#SRC=$(wildcard *.cpp)

SEQBIN=$(patsubst %seq.cpp,%seq,$(SEQSRC))
FFBIN=$(patsubst %ff.cpp,%ff,$(FFSRC))
# SEQBINDEB=$(patsubst %seq.cpp,%seq,$(SEQSRC))
# FFBINDEB=$(patsubst %ff.cpp,%ff,$(FFSRC))

.SILENT: test_seq test_ff test_all clean

all: ff sequential

prova: $(SEQSRC)
	echo $?

ff:	$(FFBIN)

sequential: $(SEQBIN)

$(FFBIN): $(FFSRC) $(HEADERS)
<<<<<<< HEAD
	$(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) $(LIBS) -o $@ $@.cpp

$(SEQBIN): $(SEQSRC) $(HEADERS)
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $@.cpp
=======
	$(CXX) $(DBG) $(CFLAGS) -I$(FF) -I$(RPL) $(LIBS) -o $@ $@.cpp

$(SEQBIN): $(SEQSRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -o $@ $@.cpp
>>>>>>> e9dce9cf3051f15f46492d03c728495952c63d13
#per opt
#$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -o $@ $@.cpp

# ff_debug: $(FFBINDEB)
#
# sequential_debug: $(SEQBINDEB)
#
# $(SEQBINDEB): $(SEQSRC) $(HEADERS)
# 	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -o debug_$@ $@.cpp
#
# $(FFBINDEB): $(FFSRC) $(HEADERS)
# 	$(CXX) $(DBG) $(CFLAGS) -I$(FF) -I$(RPL) -o debug_ $@.cpp $(LIBS)

# test_seq_debug: $(SEQBINDEB)
# 	$(foreach exec, $(SEQBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_seq: $(SEQBIN)
	$(foreach exec, $(SEQBIN), echo "\nExecuting $(exec)\n"; ./$(exec);)

# test_ff_debug: $(FFBINDEB)
# 	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_ff: $(FFBIN)
	$(foreach exec, $(FFBIN), echo "\nExecuting $(exec)\n"; ./$(exec);)

# test_all_debug: $(SEQBINDEB) $(FFBINDEB)
# 	$(foreach exec, $(SEQBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")
# 	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_all: $(SEQBIN) $(FFBIN)
	$(foreach exec, $(SEQBIN), echo "\nExecuting $(exec)\n"; ./$(exec);)
	$(foreach exec, $(FFBIN), echo "\nExecuting $(exec)\n"; ./$(exec);)

remove_ff_code:
	rm $(FFSRC)

clean:
	$(RM) $(SEQBIN)
	$(RM) $(SEQBINDEB)
	$(RM) $(FFBIN)
	$(RM) $(FFBINDEB)
	$(RM) res_seq.txt
	$(RM) res_ff.txt
