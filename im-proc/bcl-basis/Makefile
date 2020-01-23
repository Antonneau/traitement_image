ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl

BIN=\
	test-bcl\
	extract-subimage

.PHONY: all
all:$(BIN)

.PHONY: clean cleanall
clean:
	$(RM) *.o *~ *.ppm
cleanall: clean
	$(RM) $(BIN) 



