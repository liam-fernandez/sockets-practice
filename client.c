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

    // struct for specifiying local namespace sockets
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

    /*
     * Extra byte at the end needed in the event that client sends a message
     * of size MAX_MSG_LEN, in which a newline needs to be appened (1 byte)
     * as well as the null char (1 byte)
     */
    char client_msg[MAX_MSG_LEN + 2];
    char server_rsp[MAX_MSG_LEN + 2];
    while ((fgets(client_msg, MAX_MSG_LEN + 1, stdin))) {
        if (strncmp(client_msg, QUIT_MSG, 4) == 0) {
            break;
        }

        /*
         * When client sends message of size MAX_MSG_LEN or greater, properly
         * terminate message and clear stdin buffer for next message
         */
        if (!strchr(client_msg, '\n')) {
            client_msg[MAX_MSG_LEN] = '\n';
            client_msg[MAX_MSG_LEN + 1] = '\0';
            char ch;
            while (((ch = getchar()) != EOF) && (ch != '\n')) {
            };
        }

        /*
         * Client message could be MAX_MSG_LEN + 1 bytes long, in the event
         * that client sent message of MAX_MSG_LEN, then we append newline.
         * Extra +1 is for null char
         */
        send(client_fd, client_msg, strnlen(client_msg, MAX_MSG_LEN + 1) + 1,
             0);
        read(client_fd, server_rsp, MAX_MSG_LEN + 2);
        printf("Server said: %s", server_rsp);
        printf("Enter a message to send (10 char limit): ");
    }

    printf("Closing connection");

    // closing the connected socket
    close(client_fd);
    return EXIT_SUCCESS;
}
