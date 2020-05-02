#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("Usage: %s <un> <uid> <gn> <gui> <home>\n", argv[0]);
        return(1);
    }
    char* gid = argv[4]; char* gn = argv[3]; char* uid = argv[2]; char* un = argv[1];
    char* home = argv[5];
    char* spec = \
        "grep -q '^%s' /etc/group || /usr/sbin/groupadd -g %s %s;" \
        "grep -q '^%s' /etc/passwd || /usr/sbin/useradd -u %s -g %s -d %s %s;";
    char cmd[255];
    sprintf(cmd, spec, gn, gid, gn, un, uid, gid, home, un, un);
    setuid(0);
    return(execle("/bin/bash", "bash", "-x", "-c", cmd, (char*)NULL, (char*)NULL));
}
