# Makefile for MM-PIHM EnKF v0.1.0-alpha

# Current MM-PIHM EnKF version
VERS = 0.1.0-alpha

# Please specify your MM-PIHM directory
PIHM_DIR = $$HOME/work/MM-PIHM

# Required MM-PIHM version
LOW_PIHM_VERS = 0.10.1-alpha
HIGH_PIHM_VERS = above

CC = gcc
CFLAGS = -g -O2

ifeq ($(WARNING), on)
  CFLAGS += -Wall -Wextra
endif

ifeq ($(DEBUG), on)
  CFLAGS += -O0
endif

ifneq ($(OMP), off)
  CFLAGS += -fopenmp
endif

SRCDIR = ./src
INCLUDES = -I$(SRCDIR)/include

PIHM_SRCDIR = $(PIHM_DIR)/src
PIHM_INCLUDES = -I$(PIHM_SRCDIR)/include
PIHM_OBJS_ = \
	free_mem.o\
	init_mesh.o\
	init_river.o\
	init_soil.o\
	init_topo.o\
	read_att.o\
	read_func.o\
	noah/lsm_func.o\
	noah/lsm_init.o\
	noah/lsm_read.o\
	noah/noah.o\
	read_calib.o\
	read_mesh.o\
	read_river.o\
	read_soil.o\
	soil.o\
	time_func.o
PIHM_LIB = ./lib/pihmlib.a

LIBS = -lm

ENKF_IO_SRCS_ =\
	custom_io.c\
	read_obs.c\
	read_obstbl.c\
	read_param.c\
	read_paramtbl.c\
	read_var.c\
	read_vartbl.c\
	write_ic.c\
	write_param.c\
	write_var.c
ENKF_IO_LIB = ./lib/enkfiolib.a

PERTURB_SRCS_ =\
	ltqnorm/ltqnorm.c\
	perturb.c

ASSIM_SRCS_ =\
	assim.c\
	build_pihm.c\
	cov_inflt.c\
	custom_io.c\
	enkf.c\
	forecast.c\
	obs_oper.c\
	util.c\
	water_bal.c

HEADERS_ =\
	include/custom_io.h\
	include/enkf.h\
	include/enkf_const.h\
	include/enkf_struct.h\
	include/enkf_func.h

PERTURB = util/perturb
ASSIM = util/assim

HEADERS = $(patsubst %,$(SRCDIR)/%,$(HEADERS_))

ENKF_IO_SRCS = $(patsubst %,$(SRCDIR)/%,$(ENKF_IO_SRCS_))
ENKF_IO_OBJS = $(ENKF_IO_SRCS:.c=.o)

PERTURB_SRCS = $(patsubst %,$(SRCDIR)/%,$(PERTURB_SRCS_))
PERTURB_OBJS = $(PERTURB_SRCS:.c=.o)

ASSIM_SRCS = $(patsubst %,$(SRCDIR)/%,$(ASSIM_SRCS_))
ASSIM_OBJS = $(ASSIM_SRCS:.c=.o)

PIHM_OBJS = $(patsubst %,$(PIHM_SRCDIR)/%,$(PIHM_OBJS_))

.PHONY: all check_version perturb assim clean

all: check_version pihmlib enkflib perturb assim
	@chmod 755 ./*.sh
	@chmod 755 ./util/*.sh

check_version:
	@util/check_version.sh $(PIHM_DIR) $(LOW_PIHM_VERS) $(HIGH_PIHM_VERS) $(VERS)

pihmlib:
	@mkdir -p lib
	ar rc $(PIHM_LIB) $(PIHM_OBJS)

enkflib: $(ENKF_IO_OBJS) $(HEADERS)
	ar rc $(ENKF_IO_LIB) $(ENKF_IO_OBJS)

perturb: $(PERTURB_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -o $(PERTURB) $(PERTURB_OBJS) $(ENKF_IO_LIB) $(PIHM_LIB) $(LIBS)

assim: $(ASSIM_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -o $(ASSIM) $(ASSIM_OBJS) $(ENKF_IO_LIB) $(PIHM_LIB) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $(PIHM_INCLUDES) -c $< -o $@

clean:
	@rm -f $(ENKF_IO_OBJS) $(PERTURB_OBJS) $(PERTURB) $(ASSIM_OBJS) $(ASSIM) $(PIHM_LIB) $(ENKF_IO_LIB)
