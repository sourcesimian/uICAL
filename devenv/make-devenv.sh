#!/bin/bash
set -e
BASE_IMAGE=ubuntu:20.04
REPO_TAG=sourcesimian/uical/devenv

HERE=$(cd $(dirname "$0"); pwd)
BUILD_DIR=${HERE}/../.build
LAUNCHER=$(cd ${HERE}/..; pwd)/de

# Setup Build area
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
trap "rm -rf ${BUILD_DIR}" EXIT INT TERM KILL

cp ${HERE}/docker/* ${BUILD_DIR}

function write_dockerfile_devenv() {
cat <<EOF > ${1:?DOCKERFILE Required}
FROM ${BASE_IMAGE} as base

RUN apt-get update && \
	apt-get install -y \
        locales \
		git \
        clang \
		python3 \
		python3-distutils \
        curl \

# Locale
RUN locale-gen en_US.UTF-8 && update-locale LC_ALL=en_US.UTF-8 LANG=en_C.UTF-8

# User setup
COPY ./setupuser.c /
RUN clang -o /usr/bin/setupuser /setupuser.c && chmod 4511 /usr/bin/setupuser

# Python
RUN curl -kLo /get-pip.py https://bootstrap.pypa.io/get-pip.py
COPY ./python3-requirements.txt /
RUN python3 /get-pip.py && pip3 install -r /python3-requirements.txt

RUN apt-get install -y make


COPY ./usage.sh /
ENTRYPOINT ./usage.sh
COPY ./entrypoint.sh /
COPY ./launcher.sh /

EOF
}

function build_devenv() {
	write_dockerfile_devenv ${BUILD_DIR}/Dockerfile
	echo "# Docker build devenv: ${BUILD_DIR}"
	docker build \
		--build-arg http_proxy=${http_proxy} \
		--build-arg https_proxy=${https_proxy} \
		--build-arg no_proxy=${no_proxy} \
		--tag ${REPO_TAG} \
		--file ${BUILD_DIR}/Dockerfile \
		${BUILD_DIR}
}

build_devenv

docker run -i --rm --entrypoint /bin/cat sourcesimian/uical/devenv /launcher.sh > $LAUNCHER
chmod +x $LAUNCHER
echo "Wrote: $LAUNCHER"
