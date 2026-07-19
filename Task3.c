/*
 * Task 3: File System Operations and Security (C version)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define VAULT_DIR      "vault"
#define USERS_FILE     "vault/users.db"
#define AUDIT_FILE     "vault/audit.log"
#define MAX_USERS      32
#define MAX_FILES      32
#define MAX_LINE       1024

/* ---------------- simple salted hash (FNV-1a based, for demo use) ---------------- */

unsigned long simple_hash(const char *text) {
    unsigned long hash = 2166136261UL;    /* FNV offset basis */
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        hash ^= *p;
        hash *= 16777619UL;               /* FNV prime */
    }
    return hash;
}

/* Runs the hash several rounds with a per-user salt mixed in, which
 * is deliberately slower than a single hash pass -- a very small,
 * from-scratch stand-in for the "slow key-derivation function" idea
 * behind real algorithms like PBKDF2/bcrypt. */
unsigned long hash_password(const char *password, unsigned long salt) {
    char buffer[MAX_LINE];
    unsigned long h = salt;
    for (int round = 0; round < 1000; round++) {
        snprintf(buffer, sizeof(buffer), "%lu:%s:%d", h, password, round);
        h = simple_hash(buffer);
    }
    return h;
}

/* ---------------- tiny keyed stream cipher (XOR + PRNG) ---------------- */

void stream_cipher(char *data, int length, unsigned long key) {
    unsigned long state = key;
    for (int i = 0; i < length; i++) {
        /* simple xorshift PRNG keyed by the file's encryption key */
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        data[i] = (char)(data[i] ^ (state & 0xFF));
    }
}

/* ---------------- data model ---------------- */

typedef struct {
    char username[32];
    unsigned long salt;
    unsigned long password_hash;
    char group[16];
} User;

typedef struct {
    char filename[64];
    char owner[32];
    char group[16];
    char permissions[10];   /* e.g. "rw-r-----" */
    int sensitive;
    unsigned long enc_key;  /* only used if sensitive == 1 */
} FileMeta;

static User users[MAX_USERS];
static int user_count = 0;

static FileMeta files[MAX_FILES];
static int file_count = 0;

static char current_user[32] = "";

/* ---------------- audit log ---------------- */

void audit(const char *action, const char *filename, int allowed, const char *detail) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestr[32];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);

    const char *who = current_user[0] ? current_user : "anonymous";
    const char *status = allowed ? "ALLOWED" : "DENIED";

    FILE *f = fopen(AUDIT_FILE, "a");
    if (f) {
        fprintf(f, "%s | %s | %s | %s | %s | %s\n", timestr, who, action, filename, status, detail);
        fclose(f);
    }
    printf("[AUDIT] %s | %-10s | %-8s | %-20s | %s %s\n", timestr, who, action, filename, status, detail);
}

/* ---------------- user management ---------------- */

User *find_user(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) return &users[i];
    }
    return NULL;
}

int register_user(const char *username, const char *password, const char *group) {
    if (find_user(username) != NULL) {
        printf("User '%s' already exists.\n", username);
        return 0;
    }
    User *u = &users[user_count++];
    snprintf(u->username, sizeof(u->username), "%s", username);
    snprintf(u->group, sizeof(u->group), "%s", group);
    u->salt = (unsigned long)time(NULL) + (unsigned long)(rand() % 100000) + user_count * 7919UL;
    u->password_hash = hash_password(password, u->salt);
    printf("Registered new user '%s' (group: %s).\n", username, group);
    return 1;
}

int login(const char *username, const char *password) {
    User *u = find_user(username);
    if (u != NULL && hash_password(password, u->salt) == u->password_hash) {
        snprintf(current_user, sizeof(current_user), "%s", username);
        printf("Login successful. Welcome, %s.\n", username);
        return 1;
    }
    printf("Login failed: incorrect username or password.\n");
    return 0;
}

void logout(void) {
    printf("User '%s' logged out.\n", current_user);
    current_user[0] = '\0';
}

/* ---------------- permission checking ---------------- */

FileMeta *find_file(const char *filename) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].filename, filename) == 0) return &files[i];
    }
    return NULL;
}

/* action: 0 = read, 1 = write, 2 = execute */
int check_permission(FileMeta *meta, int action) {
    if (current_user[0] == '\0') return 0;

    const char *perm_chars;
    if (strcmp(current_user, meta->owner) == 0) {
        perm_chars = meta->permissions + 0;       /* owner bits    */
    } else {
        User *u = find_user(current_user);
        if (u != NULL && strcmp(u->group, meta->group) == 0) {
            perm_chars = meta->permissions + 3;    /* group bits    */
        } else {
            perm_chars = meta->permissions + 6;    /* others bits   */
        }
    }
    return perm_chars[action] != '-';
}

/* ---------------- file paths ---------------- */

void content_path(const char *filename, char *out, size_t out_size) {
    snprintf(out, out_size, "%s/%s.data", VAULT_DIR, filename);
}

/* ---------------- core file operations ---------------- */

int create_file(const char *filename, const char *content, const char *permissions, int sensitive) {
    if (current_user[0] == '\0') {
        printf("You must log in before creating files.\n");
        return 0;
    }
    if (find_file(filename) != NULL) {
        printf("File '%s' already exists.\n", filename);
        return 0;
    }

    User *u = find_user(current_user);
    FileMeta *meta = &files[file_count++];
    snprintf(meta->filename, sizeof(meta->filename), "%s", filename);
    snprintf(meta->owner, sizeof(meta->owner), "%s", current_user);
    snprintf(meta->group, sizeof(meta->group), "%s", u->group);
    snprintf(meta->permissions, sizeof(meta->permissions), "%s", permissions);
    meta->sensitive = sensitive;
    meta->enc_key = sensitive ? (unsigned long)time(NULL) * 2654435761UL + file_count : 0;

    char path[128];
    content_path(filename, path, sizeof(path));

    int len = (int)strlen(content);
    char *buffer = malloc(len + 1);
    memcpy(buffer, content, len);

    if (sensitive) {
        stream_cipher(buffer, len, meta->enc_key);
    }

    FILE *f = fopen(path, "wb");
    fwrite(buffer, 1, len, f);
    fclose(f);
    free(buffer);

    char detail[64];
    snprintf(detail, sizeof(detail), "(sensitive=%s)", sensitive ? "true" : "false");
    audit("CREATE", filename, 1, detail);
    return 1;
}

