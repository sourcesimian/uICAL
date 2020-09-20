#!/bin/bash
set -e
/usr/bin/setupuser ${USER} $(id -u) ${HOST_GROUP} $(id -g) ${HOME} </dev/null
unset HOST_GROUP

EXTRA_PATH=$(echo $HOST_PATH | tr ':' '\n' | grep "^$HOME" | sort -u | xargs echo | tr ' ' ':')
if [ -n "$EXTRA_PATH" ]; then
    export PATH=$PATH:$EXTRA_PATH
fi
unset HOST_PATH

export BASH_OPTS=$HOST_BASH_OPTS
unset HOST_BASH_OPTS

if [ -n "$1" ]; then
    exec "${@}"
fi

exec /bin/bash
