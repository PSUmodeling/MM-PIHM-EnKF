#!/bin/sh

PIHM_DIR=$1
LOW_VERSION=$2
HIGH_VERSION=$3
ENKF_VERSION=$4

# Check if PIHM_DIR exists
if [ ! -d $PIHM_DIR ]; then
    echo "The MM-PIHM directory does not exist."
    echo "Please check if the PIHM_DIR path in the Makefile is correct."
    exit 1
fi

cd $PIHM_DIR

# Check if Flux-PIHM executable exists
if [ ! -e flux-pihm ]; then
    echo "Cannot find Flux-PIHM executable in the MM-PIHM directory."
    echo "Please check if the PIHM_DIR path in the Makefile is correct."
    exit 1
fi

# Check MM-PIHM version
CUR_VERS=$(./flux-pihm -V |grep "MM-PIHM" |awk '{print $3}')

# Check if MM-PIHM version is compatible
if [ "$(printf '%s\n' $CUR_VERS $LOW_VERSION $HIGH_VERSION | sort -V | sed -n 2p)" != "$CUR_VERS" ]; then
    if [ "$HIGH_VERSION" = "above" ]; then
        echo "MM-PIHM EnKF v$ENKF_VERSION requires MM-PIHM v$LOW_VERSION or $HIGH_VERSION."
    else
        echo "MM-PIHM EnKF v$ENKF_VERSION requires MM-PIHM v$LOW_VERSION to v$HIGH_VERSION."
    fi
    echo "Currently MM-PIHM v$CUR_VERS is installed."
    echo "Please go to https://github.com/PSUmodeling/MM-PIHM/releases and choose a compatible version."
    exit 1
fi
