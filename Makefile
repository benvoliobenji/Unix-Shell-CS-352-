CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

CFLAGS = -Wall -Werror -ggdb -funroll-loops
CXXFLAGS = -std=c++14 -Wall -g

BIN = myshell
OBJS = myshell.o date.o

all: myshell.cpp
	@$(ECHO) Compiling myshell
	g++ -std=c++14 -Wall -g myshell.cpp -o myshell

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch]