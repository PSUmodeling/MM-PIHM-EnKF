CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -fopenmp

SRCDIR = ./src
INCLUDES = -I$(SRCDIR)/include

PIHM_SRCDIR =~/scratch/MM-PIHM/src
PIHM_INCLUDES = -I$(PIHMSRCDIR)/include
PIHM_OBJS_ = read_mesh.o read_river.o
PIHM_LIB = ./lib/pihmlib.a

LIBS = -lm

PERTURB_SRCS_ = perturb.c read_param.c custom_io.c read_paramtbl.c write_param.c
ASSIM_SRCS_ = assim.c custom_io.c read_var.c read_vartbl.c read_param.c read_paramtbl.c util.c

HEADERS_ =\
	include/enkf.h\
	include/enkf_const.h\
	include/enkf_struct.h\
	include/enkf_func.h

PERTURB = util/perturb
ASSIM = util/assim

HEADERS = $(patsubst %,$(SRCDIR)/%,$(HEADERS_))

PERTURB_SRCS = $(patsubst %,$(SRCDIR)/%,$(PERTURB_SRCS_))
PERTURB_OBJS = $(PERTURB_SRCS:.c=.o)

ASSIM_SRCS = $(patsubst %,$(SRCDIR)/%,$(ASSIM_SRCS_))
ASSIM_OBJS = $(ASSIM_SRCS:.c=.o)

PIHM_OBJS = $(patsubst %,$(PIHM_SRCDIR)/%,$(PIHM_OBJS_))

.PHONY: all perturb assim clean

all: perturb assim
	@chmod 755 ./*.sh
	@chmod 755 ./util/*.sh

perturb: $(PERTURB_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(PERTURB) $(PERTURB_OBJS) $(LIBS)

assim: $(ASSIM_OBJS) $(HEADERS)
	@mkdir -p lib
	ar rc $(PIHM_LIB)  $(PIHM_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(ASSIM) $(ASSIM_OBJS) $(PIHM_LIB) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	@rm -f $(PERTURB_OBJS) $(PERTURB) $(ASSIM_OBJS) $(ASSIM) $(PIHM_LIB)
