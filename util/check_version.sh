#!/bin/sh

PIHM_DIR=$1
REQ_VERSION=$2
ENKF_VERSION=$3
cd $PIHM_DIR

CUR_VERS=$(./flux-pihm -V |grep "MM-PIHM" |awk '{print $3}')

if [ "$(printf '%s\n' $CUR_VERS $REQ_VERSION | sort -V | head -n 1)" != "$REQ_VERSION" ] ; then
    echo "MM-PIHM EnKF v$ENKF_VERSION requires MM-PIHM v$REQ_VERSION or above."
    echo "Please go to https://github.com/PSUmodeling/MM-PIHM/releases and choose a compatible version."
    exit 1
fi
