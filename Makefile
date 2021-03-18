#CC = gcc
#
#SRC = src
#BUILD = build
#PROGRAM = main
#CFLAGS= -Wall -g -O2
#
#INC = -Iinclude# -Ilib/clog/include
#LIB = #-L ./lib -l:clog/lib/clog.a
#
#
#FILES = $(wildcard $(SRC)/*.c)
#
## directories with makefiles which must be called
## SUBMAKE = lib/clog
#
#
#all: debug run
#
##makeLibs:
##	for dir in $(SUBMAKE); do \
##		$(MAKE) -C $$dir ; \
##	done
#
##testLibs: 
##	for dir in $(SUBMAKE); do \
##		$(MAKE) -C $$dir test; \
##	done
#
##cleanLibs:
##	for dir in $(SUBMAKE); do \
##		$(MAKE) -C $$dir clean; \
##	done
#
#debug: #makeLibs
#	@mkdir -p $(BUILD)
#	$(CC) $(CFLAGS) $(FILES) $(LIB) $(INC) -o $(BUILD)/$(PROGRAM)
#
#run:
#	./$(BUILD)/$(PROGRAM)
#
#clean: #cleanLibs
#	rm -rf $(BUILD)/*






CC = gcc

SRC = src

# Where the .a and .o files are compiled to
LIB = lib

PROGRAM = sense-api
CFLAGS= -g -O2 -Wall

INC = -Iinclude

FILES = $(wildcard $(SRC)/*.c)
FILENAMES = $(basename $(notdir $(wildcard $(SRC)/*.c) ))

# Testing exclusive
TESTCFLAGS = -g -Wall
TESTDIR = testing
TESTFILES = $(wildcard $(TESTDIR)/$(SRC)/*.c)
TESTINCLUDE = -I$(TESTDIR)/include
TESTBUILD = build
TESTPROGRAM = test
TESTLIB = -L lib -l:$(PROGRAM).a 

all: debug

debug: 
	@mkdir -p $(LIB)
	for path in $(FILENAMES); do \
		$(CC) $(CFLAGS) -c $(SRC)/$$path.c $(INC) -o $(LIB)/$$path.o; \
	done
	
	ar rcs $(LIB)/$(PROGRAM).a $(addprefix $(LIB)/, $(addsuffix .o,$(FILENAMES)))

test: debug
	@mkdir -p $(TESTDIR)/$(TESTBUILD)
	$(CC) $(TESTCFLAGS) $(INC) $(TESTINCLUDE) $(TESTFILES) -o $(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM) $(TESTLIB)

	./$(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM)

clean:
	rm -rf $(LIB)/*
	rm -rf $(TESTDIR)/$(TESTBUILD)/*