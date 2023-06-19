#include "test.h"

#define SERVER_IP "127.0.0.1"

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void term(int signum) {
    printf("\nServer shutting down...\n");
    exit(0);
}

// Make a Minecraft server packet listener for clients to connect to
int main(int argc, char *argv[]) {
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = term;
    sigaction(SIGINT, &action, NULL);

    struct addrinfo hints, *info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    int status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &info);
    CHECK_ERROR(status);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_ERROR(socket_fd);
    char buffer[MAXLINE];

    for (struct addrinfo *p = info; p != NULL; p = p->ai_next) {
        int bind_status = bind(socket_fd, p->ai_addr, p->ai_addrlen);
        if (bind_status == -1) {
            perror("bind");
            continue;
        }
        break;
    }

    int listen_status = listen(socket_fd, SERVER_BACKLOG);
    CHECK_ERROR(listen_status);

    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    int sigaction_status = sigaction(SIGCHLD, &sa, NULL);
    CHECK_ERROR(sigaction_status);

    printf("Listening on %s:%s\n", SERVER_IP, SERVER_PORT);

    struct sockaddr_storage client_addr;
    int client_fd;
    for (;;) {
        socklen_t socket_len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &socket_len);
        CHECK_ERROR(client_fd);
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), buffer, sizeof(buffer));
        printf("Got connection from %s\n", buffer);

        if (!fork()) { // this is the child process
            close(socket_fd); // child doesn't need the listener
            int numbytes = recv(client_fd, buffer, MAXLINE - 1, 0);
            CHECK_ERROR(numbytes);
            printf("Packet Length: %d\n", numbytes);
            printf("Packet: ");
            for (int i = 0; i < numbytes; i++) {
                printf("%02x ", buffer[i]);
            }
            printf("\n");
            char *response = "Hello, world!";
            int send_status = send(client_fd, response, strlen(response), 0);
            CHECK_ERROR(send_status);
            close(client_fd);
            exit(0);
        }
        close(client_fd); // parent doesn't need this
    }

    close(socket_fd);
    freeaddrinfo(info);
    return 0;
}