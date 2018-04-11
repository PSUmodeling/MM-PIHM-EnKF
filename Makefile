CC = gcc
CFLAGS = -g -O0 -Wall -fopenmp

SRCDIR = ./src
LIBS = -lm
INCLUDES = -I$(SRCDIR)/include

SRCS_ = read_func.c read_param.c perturb.c read_calib.c write_param.c

HEADERS_ = include/enkf.h\
	include/enkf_const.h\
	include/enkf_struct.h\
	include/enkf_func.h

EXECUTABLE = util/perturb

SRCS = $(patsubst %,$(SRCDIR)/%,$(SRCS_))
HEADERS = $(patsubst %,$(SRCDIR)/%,$(HEADERS_))
OBJS = $(SRCS:.c=.o)

.PHONY: all perturb clean help

all: perturb
	@chmod 755 ./*.sh

perturb: $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXECUTABLE) $(OBJS) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	@rm -f $(OBJS) $(EXECUTABLE)
