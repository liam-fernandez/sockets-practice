#include "constants.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT 8080
int main(void) {
    // Create socket for local tcp connection
    int server_fd;
    if ((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        perror("server_fd setup failed");
        exit(EXIT_FAILURE);
    }

    // struct for specifying local namespace sockets. This socket's "address"
    struct sockaddr_un sockaddr_local;
    sockaddr_local.sun_family = AF_LOCAL;
    char *filename = "07-20-server";
    strncpy(sockaddr_local.sun_path, filename, strlen(filename));
    sockaddr_local.sun_path[sizeof(sockaddr_local.sun_path) - 1] = '\0';

    // Bind socket to port 8080
    if (bind(server_fd, (struct sockaddr *)&sockaddr_local,
             SUN_LEN(&sockaddr_local)) < 0) {
        perror("server_fd bind failed");
        exit(EXIT_FAILURE);
    }

    // Puts the server socket into passive mode
    if (listen(server_fd, 3) < 0) {
        perror("server_fd failed to listen");
        exit(EXIT_FAILURE);
    }

    // Extract first connection request. accept will BLOCK until a connection
    // is present!
    int client_fd;
    socklen_t sockaddr_len = sizeof(sockaddr_local);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&sockaddr_local,
                            &sockaddr_len)) < 0) {
        perror("server_fd failed to accept");
        exit(EXIT_FAILURE);
    }

    char client_buf[MAX_MSG_LEN + 1];
    read(client_fd, client_buf, MAX_MSG_LEN + 1);
    while (strncmp(client_buf, QUIT_MSG, 4) != 0) {
        printf("Client said: %s", client_buf);
        send(client_fd, client_buf, strnlen(client_buf, MAX_MSG_LEN) + 1, 0);
        read(client_fd, client_buf, MAX_MSG_LEN + 1);
    }

    printf("Client closed connection\n");

    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
