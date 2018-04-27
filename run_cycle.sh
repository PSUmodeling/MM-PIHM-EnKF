#!/bin/sh

# Bash script for MM-PIHM EnKF sytem
# Author: Yuning Shi (yshi.at.psu.edu)

echo
echo "##     ## ##     ##       ########  #### ##     ## ##     ##    ######## ##    ## ##    ## ########"
echo "###   ### ###   ###       ##     ##  ##  ##     ## ###   ###    ##       ###   ## ##   ##  ##      "
echo "#### #### #### ####       ##     ##  ##  ##     ## #### ####    ##       ####  ## ##  ##   ##      "
echo "## ### ## ## ### ## ##### ########   ##  ######### ## ### ##    ######   ## ## ## #####    ######  "
echo "##     ## ##     ##       ##         ##  ##     ## ##     ##    ##       ##  #### ##  ##   ##      "
echo "##     ## ##     ##       ##         ##  ##     ## ##     ##    ##       ##   ### ##   ##  ##      "
echo "##     ## ##     ##       ##        #### ##     ## ##     ##    ######## ##    ## ##    ## ##      "
echo

###########################
# Read configuration file #
###########################
CONFIG_FILE=./enkf.config
chmod 700 $CONFIG_FILE
. $CONFIG_FILE

################################
# Set MM-PIHM output directory #
################################
if [ -z "$OUTPUT_DIR" ]; then
    OUTPUT_DIR=$PROJECT$(date +".%y%m%d%H%M")
    echo "OUPUT_DIR is set to $OUTPUT_DIR"
fi

mkdir -p $PIHM_DIR"/output"
mkdir -p $PIHM_DIR"/output/"$OUTPUT_DIR

##############################################
# Read number of elements and river segments #
##############################################
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
export INFLT_WEIGHT
export NELEM
export NRIVER

WORK_DIR=$PWD

########################################################
# Create initial parameter values through perturbation #
########################################################
if [ $PERTURB_MODE -lt 3 ] ; then
    echo
    echo "* Perturb initial parameter values..."
    echo
    ./util/perturb $(date -d "$CYCLE_START_TIME" +"%s" --utc)
fi

###################################################################
# Run MM-PIHM EnKF cycles from CYCLE_START_TIME to CYCLE_END_TIME #
###################################################################
echo
echo "* Run MM-PIHM EnKF Cycles..."
echo

C_START_TIME=$CYCLE_START_TIME
C_END_TIME=$ASSIM_START_TIME

while [ $(date -d "$C_START_TIME" +"%s" --utc) -lt $(date -d "$CYCLE_END_TIME" +"%s" --utc) ]
do
    if [ "$C_END_TIME" == "$ASSIM_START_TIME" ] ; then
        FIRST_CYCLE=1
    else
        FIRST_CYCLE=0
    fi

    echo "  Run MM-PIHM from $C_START_TIME to $C_END_TIME"

    # Write MM-PIHM control parameter file
    . ./util/write_para.sh "$C_START_TIME" "$C_END_TIME" $FIRST_CYCLE

    # Run ensemble simulations
    . ./util/run_ens.sh $FIRST_CYCLE

    # Assimilation
    OBS_TIME=$(date -d "$C_END_TIME" +"%s" --utc)

    cd $WORK_DIR
    ./util/assim $OBS_TIME $FIRST_CYCLE

    # Advance time by assimilation interval
    C_START_TIME=$(date -d "$C_START_TIME $ASSIM_INTVL hour" +"%Y-%m-%d %H:%M" --utc)
    C_END_TIME=$(date -d "$C_END_TIME $ASSIM_INTVL hour" +"%Y-%m-%d %H:%M" --utc)
done
