ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I.
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm

VIEWER = pvisu
DATA   = ../data

BIN=\
	make-se\
	labeling\
	dilation

.PHONY: all
all: $(OBJ) $(BIN)

morphology.o: se.o
make-se: se.o
dilation: morphology.o

.PHONY: extract-gear
extract-gear:
	$(VIEWER) $(DATA)/gear.ppm

.PHONY: morphology-mrg morphology-lex morphology-bit
morphology-mrg: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color
morphology-lex: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color
morphology-bit: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color

.PHONY: test-color
test-color: all
	./dilation 2 1 $(DATA)/mm-color.ppm a.ppm; #$(VIEWER) a.ppm

.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN)

