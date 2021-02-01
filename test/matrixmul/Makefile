
CXX	= g++ -std=c++17
#CFLAGS 	= `pkg-config --cflags opencv` #-pg
CFLAGS = -Wall
LIBS = -pthread
RPL	= ../../
JSON = ../
FF = $(FF_HOME)
#FF	= ../../fastflow/
OPT = -O3
DBG = -O0 -g -DDEBUG
#LIBS 	= `pkg-config --libs opencv`

HEADERS=$(wildcard ../*.hpp)

FFSRC=$(wildcard *ff.cpp)
SEQSRC=$(wildcard *seq.cpp)
#SRC=$(wildcard *.cpp)

SEQBIN=$(patsubst %seq.cpp,%seq,$(SEQSRC))
FFBIN=$(patsubst %ff.cpp,%ff,$(FFSRC))
# FFBINOPT=$(patsubst %ff.cpp,%opt_ff,$(FFSRC))
# SEQBINOPT=$(patsubst %seq.cpp,%opt_seq,$(SEQSRC))
# SEQBINDEB=$(patsubst %seq.cpp,%seq,$(SEQSRC))
# FFBINDEB=$(patsubst %ff.cpp,%ff,$(FFSRC))

.SILENT: test_seq test_ff test_all clean

all: ff sequential

ff:	$(FFBIN)

sequential: $(SEQBIN)

$(FFBIN): $(FFSRC) $(HEADERS)
	$(CXX) $(OPT) $(CFLAGS) -I$(FF) -I$(RPL) -I$(JSON) -o $@ $@.cpp $(LIBS)

$(SEQBIN): $(SEQSRC) $(HEADERS)
	$(CXX) $(OPT) $(CFLAGS) -I$(RPL) -I$(JSON) -o $@ $@.cpp


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
	$(RM) $(FFSRC)

clean:
	$(RM) $(SEQBIN)
	$(RM) $(SEQBINDEB)
	$(RM) $(FFBIN)
	$(RM) $(FFBINDEB)
	$(RM) res_seq.txt
	$(RM) res_ff.txt
