#ifndef ENKF_HEADER
#define ENKF_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#if defined(_OPENMP)
# include <omp.h>
#endif

#define VERSION    "0.1.0-alpha"

#include "custom_io.h"

#include "enkf_const.h"
#include "enkf_struct.h"
#include "enkf_func.h"

#endif
