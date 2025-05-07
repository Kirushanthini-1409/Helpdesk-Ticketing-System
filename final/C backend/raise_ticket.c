#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 100
#define MAX_PROB 200
#define MAX_DESC 500

// Read POST data from stdin
void get_post_data(char *buf, int len) {
    fread(buf, 1, len, stdin);
    buf[len] = '\0';
}

// Decode URL-encoded strings
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') *dest++ = ' ';
        else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = {src[1], src[2], '\0'};
            *dest++ = (char) strtol(hex, NULL, 16);
            src += 2;
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

// Extract a field's value from POST data
void extract_field(const char *data, const char *field, char *out) {
    char pattern[64];
    sprintf(pattern, "%s=", field);
    char *start = strstr(data, pattern);
    if (!start) {
        *out = '\0';
        return;
    }
    start += strlen(pattern);
    char *end = strchr(start, '&');
    int len = end ? end - start : strlen(start);
    char temp[1024] = {0};
    strncpy(temp, start, len);
    url_decode(temp, out);
}

// Alert and redirect
void alert(const char *msg, const char *redirect) {
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, redirect);
}

// Determine the next ticket number based on the highest "TCKT-%d" value in tickets.txt
int get_next_ticket_number() {
    FILE *fp = fopen("tickets.txt", "r");
    if (!fp) return 1;  // If file doesn't exist, start from ticket 1

    int max_id = 0;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, "|");  // Get the ticket ID (first field)
        if (token && strncmp(token, "TCKT-", 5) == 0) {
            int id = atoi(token + 5);  // Extract numeric part after "TCKT-"
            if (id > max_id) {
                max_id = id;
            }
        }
    }

    fclose(fp);
    return max_id + 1;  // Return the next available ticket number
}

// Main CGI handler
int main() {
    // 1. Validate session
    char email[MAX_LEN], role[10];
    FILE *session = fopen("session.txt", "r");
    if (!session) {
        alert("Login required", "/login.html");
        return 0;
    }
    fscanf(session, "%[^,],%s", email, role);
    fclose(session);

    if (strcmp(role, "user") != 0) {
        alert("Only users can raise tickets", "/cgi-bin/tic_home.exe");
        return 0;
    }

    // 2. Read POST data
    char *len_str = getenv("CONTENT_LENGTH");
    int len = len_str ? atoi(len_str) : 0;
    if (len <= 0) {
        alert("No data received", "/raise_ticket.html");
        return 0;
    }

    char post_data[2048];
    get_post_data(post_data, len);

    // 3. Extract form fields
    char problem[MAX_PROB], description[MAX_DESC], product[MAX_LEN], other_reason[MAX_PROB];
    extract_field(post_data, "problem", problem);
    extract_field(post_data, "description", description);
    extract_field(post_data, "product", product);
    extract_field(post_data, "other_reason", other_reason);

    if (strcmp(problem, "Others") == 0 && strlen(other_reason) > 0) {
        strncpy(problem, other_reason, MAX_PROB - 1);
    }

    // 4. Generate ticket ID and timestamp
    int next_ticket_num = get_next_ticket_number();
    char ticket_id[20];
    sprintf(ticket_id, "TCKT-%d", next_ticket_num);

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);

    // 5. Set default statuses
    const char *ticket_status = "Open";
    const char *product_status = "Need to Surrender";
    const char *pay="Not Paid";

    // 6. Save to file in structured format (corrected to include description)
    FILE *fp = fopen("tickets.txt", "a");
    if (!fp) {
        alert("Error saving ticket", "/raise_ticket.html");
        return 0;
    }

    // Format: ID|Email|Skip|Problem|Description|TicketStatus|ProductStatus|Timestamp|payment
    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s\n",
            ticket_id, email, problem, description, ticket_status, product_status, timestamp,pay);
    fclose(fp);

    // 7. Confirmation
    alert("Ticket raised successfully!", "/cgi-bin/view_ticket.exe");
    return 0;
}

