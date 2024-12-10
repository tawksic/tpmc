#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>

#define PW_FILE "passwords.txt"

void usage() {
    printf("Usage: \n");
    printf("./a.out add  --------- (Add a new password)\n");
    printf("./a.out print -------- (Print your passwords)\n");
}

void free_stuff(void *allocs, ...) {
    va_list args;
    va_start(args, allocs);

    void *ptr = allocs;

    while (ptr != NULL) {
        free(ptr);
        ptr = va_arg(args, void *);
    }

    va_end(args);
}

int _check_existing_service(char *service_user, const char *filename) {
    char buff[256];
    FILE *pw_file = fopen(filename, "r");
    if (!pw_file) {
        perror("Failed to open file for reading");
        return 0;
    }

    while (fgets(buff, sizeof(buff), pw_file) != NULL) {
        char *first_colon = strchr(buff, ':');
        if (!first_colon) continue;

        char *second_colon = strchr(first_colon + 1, ':');
        if (!second_colon) continue;

        size_t length = second_colon - buff;
        char *service_in_file = malloc(length + 1);
        if (service_in_file == NULL) {
            perror("Memory allocation failed");
            fclose(pw_file);
            return 0;
        }

        strncpy(service_in_file, buff, length);
        service_in_file[length] = '\0';

        if (strcmp(service_user, service_in_file) == 0) {
            printf("Service:User '%s' already exists.\n", service_user);
            free(service_in_file);
            fclose(pw_file);
            return 1;
        }
        free(service_in_file);
    }

    fclose(pw_file);
    return 0;
}

int add_password(const char *filename) {
    char *service = NULL;
    char *username = NULL;
    char *password = NULL;
    size_t buffer_size = 50;

    printf("Type the service name: ");
    getline(&service, &buffer_size, stdin);
    service[strcspn(service, "\n")] = '\0';

    printf("Type your username: ");
    getline(&username, &buffer_size, stdin);
    username[strcspn(username, "\n")] = '\0';

    size_t total_len = strlen(service) + strlen(username) + 2;
    char *result = malloc(total_len);
    if (result == NULL) {
        perror("Memory allocation failed");
        free_stuff(service, username, NULL);
        return 1;
    }

    snprintf(result, total_len, "%s:%s", service, username);

    if (_check_existing_service(result, filename)) {
        free_stuff(service, username, result, NULL);
        return 1;
    }
    free(result);

    printf("Type your password: ");
    system("stty -echo");
    getline(&password, &buffer_size, stdin);
    system("stty echo\n");
    printf("\n");

    password[strcspn(password, "\n")] = '\0';
    if (strchr(password, ':')) {
        printf("Invalid characters in password, don't use colons\n");
        free_stuff(service, username, password, NULL);
        return 1;
    }

    FILE *pw_file = fopen(filename, "a");
    if (!pw_file) {
        perror("Failed to open file for appending");
        free_stuff(service, username, password, NULL);
        return 1;
    }

    fprintf(pw_file, "%s:%s:%s\n", service, username, password);

    fclose(pw_file);
    free_stuff(service, username, password, NULL);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    char command[256];
    const char *pw_file = PW_FILE;
    sprintf(command, "ls %s > /dev/null 2>&1", PW_FILE);

    if (system(command) != 0 ) {
        sprintf(command, "touch %s > /dev/null 2>&1", PW_FILE);
        system(command);
    }
    
    int cmd = 0;

    if (strcmp(argv[1], "add") == 0) {
        cmd = 1;
    }

    switch (cmd) {
        case 1:
            add_password(pw_file);
            break;
        default:
            printf("Bad Argument\n");
            usage();
            break;
    }

    return 0;
}
