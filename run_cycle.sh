#!/bin/sh

# Bash script for MM-PIHM EnKF sytem
# Author: Yuning Shi (yshi.at.psu.edu)

echo "##############################################"
echo "# MM-PIHM EnKF System                        #"
echo "##############################################"

# Read configuration file
CONFIG_FILE=./enkf.config
chmod 700 $CONFIG_FILE
. $CONFIG_FILE

# Set MM-PIHM output directory
if [ -z "$OUTPUT_DIR" ]; then
    OUTPUT_DIR=$PROJECT$(date +".%y%m%d%H%M")
    echo "OUPUT_DIR is set to $OUTPUT_DIR"
fi

mkdir -p $PIHM_DIR"/output"
mkdir -p $PIHM_DIR"/output/"$OUTPUT_DIR

# Read number of elements and river segments
NELEM=$( head -1 $PIHM_DIR"/input/"$PROJECT"/"$PROJECT".mesh" |awk '{print $2}' )
NRIVER=$( head -1 $PIHM_DIR"/input/"$PROJECT"/"$PROJECT".riv" |awk '{print $2}' )

# Export variables
export PIHM_DIR
export PROJECT
export OUTPUT_DIR
export PARAM_TBL
export VAR_TBL
export OBS_TBL
export PERTURB_MODE
export NUM_MEMBER
export NELEM
export NRIVER

#
# Create initial parameter values through perturbation
#
if [ $PERTURB_MODE -lt 3 ] ; then
    echo
    echo "* Perturb initial parameter values..."
    echo
    ./util/perturb $PIHM_DIR $PROJECT $OUTPUT_DIR $PARAM_TBL $PERTURB_MODE $NUM_MEMBER "$CYCLE_START_TIME"
fi


C_START_TIME=$CYCLE_START_TIME
C_END_TIME=$ASSIM_START_TIME
INIT_MODE=0

# Write MM-PIHM control parameter file
. ./util/write_para.sh "$C_START_TIME" "$C_END_TIME" $INIT_MODE

# Run ensemble simulations
. ./util/run_ens.sh

# Assimilation

