// login.c

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <ctype.h>      

#define MAX_LEN 100     // Maximum length for name, email, password, etc.

// Structure definition for a User
typedef struct User {
    char name[MAX_LEN];       // User's name
    char email[MAX_LEN];      // User's email
    char password[MAX_LEN];   // User's password
    char phone[20];           // User's phone number
    char role[10];            // User's role: "user" or "admin"
    struct User *next;        // Pointer to the next user in the list
} User;

// Reads POST data sent via CGI into buffer
void get_post_data(char *buf, int len) {
    fread(buf, 1, len, stdin);    // Read len bytes from stdin
    buf[len] = '\0';              // Null-terminate the string
}

// Decodes URL-encoded form input (e.g., '+' -> ' ', '%20' -> ' ')
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') *dest++ = ' ';   // Convert + to space
        else if (*src == '%' && src[1] && src[2]) {  // Convert %XX to ASCII
            char hex[3] = {src[1], src[2], 0};
            *dest++ = (char)strtol(hex, NULL, 16);   // Convert hex to char
            src += 2;
        } else {
            *dest++ = *src;   // Copy regular character
        }
        src++;
    }
    *dest = '\0';  // Null-terminate destination string
}

// Extracts the value of a given field name from POST data
void extract_field(const char *data, const char *field, char *output) {
    char pattern[64];
    sprintf(pattern, "%s=", field);   // e.g., "username="
    char *start = strstr(data, pattern); // Find start of the field
    if (!start) { output[0] = 0; return; } // Field not found
    start += strlen(pattern);             // Move to value start
    char *end = strchr(start, '&');       // Find end of value
    int len = end ? end - start : strlen(start); // Calculate length
    char temp[256] = {0};
    strncpy(temp, start, len);        // Copy value substring
    url_decode(temp, output);         // Decode URL encoding
}

// Load all user records from "users.txt" file into a linked list
User* load_users() {
    FILE *fp = fopen("users.txt", "r"); // Open user file for reading
    if (!fp) return NULL;               // Return NULL if file fails
    User *head = NULL, *tail = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {  // Read each line
        User *u = malloc(sizeof(User));      // Allocate new user
        if (!u) continue;                    // Skip if allocation fails
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", u->name, u->email, u->password, u->phone, u->role);  // Parse user fields
        u->next = NULL;                      // Set next to NULL
        if (!head) head = tail = u;          // First user
        else { tail->next = u; tail = u; }   // Append to list
    }

    fclose(fp);  // Close file
    return head; // Return head of user list
}

// Frees the entire linked list of users from memory
void free_users(User *head) {
    User *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void alert(const char *msg, const char *redirect) {
    printf("Content-Type: text/html\n\n");                      // Output HTTP header
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, redirect);  // Show alert and redirect
}

int main() {
    char *clen = getenv("CONTENT_LENGTH");     // Get the length of POST data
    int len = clen ? atoi(clen) : 0;           // Convert to int
    if (len <= 0) {                            // No data received
        alert("No data received", "/login.html");
        return 0;
    }

    char body[1024];
    get_post_data(body, len);                  // Read POST body

    char role[10], username[MAX_LEN], password[MAX_LEN];
    extract_field(body, "role", role);         // Extract role field
    extract_field(body, "username", username); // Extract username/email field
    extract_field(body, "password", password); // Extract password field

    // Hardcoded admin
    if (!strcmp(role, "admin") &&
        (!strcmp(username, "kishor") || !strcmp(username, "kishor@gmail.com")) &&
        !strcmp(password, "kishor9361")) {

        FILE *sf = fopen("session.txt", "w");  // Open session file for writing
        if (sf) {
            fprintf(sf, "kishor@gmail.com,admin\n"); // Save session data
            fclose(sf);
        }
        alert("Admin logged in...", "/cgi-bin/tic_home.exe");  
        return 0;
    }

    // Load users from file
    User *users = load_users();
    User *u = users;

    while (u) {
        if ((!strcmp(u->email, username) || !strcmp(u->name, username)) &&
            !strcmp(u->password, password)) {

            FILE *sf = fopen("session.txt", "w");
            if (sf) {
                fprintf(sf, "%s,user\n", u->email);
                fclose(sf);
            }
            free_users(users);
            alert("User logged in...", "/cgi-bin/home.exe");
            return 0;
        }
        u = u->next;
    }

    free_users(users); 
    alert("Invalid username/email or password", "/login.html");
    return 0;
}

