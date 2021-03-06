#!/bin/sh

START_TIME=$1
END_TIME=$2
first_cycle=$3

if [ $first_cycle == 1 ]; then
    INIT_MODE=0
else
    INIT_MODE=1
fi

# Create parameter file
cat << EOF > $PIHM_DIR"/input/"$PROJECT"/"$PROJECT".para"
SIMULATION_MODE     0
INIT_MODE           $INIT_MODE
ASCII_OUTPUT        0
WATBAL_OUTPUT       0
WRITE_IC            0
UNSAT_MODE          2
SURF_MODE           2
RIV_MODE            2
START               $START_TIME
END                 $END_TIME
MAX_SPINUP_YEAR     0
MODEL_STEPSIZE      60
LSM_STEP            900

ABSTOL              1E-4
RELTOL              1E-3
INIT_SOLVER_STEP    5E-5
NUM_NONCOV_FAIL     0.0
MAX_NONLIN_ITER     3.0
MIN_NONLIN_ITER     1.0
DECR_FACTOR         1.2
INCR_FACTOR         1.2
MIN_MAXSTEP         1.0

SURF                HOURLY
UNSAT               HOURLY
GW                  HOURLY
RIVSTG              HOURLY
RIVGW               HOURLY
SNOW                HOURLY
CMC                 HOURLY
INFIL               HOURLY
RECHARGE            HOURLY
EC                  HOURLY
ETT                 HOURLY
EDIR                HOURLY
RIVFLX0             HOURLY
RIVFLX1             HOURLY
RIVFLX2             HOURLY
RIVFLX3             HOURLY
RIVFLX4             HOURLY
RIVFLX5             HOURLY
RIVFLX6             HOURLY
RIVFLX7             HOURLY
RIVFLX8             HOURLY
RIVFLX9             HOURLY
RIVFLX10            HOURLY
SUBFLX              HOURLY
SURFFLX             HOURLY
IC                  0
EOF
