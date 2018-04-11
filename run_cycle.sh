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

# Create initial parameter values through perturbation
if [ $PERTURB_MODE -lt 3 ] ; then
    echo
    echo "* Perturb initial parameter values..."
    echo
    . ./util/perturb.sh
fi
