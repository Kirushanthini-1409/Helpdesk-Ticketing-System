// register.c 

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <ctype.h>      

#define MAX_LEN 100     // Maximum buffer length for fields

// Structure to hold user details in a linked list
typedef struct User {
    char name[MAX_LEN];
    char email[MAX_LEN];
    char password[MAX_LEN];
    char phone[20];
    char role[10];
    struct User* next;
} User;

// Function to read POST data from standard input
void get_post_data(char *buf, int len) {
    fread(buf, 1, len, stdin);     // Read len bytes from stdin (POST body)
    buf[len] = '\0';               // Null-terminate the buffer
}

// Decode URL-encoded data (e.g., %20 => space)
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') *dest++ = ' ';   // Convert + to space
        else if (*src == '%' && src[1] && src[2]) {  // Decode %xx
            char hex[3] = {src[1], src[2], 0};
            *dest++ = (char)strtol(hex, NULL, 16);
            src += 2;
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';  // Null-terminate destination
}

// Extract a specific field from POST data string
void extract_field(const char *data, const char *field, char *output) {
    char pattern[64];
    sprintf(pattern, "%s=", field);             // Create search pattern like "email="
    char *start = strstr(data, pattern);        // Find start of field
    if (!start) { output[0] = 0; return; }      // Field not found
    start += strlen(pattern);                   // Move past '='
    char *end = strchr(start, '&');             // Find '&' indicating end of field
    int len = end ? end - start : strlen(start);
    char temp[256] = {0};
    strncpy(temp, start, len);                  // Copy raw field data
    url_decode(temp, output);                   // Decode URL encoding
}

// Load all users from users.txt into a linked list
User* load_users() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return NULL;   // File not found or empty
    User *head = NULL, *tail = NULL;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        User *u = malloc(sizeof(User));    // Allocate memory for new user
        if (!u) continue;                  // Skip if allocation fails
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", u->name, u->email, u->password, u->phone, u->role);
        u->next = NULL;
        if (!head) head = tail = u;        // First node
        else { tail->next = u; tail = u; } // Append to list
    }
    fclose(fp);
    return head;
}

// Free all memory used by the user linked list
void free_users(User *head) {
    User *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

// Check if phone is 10 digits and does not start with 0
int is_valid_phone(const char *phone) {
    if (strlen(phone) != 10 || phone[0] == '0') return 0;
    int i;
    for (i = 0; i < 10; i++)
        if (!isdigit(phone[i])) return 0;
    return 1;
}

// Check if username or email already exists in the list
int exists(User *head, const char *name, const char *email) {
    for (; head; head = head->next)
        if (!strcmp(head->name, name) || !strcmp(head->email, email))
            return 1;   // Found match
    return 0;           // Not found
}

// Validate password for strength requirements
int is_strong_password(const char *password) {
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    if (strlen(password) < 8) return 0;         // Too short
    int i;
	for (i = 0; password[i]; i++) {
        if (isupper(password[i])) has_upper = 1;
        else if (islower(password[i])) has_lower = 1;
        else if (isdigit(password[i])) has_digit = 1;
        else has_special = 1;                   // Any other character is special
    }
    return has_upper && has_lower && has_digit && has_special;
}

void alert(const char *msg, const char *redirect) {
    printf("Content-Type: text/html\n\n");      // HTML content type
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, redirect);
}

// Save the full linked list 
void save_users(User *head) {
    FILE *fp = fopen("users.txt", "w");
    if (!fp) return;
    User *u;
    for (u = head; u; u = u->next)
        fprintf(fp, "%s,%s,%s,%s,%s\n", u->name, u->email, u->password, u->phone, u->role);
    fclose(fp);
}

int main() {
    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;
    if (len <= 0) {
        alert("No data received", "/register.html");
        return 0;
    }

    char body[1024];                      // To hold POST data
    get_post_data(body, len);            // Read POST data

    // Extract individual fields
    char name[MAX_LEN], email[MAX_LEN], pass[MAX_LEN], confirm[MAX_LEN], phone[20];
    extract_field(body, "username", name);
    extract_field(body, "email", email);
    extract_field(body, "password", pass);
    extract_field(body, "confirm-password", confirm);
    extract_field(body, "phone", phone);

    // Validation checks
    if (strcmp(pass, confirm)) {
        alert("Passwords do not match", "/register.html");
        return 0;
    }

    if (!is_strong_password(pass)) {
        alert("Password must be 8+ chars, include upper, lower, digit, and special", "/register.html");
        return 0;
    }

    if (!is_valid_phone(phone)) {
        alert("Invalid phone number. Must be 10 digits and not start with 0.", "/register.html");
        return 0;
    }

    // Load existing users into list
    User *users = load_users();
    if (exists(users, name, email)) {
        free_users(users);
        alert("Username or Email already exists", "/register.html");
        return 0;
    }

    // Allocate and populate new user node
    User *new_user = malloc(sizeof(User));
    if (!new_user) {
        free_users(users);
        alert("Memory allocation failed", "/register.html");
        return 0;
    }
    strcpy(new_user->name, name);
    strcpy(new_user->email, email);
    strcpy(new_user->password, pass);        // Password saved as plain text
    strcpy(new_user->phone, phone);
    strcpy(new_user->role, "user");          // Default role is 'user'
    new_user->next = NULL;

    // Append new user to the linked list
    if (!users) {
        users = new_user;
    } else {
        User *tail = users;
        while (tail->next) tail = tail->next;
        tail->next = new_user;
    }

    // Save entire list back to file
    save_users(users);
    free_users(users);                       // Clean up memory

    alert("Registered successfully! Please log in.", "/login.html");
    return 0;
}

