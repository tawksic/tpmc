#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h" // todo

struct Passphrase {
    char *user;
    char *password;
};

void usage() {
    printf("Usage: \n");
    printf("./a.out add ------- (Add a new password)\n");
    printf("./a.out print ----- (Print your passwords)\n");
}

int add_password(char user[], FILE *pw_file) {
    char *passphrase = NULL;
    size_t buffer_size = 50;

    printf("Type your password: ");
    system("stty -echo");
    getline(&passphrase, &buffer_size, stdin);
    system("stty echo\n");
    printf("\n");
    // passphrase[strcspn(passphrase, "\n")] = '\0';
    // will no longer need this when todo is accomplished

    struct Passphrase s = {.user = user, .password = passphrase };
    
    char buffer[strlen(s.user) + strlen(s.password) + 3];
    sprintf(buffer, "%s: %s", s.user, s.password);

    if (fputs(buffer, pw_file) == EOF) {
        perror("Failed to write to password file");
        free(passphrase);
        return 1;
    }

    free(passphrase);
}

int main(int argc, char *argv[]) {

    if (argv[1] == NULL) {
        usage();
        exit(1);
    };

    FILE *pw_file = fopen("passwords.json", "a");
    if (!pw_file) {
        perror("Opening the passwords.json failed.");
        return 1;
    }

    int cmd;

    if ( strcmp(argv[1], "add") == 0 ) {
        cmd = 1;
    };

    switch (cmd) {
        case 1: add_password(argv[2], pw_file); break;
        default: printf("Bad Argument\n"); break;
    };

    fclose(pw_file);
    return 0;
}

