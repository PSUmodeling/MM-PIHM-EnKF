#!/bin/sh

PIHM_DIR=$1
LOW_VERSION=$2
HIGH_VERSION=$3
ENKF_VERSION=$4

# Check MM-PIHM version
cd $PIHM_DIR
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