int read_file(const char *filename, char *out, size_t out_size) {
    FileMeta *meta = find_file(filename);
    if (meta == NULL) {
        printf("File '%s' does not exist.\n", filename);
        return 0;
    }
    if (!check_permission(meta, 0)) {
        audit("READ", filename, 0, "permission denied");
        printf("Permission denied: you cannot read this file.\n");
        return 0;
    }

    char path[128];
    content_path(filename, path, sizeof(path));
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    fread(out, 1, (size_t)len, f);
    fclose(f);
    out[len] = '\0';

    if (meta->sensitive) {
        stream_cipher(out, (int)len, meta->enc_key); /* XOR cipher decrypts with the same key */
    }

    audit("READ", filename, 1, "");
    return 1;
}

int write_file(const char *filename, const char *new_content) {
    FileMeta *meta = find_file(filename);
    if (meta == NULL) {
        printf("File '%s' does not exist.\n", filename);
        return 0;
    }
    if (!check_permission(meta, 1)) {
        audit("WRITE", filename, 0, "permission denied");
        printf("Permission denied: you cannot write to this file.\n");
        return 0;
    }

    char path[128];
    content_path(filename, path, sizeof(path));
    int len = (int)strlen(new_content);
    char *buffer = malloc(len + 1);
    memcpy(buffer, new_content, len);

    if (meta->sensitive) {
        stream_cipher(buffer, len, meta->enc_key);
    }

    FILE *f = fopen(path, "wb");
    fwrite(buffer, 1, len, f);
    fclose(f);
    free(buffer);

    audit("WRITE", filename, 1, "");
    return 1;
}

int delete_file(const char *filename) {
    FileMeta *meta = find_file(filename);
    if (meta == NULL) {
        printf("File '%s' does not exist.\n", filename);
        return 0;
    }
    if (!check_permission(meta, 1)) {  /* deleting requires write permission */
        audit("DELETE", filename, 0, "permission denied");
        printf("Permission denied: you cannot delete this file.\n");
        return 0;
    }

    char path[128];
    content_path(filename, path, sizeof(path));
    remove(path);

    /* remove meta entry by shifting the array down */
    int idx = (int)(meta - files);
    for (int i = idx; i < file_count - 1; i++) files[i] = files[i + 1];
    file_count--;

    audit("DELETE", filename, 1, "");
    return 1;
}

void print_raw_bytes(const char *filename) {
    char path[128];
    content_path(filename, path, sizeof(path));
    FILE *f = fopen(path, "rb");
    if (!f) return;
    printf("Raw bytes on disk (hex, should look scrambled, NOT readable text):\n");
    int c, count = 0;
    while ((c = fgetc(f)) != EOF && count < 64) {
        printf("%02x ", (unsigned char)c);
        count++;
    }
    printf("\n");
    fclose(f);
}

/* ---------------- demo walkthrough ---------------- */

int main(void) {
    srand((unsigned)time(NULL));
    mkdir(VAULT_DIR, 0755);
    remove(AUDIT_FILE); /* start each run with a clean audit log */

    char buffer[MAX_LINE];

    printf("=== Registering users ===\n");
    register_user("alice", "alicePassword123", "staff");
    register_user("bob", "bobPassword456", "staff");
    register_user("carol", "carolPassword789", "guests");

    printf("\n=== Alice logs in and creates files ===\n");
    login("alice", "alicePassword123");
    create_file("notes.txt", "Team meeting notes for Monday.", "rw-r-----", 0);
    create_file("passwords.txt", "db_password=Sup3rSecret!", "rw-------", 1);
    logout();

    printf("\n=== Bob (same group as Alice) logs in ===\n");
    login("bob", "bobPassword456");
    printf("Bob reads notes.txt (should be allowed, group has read access):\n");
    if (read_file("notes.txt", buffer, sizeof(buffer))) printf("%s\n", buffer);

    printf("\nBob tries to write notes.txt (should be denied, group has no write access):\n");
    write_file("notes.txt", "Bob was here");

    printf("\nBob tries to read the sensitive passwords.txt (should be denied):\n");
    read_file("passwords.txt", buffer, sizeof(buffer));
    logout();

    printf("\n=== Carol (different group) logs in ===\n");
    login("carol", "carolPassword789");
    printf("Carol tries to read notes.txt (should be denied, 'others' has no access):\n");
    read_file("notes.txt", buffer, sizeof(buffer));
    logout();

    printf("\n=== Alice logs back in to prove encryption works ===\n");
    login("alice", "alicePassword123");
    printf("Alice reads passwords.txt (decrypted automatically for the owner):\n");
    if (read_file("passwords.txt", buffer, sizeof(buffer))) printf("%s\n", buffer);
    print_raw_bytes("passwords.txt");
    delete_file("passwords.txt");
    logout();

    printf("\n=== Failed login attempt ===\n");
    login("alice", "wrongPassword");

    printf("\n=== DEMO COMPLETE (see vault/audit.log for the full audit trail) ===\n");
    return 0;
}
