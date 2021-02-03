
CXX	= g++ -std=c++17
#CFLAGS 	= `pkg-config --cflags opencv` #-pg
CFLAGS = -Wall -DTRACE_FASTFLOW
LIBS = -pthread
RPL	= $(RPL_HOME)
FF = $(FF_HOME)
JSON = ../
#FF	= ../../fastflow/
OPT = -O3
DBG = -O0 -g -DDEBUG
#LIBS 	= `pkg-config --libs opencv`

HEADERS=$(wildcard ../*.hpp)

FFSRC=$(wildcard *ff.cpp)
SEQSRC=$(wildcard *seq.cpp)
#SRC=$(wildcard *.cpp)

SEQBINDEB=$(patsubst %seq.cpp,%seq,$(SEQSRC))
FFBINDEB=$(patsubst %ff.cpp,%ff,$(FFSRC))
SEQBINOPT=$(patsubst %seq.cpp,%seq_opt,$(SEQSRC))
FFBINOPT=$(patsubst %ff.cpp,%ff_opt,$(FFSRC))

.SILENT: test_seq test_ff test_all clean

all: ff sequential

ff:	$(FFBINDEB)

sequential: $(SEQBINDEB)

opt: $(SEQBINOPT) $(FFBINOPT)

$(FFBINDEB): $(FFSRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(FF) -I$(RPL) -I$(JSON) $(LIBS) -o $@ $@.cpp

$(SEQBINDEB): $(SEQSRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -I$(JSON) -o $@ $@.cpp

$(FFBINOPT): $(FFSRC) $(HEADERS)
	$(foreach f, $(FFBINDEB), $(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) -I$(JSON) $(LIBS) -o $(f)_opt  $(f).cpp;)

$(SEQBINOPT): $(SEQSRC) $(HEADERS)
	$(CXX) $(DBG) $(CFLAGS) -I$(RPL) -I$(JSON) -o $@ $<


test_seq: $(SEQBINOPT)
	$(foreach exec, $(SEQBINOPT), echo "\nExecuting $(exec)\n"; ./$(exec);)

# test_ff_debug: $(FFBINDEB)
# 	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_ff: $(FFBINOPT)
	$(foreach exec, $(FFBINOPT), echo "\nExecuting $(exec)\n"; ./$(exec);)

# test_all_debug: $(SEQBINDEB) $(FFBINDEB)
# 	$(foreach exec, $(SEQBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")
# 	$(foreach exec, $(FFBINDEB), echo "Executing $(exec)\n"; ./$(exec); echo "------------\n")

test_all: $(SEQBINOPT) $(FFBINOPT)
	$(foreach exec, $(SEQBINOPT), echo "\nExecuting $(exec)\n"; ./$(exec);)
	$(foreach exec, $(FFBINOPT), echo "\nExecuting $(exec)\n"; ./$(exec);)

remove_ff_code:
	$(RM) $(FFSRC)

clean:
	$(RM) $(SEQBINOPT)
	$(RM) $(SEQBINDEB)
	$(RM) $(FFBINOPT)
	$(RM) $(FFBINDEB)
	$(RM) res_seq.txt
	$(RM) res_ff.txt
