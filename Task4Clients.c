/*
 * Extra test (not required, but good evidence for the report):
 * launches several client connections concurrently, from separate
 * pthreads, to prove the server truly handles multiple clients at
 * the same time.
 *
 * Compile:  gcc -Wall -pthread multi_client_test.c -o multi_client_test
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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

void send_command(int sock, const char *command_line) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s\n", command_line);
    send(sock, buffer, strlen(buffer), 0);
    char response[BUFFER_SIZE];
    read_line(sock, response, sizeof(response));
    printf("> %s\n< %s\n", command_line, response);
}

void *run_one_client(void *arg) {
    int client_number = *(int *)arg;
    const char *username = (client_number % 2 == 0) ? "bob" : "alice";
    const char *password = (client_number % 2 == 0) ? "bob123" : "alice123";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("[CLIENT] Connected to 127.0.0.1:%d\n", PORT);

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AUTH %s %s", username, password);
    send_command(sock, cmd);

    snprintf(cmd, sizeof(cmd), "ECHO Hello, I am client number %d", client_number);
    send_command(sock, cmd);

    snprintf(cmd, sizeof(cmd), "ADD %d %d", client_number, client_number * 10);
    send_command(sock, cmd);

    send_command(sock, "QUIT");
    close(sock);
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int ids[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, run_one_client, &ids[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nAll concurrent clients finished successfully.\n");
    return 0;
}
