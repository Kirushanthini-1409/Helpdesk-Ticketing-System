// view_users.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100

// User node for linked list
typedef struct User {
    char name[MAX_LEN];     // username
    char email[MAX_LEN];    // email address
    char password[MAX_LEN]; // password (displayed here for admin)
    char phone[20];         // phone number
    char role[10];          // role (user/admin)
    struct User *next;      // next pointer
} User;

// Read users.txt into a linked list and return head pointer
User* load_users() {
    FILE *fp = fopen("users.txt", "r");         // open user database
    if (!fp) return NULL;                       // if fail, return NULL
    User *head = NULL, *tail = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {     // read each line
        User *u = malloc(sizeof(User));         // allocate node
        if (!u) continue;                       // skip on malloc fail
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s",
               u->name, u->email, u->password, u->phone, u->role);
        u->next = NULL;                         
        if (!head) head = tail = u;            
        else { tail->next = u; tail = u; }     
    }

    fclose(fp);                                  // close file
    return head;                                 // return list head
}

// Free all nodes in the linked list
void free_users(User *head) {
    User *tmp;
    while (head) {
        tmp = head; 
        head = head->next;
        free(tmp);                              // free each node
    }
}

// Decode URL-encoded text (e.g. '+' ? ' ', '%20' ? space, etc.)
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') {
            *dest++ = ' ';                      // '+' ? space
        }
        else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = { src[1], src[2], '\0' };
            *dest++ = (char)strtol(hex, NULL, 16); // %xx ? char
            src += 2;                           // skip hex digits
        }
        else {
            *dest++ = *src;                     // copy normal char
        }
        src++;
    }
    *dest = '\0';                               // null-terminate
}

// Extract a field’s value (field=value) from a query string
void extract_field(const char *data, const char *field, char *out) {
    char pattern[64];
    sprintf(pattern, "%s=", field);             // build "field="
    char *start = strstr(data, pattern);        // locate field
    if (!start) { out[0] = '\0'; return; }      // not found ? empty
    start += strlen(pattern);                   // skip "field="
    char *end = strchr(start, '&');             // find end or '&'
    int len = end ? (end - start) : strlen(start);
    char temp[256] = {0};
    strncpy(temp, start, len);                  // copy raw value
    url_decode(temp, out);                      // decode into out
}

// Case-insensitive substring check: returns 1 if `substr` in `str`
int str_contains_ci(const char *str, const char *substr) {
    if (!substr[0]) return 1;                   // empty substr matches
    char s_low[256], sub_low[256];
    int i;
    // lowercase `str`
    for (i = 0; str[i] && i < 255; i++) s_low[i] = tolower((unsigned char)str[i]);
    s_low[i] = '\0';
    // lowercase `substr`
    for (i = 0; substr[i] && i < 255; i++) sub_low[i] = tolower((unsigned char)substr[i]);
    sub_low[i] = '\0';
    return strstr(s_low, sub_low) != NULL;      // check substring
}

void alert_redirect(const char *msg, const char *loc) {
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, loc);
}

int main() {
    char email_sess[MAX_LEN], role_sess[10];
    FILE *sf = fopen("session.txt", "r");
    if (!sf) {
        alert_redirect("Login required", "/login.html");
        return 0;
    }
    fscanf(sf, "%[^,],%s", email_sess, role_sess);
    fclose(sf);
    if (strcmp(role_sess, "admin") != 0) {
        alert_redirect("Access denied", "/cgi-bin/home.exe");
        return 0;
    }

    char raw_q[100] = {0}, search[100] = {0};
    char *query = getenv("QUERY_STRING");        // e.g. "search=alice"
    if (query) extract_field(query, "search", raw_q);
    url_decode(raw_q, search);                   // decode percent-encoding

    // --- Load all users ---
    User *users = load_users();

    // --- Begin HTML output ---
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang='en'><head>"
           "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>Admin - View Users</title>"
           "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>"
           "</head><body>");

    // --- Navbar ---
    printf("<nav class='navbar navbar-expand-lg navbar-dark bg-dark px-3 py-2'>"
           "<a class='navbar-brand' href='/cgi-bin/tic_home.exe'><h3>HI-5 SOLUTIONS</h3></a>"
           "<div class='collapse navbar-collapse'><ul class='navbar-nav ms-auto'>"
           "<li class='nav-item'><a class='nav-link' href='/tic_home.html'>Dashboard</a></li>"
           "<li class='nav-item'><a class='nav-link' href='/contact.html'>Contact</a></li>"
           "<li class='nav-item'><a class='nav-link' href='/cgi-bin/logout.exe'>Logout</a></li>"
           "</ul></div></nav>");

    // --- Container & Search Form ---
    printf("<div class='container mt-4'>"
           "<h2>Registered Users</h2>"
           "<form method='get' action='/cgi-bin/view_users.exe' class='mb-3'>"
           "<div class='input-group'>"
           "<input type='text' name='search' class='form-control' placeholder='Search by name or email' value='%s'/>"
           "<button class='btn btn-primary' type='submit'>Search</button>"
           "</div></form>", search);

    // --- Users Table Header ---
    printf("<div class='table-responsive'><table class='table table-striped'>"
           "<thead><tr><th>Name</th><th>Email</th><th>Phone</th><th>Role</th></tr></thead><tbody>");

    // --- Table Rows (filtered if search provided) ---
    User *cur = users;
    while (cur) {
        // show row if no search or search matches name/email
        if (str_contains_ci(cur->name, search) ||
            str_contains_ci(cur->email, search)) {
            printf("<tr>"
                   "<td>%s</td>"
                   "<td>%s</td>"
                   "<td>%s</td>"
                   "<td>%s</td>"
                   "</tr>",
                   cur->name, cur->email, cur->phone, cur->role);
        }
        cur = cur->next;
    }

    // --- Close table & HTML ---
    printf("</tbody></table></div></div><footer class='bg-dark text-white text-center py-3 mt-5 fixed-bottom'>&copy; 2025 HI-5 SOLUTIONS | Laptop & Mobile Sales and Repairs</footer>"
           "</body></html>");

    // --- Cleanup memory ---
    free_users(users);

    return 0;
}

