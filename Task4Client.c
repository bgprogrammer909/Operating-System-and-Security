/*
 * Task 4: Network Programming and IPC -- CLIENT (C version)
 * =============================================================
 *
 * Connects to server.c and speaks the same line-based protocol
 * (see protocol.md).
 *
 * Usage:
 *   ./client            interactive mode -- type commands yourself
 *   ./client --demo      scripted demo -- runs a fixed sequence of
 *                         commands automatically (good for reports)
 *
 * Compile:  gcc -Wall client.c -o client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5050
#define BUFFER_SIZE 1024

int read_line(int sock, char *out, int max_len) {
    int pos = 0;
    while (pos < max_len - 1) {
        char c;
        int n = recv(sock, &c, 1, 0);
        if (n <= 0) return n;
        if (c == '\n') break;
        if (c != '\r') out[pos++] = c;
    }
    out[pos] = '\0';
    return pos;
}

int connect_to_server(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
    printf("[CLIENT] Connected to 127.0.0.1:%d\n", PORT);
    return sock;
}

void send_command(int sock, const char *command_line) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s\n", command_line);
    send(sock, buffer, strlen(buffer), 0);

    char response[BUFFER_SIZE];
    read_line(sock, response, sizeof(response));
    printf("> %s\n< %s\n", command_line, response);
}

void run_demo(int sock) {
    printf("\n-- Trying a command before logging in (should be rejected) --\n");
    send_command(sock, "TIME");

    printf("\n-- Logging in with WRONG password (should fail) --\n");
    send_command(sock, "AUTH alice wrongpassword");

    printf("\n-- Logging in with correct credentials --\n");
    send_command(sock, "AUTH alice alice123");

    printf("\n-- Now that we're authenticated, try the other commands --\n");
    send_command(sock, "ECHO Hello from the client!");
    send_command(sock, "TIME");
    send_command(sock, "ADD 12 30");

    printf("\n-- Sending invalid data on purpose (should be validated and rejected) --\n");
    send_command(sock, "ADD twelve thirty");

    printf("\n-- Sending an unknown command --\n");
    send_command(sock, "DANCE");

    printf("\n-- Quitting cleanly --\n");
    send_command(sock, "QUIT");
}

void run_interactive(int sock) {
    printf("Type protocol commands (AUTH <user> <pass>, ECHO <text>, TIME, ADD <a> <b>, QUIT)\n");
    char line[BUFFER_SIZE];
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        char buffer[BUFFER_SIZE];
        snprintf(buffer, sizeof(buffer), "%s\n", line);
        send(sock, buffer, strlen(buffer), 0);

        char response[BUFFER_SIZE];
        read_line(sock, response, sizeof(response));
        printf("< %s\n", response);

        if (strncmp(line, "QUIT", 4) == 0) break;
    }
}

int main(int argc, char *argv[]) {
    int sock = connect_to_server();

    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        run_demo(sock);
    } else {
        run_interactive(sock);
    }

    close(sock);
    printf("[CLIENT] Connection closed\n");
    return 0;
}
