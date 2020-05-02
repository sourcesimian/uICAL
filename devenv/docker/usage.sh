#!/bin/bash

cat << EOF
Launch the environemnt using:
$ bash <(docker run -i --rm --entrypoint /bin/cat sourcesimian/uical/devenv /launcher.sh)
EOF
