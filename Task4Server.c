/*
 * Task 4: Network Programming and IPC -- SERVER (C version)
 * =============================================================

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5050
#define BUFFER_SIZE 1024

typedef struct {
    const char *username;
    const char *password;
} Credential;

static const Credential VALID_USERS[] = {
    {"alice", "alice123"},
    {"bob", "bob123"},
};
#define NUM_USERS (int)(sizeof(VALID_USERS) / sizeof(VALID_USERS[0]))

int check_credentials(const char *username, const char *password) {
    for (int i = 0; i < NUM_USERS; i++) {
        if (strcmp(VALID_USERS[i].username, username) == 0 &&
            strcmp(VALID_USERS[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_number(const char *s) {
    if (*s == '\0') return 0;
    char *end;
    strtod(s, &end);
    while (isspace((unsigned char)*end)) end++;
    return *end == '\0';
}

void send_line(int sock, const char *message) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s\n", message);
    send(sock, buffer, strlen(buffer), 0);
}

/* Reads one line (up to '\n') from the socket into `out`. Returns
 * the number of bytes read, or 0 on clean disconnect, -1 on error. */
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

typedef struct {
    int client_sock;
    struct sockaddr_in address;
} ClientArgs;

void *handle_client(void *arg) {
    ClientArgs *client = (ClientArgs *)arg;
    int sock = client->client_sock;
    char client_id[64];
    snprintf(client_id, sizeof(client_id), "%s:%d",
             inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
    printf("[SERVER] New connection from %s\n", client_id);

    char authenticated_user[32] = "";
    char line[BUFFER_SIZE];

    while (1) {
        int n = read_line(sock, line, sizeof(line));
        if (n <= 0) break;  /* client disconnected */
        if (strlen(line) == 0) continue;

        /* split into command word + "rest of line" */
        char command[32];
        char *rest;
        char *space = strchr(line, ' ');
        if (space) {
            int cmd_len = (int)(space - line);
            if (cmd_len >= (int)sizeof(command)) cmd_len = sizeof(command) - 1;
            strncpy(command, line, cmd_len);
            command[cmd_len] = '\0';
            rest = space + 1;
        } else {
            strncpy(command, line, sizeof(command) - 1);
            command[sizeof(command) - 1] = '\0';
            rest = line + strlen(line);
        }
        for (char *p = command; *p; p++) *p = (char)toupper((unsigned char)*p);

        if (strcmp(command, "AUTH") == 0) {
            char username[64] = "", password[64] = "";
            char *sp = strchr(rest, ' ');
            if (sp) {
                int ulen = (int)(sp - rest);
                strncpy(username, rest, ulen);
                username[ulen] = '\0';
                strncpy(password, sp + 1, sizeof(password) - 1);
            }
            if (username[0] && password[0] && check_credentials(username, password)) {
                strncpy(authenticated_user, username, sizeof(authenticated_user) - 1);
                char msg[128];
                snprintf(msg, sizeof(msg), "OK AUTH Welcome %s", username);
                send_line(sock, msg);
                printf("[SERVER] %s authenticated as '%s'\n", client_id, username);
            } else {
                send_line(sock, "ERROR AUTH Invalid credentials");
                printf("[SERVER] %s failed authentication\n", client_id);
            }
            continue;
        }

        if (authenticated_user[0] == '\0') {
            send_line(sock, "ERROR You must AUTH before using other commands");
            continue;
        }

        if (strcmp(command, "ECHO") == 0) {
            char msg[BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "OK ECHO %s", rest);
            send_line(sock, msg);

        } else if (strcmp(command, "TIME") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char timestr[32];
            strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);
            char msg[64];
            snprintf(msg, sizeof(msg), "OK TIME %s", timestr);
            send_line(sock, msg);

        } else if (strcmp(command, "ADD") == 0) {
            char num1[32] = "", num2[32] = "";
            int matched = sscanf(rest, "%31s %31s", num1, num2);
            if (matched != 2 || !is_number(num1) || !is_number(num2)) {
                send_line(sock, "ERROR ADD Invalid numbers");
            } else {
                double result = atof(num1) + atof(num2);
                char msg[64];
                snprintf(msg, sizeof(msg), "OK ADD %g", result);
                send_line(sock, msg);
            }

        } else if (strcmp(command, "QUIT") == 0) {
            send_line(sock, "OK QUIT Goodbye");
            break;

        } else {
            send_line(sock, "ERROR Unknown command");
        }
    }

    close(sock);
    printf("[SERVER] Connection with %s closed\n", client_id);
    free(client);
    return NULL;
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);  /* flush stdout after every line, useful for logs */

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(server_sock, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("[SERVER] Listening on 127.0.0.1:%d (Ctrl+C to stop)\n", PORT);

    while (1) {
        ClientArgs *client = malloc(sizeof(ClientArgs));
        socklen_t addr_len = sizeof(client->address);
        client->client_sock = accept(server_sock, (struct sockaddr *)&client->address, &addr_len);
        if (client->client_sock < 0) { free(client); continue; }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client);
        pthread_detach(tid);  /* clean up automatically when the thread finishes */
    }

    close(server_sock);
    return 0;
}
