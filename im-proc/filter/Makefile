ROOT=../bcl

CPPFLAGS = -I. -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lm -lfftw3 -lbcl

BIN=heat-equation\
	butterworth

.PHONY=all
all: $(BIN)

butterworth: fft.o

.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN)


