#!/bin/bash
REPO_TAG=sourcesimian/uical/devenv

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

unset TTY; [ -t 0 ] && TTY="-t"

exec docker run -i ${TTY} --rm $(user_env) $(host_env) $(proxy_env) \
    --user $(user_id) \
    --hostname "uICAL-de" \
    $(mount_user) \
    --workdir ${PWD} \
    --entrypoint /entrypoint.sh \
    ${REPO_TAG} \
    "${@}"
