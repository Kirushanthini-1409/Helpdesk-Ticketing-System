// forgot.c – CGI “Forgot Password” Handler with Binary Search Tree (BST)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

// User node for BST
typedef struct User {
    char name[MAX_LEN];     // username
    char email[MAX_LEN];    // email
    char password[MAX_LEN]; // password
    char phone[20];         // phone number (unused here)
    char role[10];          // role (user/admin)
    struct User *left;      // left child
    struct User *right;     // right child
} User;

// Read raw POST data from stdin into buf (length len)
void get_post_data(char *buf, int len) {
    fread(buf, 1, len, stdin);
    buf[len] = '\0';
}

// Decode URL-encoded string src ? dest
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') {
            *dest++ = ' ';
        } else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = { src[1], src[2], '\0' };
            *dest++ = (char)strtol(hex, NULL, 16);
            src += 2;
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

// Find “field=value” in data, extract & decode into out[]
void extract_field(const char *data, const char *field, char *out) {
    char pattern[64];
    sprintf(pattern, "%s=", field);
    char *start = strstr(data, pattern);
    if (!start) {
        out[0] = '\0';
        return;
    }
    start += strlen(pattern);
    char *end = strchr(start, '&');
    int len = end ? (end - start) : strlen(start);
    char temp[256] = {0};
    strncpy(temp, start, len);
    url_decode(temp, out);
}

// BST insert by email
User* insert_user(User *root, User *new_user) {
    if (!root) return new_user;
    int cmp = strcmp(new_user->email, root->email);
    if (cmp < 0) {
        root->left = insert_user(root->left, new_user);
    } else if (cmp > 0) {
        root->right = insert_user(root->right, new_user);
    }
    return root;
}

// Load users.txt into a BST and return root pointer
User* load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return NULL;
    User *root = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        User *u = malloc(sizeof(User));
        if (!u) continue;
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", u->name, u->email, u->password, u->phone, u->role);
        u->left = u->right = NULL;
        root = insert_user(root, u);
    }

    fclose(fp);
    return root;
}

// Free the entire BST
void free_users(User *root) {
    if (!root) return;
    free_users(root->left);
    free_users(root->right);
    free(root);
}

// Search BST by email
User* find_user(User *root, const char *email) {
    if (!root) return NULL;
    int cmp = strcmp(email, root->email);
    if (cmp == 0) return root;
    else if (cmp < 0) return find_user(root->left, email);
    else return find_user(root->right, email);
}

void alert(const char *msg, const char *location) {
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s');window.location.href='%s';</script>", msg, location);
}

int main() {
    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;
    if (len <= 0) {
        alert("No data submitted", "/forgot.html");
        return 0;
    }

    char body[1024];
    get_post_data(body, len);

    char email[MAX_LEN];
    extract_field(body, "email", email);
    if (email[0] == '\0') {
        alert("Email field is empty", "/forgot.html");
        return 0;
    }

    User *users = load_users();
    User *found = find_user(users, email);

    if (found) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Your password is: %s", found->password);
        free_users(users);
        alert(msg, "/login.html");
        return 0;
    }

    free_users(users);
    alert("Email not found. Please register.", "/register.html");
    return 0;
}

