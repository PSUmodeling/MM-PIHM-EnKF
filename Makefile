CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -fopenmp

SRCDIR = ./src
INCLUDES = -I$(SRCDIR)/include

PIHM_SRCDIR = $$HOME/work/MM-PIHM/src
PIHM_INCLUDES = -I$(PIHM_SRCDIR)/include
PIHM_OBJS_ = \
	init_mesh.o\
	init_river.o\
	init_soil.o\
	init_topo.o\
	read_att.o\
	read_func.o\
	noah/lsm_read.o\
	read_mesh.o\
	read_river.o\
	read_soil.o\
	soil.o\
	time_func.o
PIHM_LIB = ./lib/pihmlib.a

LIBS = -lm

PERTURB_SRCS_ = perturb.c read_param.c custom_io.c read_paramtbl.c write_param.c
ASSIM_SRCS_ =\
	assim.c\
	build_pihm.c\
	cov_inflt.c\
	custom_io.c\
	enkf.c\
	forecast.c\
	obs_oper.c\
	read_obs.c\
	read_obstbl.c\
	read_var.c\
	read_vartbl.c\
	read_param.c\
	read_paramtbl.c\
	util.c

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
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -o $(PERTURB) $(PERTURB_OBJS) $(LIBS)

assim: $(ASSIM_OBJS) $(HEADERS)
	@mkdir -p lib
	ar rc $(PIHM_LIB)  $(PIHM_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -o $(ASSIM) $(ASSIM_OBJS) $(PIHM_LIB) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -c $<  -o $@

clean:
	@rm -f $(PERTURB_OBJS) $(PERTURB) $(ASSIM_OBJS) $(ASSIM) $(PIHM_LIB)
