#include "constants.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(void) {
    // create socket for local tcp connection
    int client_fd;
    if ((client_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        perror("client_fd setup failed");
        exit(EXIT_FAILURE);
    }

    // struct for specifiying local namespace sockets hehe
    struct sockaddr_un server_adr;
    server_adr.sun_family = AF_LOCAL;
    char *filename = "07-20-server";
    strncpy(server_adr.sun_path, filename, strlen(filename));
    server_adr.sun_path[sizeof(server_adr.sun_path) - 1] = '\0';

    // attempt connection to server
    if (connect(client_fd, (struct sockaddr *)&server_adr, sizeof(server_adr)) <
        0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connection succesful!\n");
    printf("Enter a message to send (10 char limit): ");
    char client_msg[MAX_MSG_LEN + 1]; // untrusted user input
    char server_rsp[MAX_MSG_LEN + 1];
    while ((fgets(client_msg, MAX_MSG_LEN + 1, stdin))) {
        if (strncmp(client_msg, QUIT_MSG, 4) == 0) {
            break;
        }
        send(client_fd, client_msg, strnlen(client_msg, MAX_MSG_LEN) + 1, 0);
        read(client_fd, server_rsp, MAX_MSG_LEN + 1);
        printf("Server said: %s", server_rsp);
        printf("Enter a message to send (10 char limit): ");
    }

    printf("Closing connection");

    // closing the connected socket
    close(client_fd);
    return EXIT_SUCCESS;
}
