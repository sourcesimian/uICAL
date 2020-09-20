#!/bin/bash
REPO_TAG=sourcesimian/uical/devenv
HERE="$(cd $(dirname "$BASH_SOURCE"); pwd)"

user_id() {
    case "$(uname)" in
        Linux*) echo "$(id -u):$(id -g)";;
        Darwin*) echo "$(id -u)";;
        *) echo "\"$(uname)\" not supported"; return 1;;
    esac
}

host_env() {
    {
        echo "HOST_GROUP=$(id -g -n)"
        echo "HOST_PATH=${PATH}"
        echo "HOST_BASH_OPTS=$-"
    } | awk '{print "--env "$1}' | tr '\n' ' '
}

user_env() {
    {
        env | grep -e '^USER=' -e '^HOME='
        [ -n "$HOSTNAME" ] && echo "HOSTNAME=$HOSTNAME"
    } | awk '{print "--env "$1}' | tr '\n' ' '
}

dev_env() {
    {
        echo "PYTHONPATH=$(printf '%q' "${HERE}/build/lib.linux-x86_64-3.8/")"
    } | awk '{print "--env "$1}' | tr '\n' ' '
}


proxy_env() {
    {
        env | grep -i -e '^HTTP_PROXY=' -e '^HTTPS_PROXY=' -e '^NO_PROXY='
    } | awk '{print "--env "$1}' | tr '\n' ' '
}

mount_user() {
    {
        echo "${HOME}:${HOME}:rw"
        [ -e /tmp ] && echo "/tmp:/tmp:rw"
    }  | awk '{print "--volume "$1}' | tr '\n' ' '
}


DOCKER_RUN=(docker run -i --rm)

[ -t 0 ] && DOCKER_RUN+=(-t)

DOCKER_RUN+=($(user_env))
DOCKER_RUN+=($(host_env))
DOCKER_RUN+=($(proxy_env))
DOCKER_RUN+=($(dev_env))
DOCKER_RUN+=(--user "$(user_id)")
DOCKER_RUN+=(--hostname "uICAL-devenv")
DOCKER_RUN+=($(mount_user))
DOCKER_RUN+=(--workdir "${PWD}")
DOCKER_RUN+=(--entrypoint /entrypoint.sh)


exec "${DOCKER_RUN[@]}" "${REPO_TAG}" "${@}"
