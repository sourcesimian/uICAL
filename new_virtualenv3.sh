#!/bin/bash
set -e

# Select python versions based on the suffix
SCRIPTNAME=$(basename "$0"); SCRIPTNAME=${SCRIPTNAME%.sh}
case "${SCRIPTNAME: -1}" in
    3) VER=3;;
    *|2) VER=2;;
esac


CACHE=~/.cache/ve
GET_PIP=${CACHE}/get_pip.py
PACKAGES=${CACHE}/site-packages${VER}

# Setup cache with pip and virtualenv executable
if [ ! -e ${GET_PIP} ]; then
    mkdir -p ${CACHE}
    curl -o ${GET_PIP} https://bootstrap.pypa.io/get-pip.py
fi

if [ ! -e ${PACKAGES}/virtualenv.py ]; then
    mkdir -p ${PACKAGES}
    python${VER} ${GET_PIP} --target ${PACKAGES}/ virtualenv
fi

# Verify local env
if [ -n "${VIRTUAL_ENV}" ]; then
    echo "! Already in virtualenv: $(which python${VER})"
    exit 1
fi

for arg in "${@}"; do
    if [ -e "$arg" ]; then
        echo "! $arg already exists" >&2
        exit 1
    fi
done

python${VER} ${PACKAGES}/virtualenv.py "${@}"
