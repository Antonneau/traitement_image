ROOT=../bcl

CPPFLAGS = -I. -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lm -lfftw3 -lbcl

BIN=

VIEWER=pvisu
DATA=../data

.PHONY: pipeline
pipeline:
	@echo ">>> Pipeline for image registration"
	@$(VIEWER) $(DATA)/test-01.ppm

.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN)


