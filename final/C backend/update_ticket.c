#include <stdio.h>                      
#include <stdlib.h> 
#include <string.h>

// Define the Ticket structure
typedef struct Ticket {
    char id[30], email[100], problem[200], description[500];
    char status[20], productStatus[30], createdAt[50], pay[50];
    struct Ticket *next;
} Ticket;

// Function to read POST data from standard input (CGI input)
void get_post_data(char *buf, int len) {
    fread(buf, 1, len, stdin);         // Read 'len' bytes from stdin
    buf[len] = '\0';                   // Null-terminate the buffer
}

// Function to decode URL-encoded strings (e.g., %20 to space)
void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') *dest++ = ' ';            // Convert '+' to space
        else if (*src == '%' && src[1] && src[2]) { // If %XX format
            char hex[3] = {src[1], src[2], '\0'};   // Extract the hex digits
            *dest++ = (char)strtol(hex, NULL, 16);  // Convert hex to char
            src += 2;                               // Skip next two chars
        } else {
            *dest++ = *src;                         // Copy character
        }
        src++;                                      // Move to next character
    }
    *dest = '\0';                                   // Null-terminate result
}

// Extracts a specific key's value from URL-encoded form data
void extract_field(const char *data, const char *key, char *out) {
    char search[64], temp[512] = {0};               // Temp buffers
    sprintf(search, "%s=", key);                    // Create search string "key="
    char *start = strstr(data, search);             // Find position of key
    if (!start) { *out = 0; return; }               // If not found, set output to empty
    start += strlen(search);                        // Move pointer to start of value
    char *end = strchr(start, '&');                 // Find end of value
    int len = end ? end - start : strlen(start);    // Determine length of value
    strncpy(temp, start, len);                      // Copy value to temp
    url_decode(temp, out);                          // Decode value
}

// Load all tickets from file into a linked list
Ticket* load_tickets() {
    FILE *fp = fopen("tickets.txt", "r");           // Open ticket file for reading
    if (!fp) return NULL;                           // Return NULL if file not found
    Ticket *head = NULL, *tail = NULL;              // Initialize linked list
    char line[1024];                                // Buffer to hold each line

    while (fgets(line, sizeof(line), fp)) {         // Read line-by-line
        Ticket *tk = malloc(sizeof(Ticket));        // Allocate memory for a new ticket
        if (!tk) continue;                          // Skip if allocation fails

        // Parse each field separated by '|'
        char *token = strtok(line, "|");
        if (!token) { free(tk); continue; } strncpy(tk->id, token, sizeof(tk->id));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->email, token, sizeof(tk->email));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->problem, token, sizeof(tk->problem));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->description, token, sizeof(tk->description));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->status, token, sizeof(tk->status));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->productStatus, token, sizeof(tk->productStatus));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; } strncpy(tk->createdAt, token, sizeof(tk->createdAt));
        
        token = strtok(NULL, "\n");
        if (!token) { free(tk); continue; } strncpy(tk->pay, token, sizeof(tk->pay));
        
        tk->next = NULL;                            // Mark next pointer as NULL

        // Append to linked list
        if (!head) head = tail = tk;
        else { tail->next = tk; tail = tk; }
    }

    fclose(fp);                                     // Close file
    return head;                                    // Return head of linked list
}

// Save all tickets from the linked list back to the file
void save_tickets(Ticket *head) {
    FILE *fp = fopen("tickets.txt", "w");           // Open file for writing
    if (!fp) return;                                // Exit if failed to open

    while (head) {
        // Write all fields to file separated by '|'
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s\n",
                head->id, head->email, head->problem, head->description,
                head->status, head->productStatus, head->createdAt, head->pay);
        head = head->next;                          // Move to next ticket
    }

    fclose(fp);                                     // Close file
}

// Free the memory used by the ticket linked list
void free_tickets(Ticket *head) {
    Ticket *tmp;
    while (head) {
        tmp = head;                                 // Store current node
        head = head->next;                          // Move to next node
        free(tmp);                                  // Free current node
    }
}

// Show a browser alert and redirect to a page
void alert(const char *msg, const char *loc) {
    printf("Content-Type: text/html\n\n");          // Set content type
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, loc);
}

int main() {
    char email[100], role[10];
    FILE *sf = fopen("session.txt", "r");           // Open session file to get logged-in user

    if (!sf) {                                      // If session file missing
        alert("Login required", "/login.html");     // Alert and redirect to login
        return 0;
    }

    fscanf(sf, "%[^,],%s", email, role);            // Read email and role from session file
    fclose(sf);                                     // Close session file

    if (strcmp(role, "admin") != 0) {               // Only admin can access
        alert("Access denied", "/cgi-bin/home.exe");// Alert and redirect
        return 0;
    }

    char *len_str = getenv("CONTENT_LENGTH");       // Get length of POST data
    int len = len_str ? atoi(len_str) : 0;          // Convert to integer
    if (len <= 0) {                                 // If no data received
        alert("No POST data received", "/cgi-bin/view_ticket.exe");
        return 0;
    }

    // Variables to hold extracted form data
    char body[2048], id[30], problem[200], description[500], status[20], productStatus[30], pay[50];
    get_post_data(body, len);                       // Read POST data

    // Extract form fields from POST body
    extract_field(body, "ticket_id", id);
    extract_field(body, "problem", problem);
    extract_field(body, "description", description);
    extract_field(body, "status", status);
    extract_field(body, "productStatus", productStatus);
    extract_field(body, "pay", pay);

    Ticket *tickets = load_tickets();               // Load tickets from file
    Ticket *cur = tickets;                          // Pointer to traverse tickets
    int found = 0;                                   // Flag to check if ticket was found

    while (cur) {                                   // Traverse ticket list
        if (strcmp(cur->id, id) == 0) {             // Match ticket ID             // Update fields
            strncpy(cur->status, status, sizeof(cur->status));
            strncpy(cur->productStatus, productStatus, sizeof(cur->productStatus));
            strncpy(cur->pay, pay, sizeof(cur->pay));
            found = 1;                              // Mark as found
            break;
        }
        cur = cur->next;                            // Move to next ticket
    }

    save_tickets(tickets);                          // Save updated ticket list
    free_tickets(tickets);                          // Free memory

    // Show success or failure message
    if (found)
        alert("Ticket updated successfully", "/cgi-bin/view_ticket.exe");
    else
        alert("Ticket ID not found", "/cgi-bin/view_ticket.exe");

    return 0;
}

