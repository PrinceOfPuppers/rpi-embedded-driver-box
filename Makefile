CC = gcc

SRC = src

# Where the .a and .o files are compiled to
LIB = lib

PROGRAM = sense-api
CFLAGS= -g -O2 -Wall -lm

INC = -Iinclude

FILES = $(wildcard $(SRC)/*.c)
FILENAMES = $(basename $(notdir $(wildcard $(SRC)/*.c) ))

# Testing exclusive
TESTDIR = testing
TESTFILES = $(wildcard $(TESTDIR)/$(SRC)/*.c)
TESTINCLUDE = -I$(TESTDIR)/include
TESTBUILD = build
TESTPROGRAM = test
TESTLIB = -L lib -l:$(PROGRAM).a 

all: debug

debug: CFLAGS += -g
debug: compile

compile:
	@mkdir -p $(LIB)
	for path in $(FILENAMES); do \
		$(CC) $(CFLAGS) -c $(SRC)/$$path.c $(INC) -o $(LIB)/$$path.o; \
	done
	
	ar rcs $(LIB)/$(PROGRAM).a $(addprefix $(LIB)/, $(addsuffix .o,$(FILENAMES)))	

test: debug
	@mkdir -p $(TESTDIR)/$(TESTBUILD)
	$(CC) $(CFLAGS) $(INC) $(TESTINCLUDE) $(TESTFILES) -o $(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM) $(TESTLIB)

	./$(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM)

clean:
	rm -rf $(LIB)/*
	rm -rf $(TESTDIR)/$(TESTBUILD)/*
