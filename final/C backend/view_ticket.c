#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define maximum lengths for various fields
#define MAX_LEN 100
#define MAX_PROB 200
#define MAX_DESC 500
#define MAX_STATUS 20
#define MAX_PSTATUS 30
#define MAX_TIMESTAMP 50

// Structure for storing ticket details
typedef struct Ticket {
    char id[30];                         // Ticket ID
    char email[MAX_LEN];                // Email of the user
    char problem[MAX_PROB];             // Problem title
    char description[MAX_DESC];         // Description of problem
    char status[MAX_STATUS];            // Ticket status
    char productStatus[MAX_PSTATUS];    // Product handling status
    char createdAt[MAX_TIMESTAMP];      // Timestamp of ticket creation
    char pay[50];                       // Payment status
    struct Ticket *next;                // Pointer to next ticket
} Ticket;

// Convert timestamp string to time_t for comparison
time_t parse_timestamp(const char *timestamp) {
    struct tm tm = {0}; // Zero initialize time structure
    sscanf(timestamp, "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec); // Parse date and time
    tm.tm_year -= 1900; // Adjust year for struct tm
    tm.tm_mon -= 1;     // Adjust month for struct tm
    return mktime(&tm); // Convert to time_t
}

// Check if the ticket is older than 2 days
int is_overdue(const char *createdAt) {
    time_t created = parse_timestamp(createdAt); // Ticket creation time
    time_t now = time(NULL);                     // Current time
    return difftime(now, created) > 2 * 24 * 60 * 60; // If older than 2 days
}

// Load tickets from file depending on role/email and filter by status
Ticket* load_tickets(const char *email, const char *role, const char *filterStatus) {
    FILE *fp = fopen("tickets.txt", "r"); // Open ticket file
    if (!fp) return NULL; // Return NULL if file doesn't exist

    Ticket *head = NULL, *tail = NULL; // Initialize linked list
    char line[1024]; // Line buffer

    while (fgets(line, sizeof(line), fp)) {
        Ticket *tk = malloc(sizeof(Ticket)); // Allocate memory
        if (!tk) continue;

        line[strcspn(line, "\n")] = '\0'; // Remove newline character

        // Parse ticket fields separated by '|'
        char *token = strtok(line, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->id, token, sizeof(tk->id));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->email, token, sizeof(tk->email));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->problem, token, sizeof(tk->problem));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->description, token, sizeof(tk->description));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->status, token, sizeof(tk->status));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->productStatus, token, sizeof(tk->productStatus));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->createdAt, token, sizeof(tk->createdAt));

        token = strtok(NULL, "|");
        if (!token) { free(tk); continue; }
        strncpy(tk->pay, token, sizeof(tk->pay));

        tk->next = NULL;

        // Skip non-admin users' unrelated tickets
        if (strcmp(role, "admin") != 0 && strcmp(tk->email, email) != 0) {
            free(tk);
            continue;
        }

        // Apply status filter if not "All"
        if (strcmp(filterStatus, "All") != 0 && strcmp(tk->status, filterStatus) != 0) {
            free(tk);
            continue;
        }

        // Add to linked list
        if (!head) head = tail = tk;
        else {
            tail->next = tk;
            tail = tk;
        }
    }

    fclose(fp);
    return head;
}

// Free memory allocated to ticket list
void free_tickets(Ticket *head) {
    while (head) {
        Ticket *tmp = head;
        head = head->next;
        free(tmp);
    }
}

// Main CGI function
int main() {
    char email[MAX_LEN], role[10], query[200], statusFilter[30] = "All";

    // Get status from QUERY_STRING if any
    char *qs = getenv("QUERY_STRING");
    if (qs && sscanf(qs, "status=%29s", statusFilter)) {
        // Replace + with space for browser encoding
        int i ;
        for (i= 0; statusFilter[i]; i++)
            if (statusFilter[i] == '+') statusFilter[i] = ' ';
    }

    // Read session info (email and role)
    FILE *sf = fopen("session.txt", "r");
    if (!sf) {
        printf("Content-Type: text/html\n\n");
        printf("<script>alert('Please login first.'); window.location.href='/login.html';</script>");
        return 0;
    }
    fscanf(sf, "%[^,],%s", email, role);
    fclose(sf);

    // Load filtered tickets
    Ticket *tickets = load_tickets(email, role, statusFilter);

    // Output HTML content type
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang='en'><head>"
           "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>Tickets</title>"
           "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>"
           "</head><body>");

    // Navbar
    printf("<nav class='navbar navbar-expand-lg navbar-dark bg-dark px-2 py-3'>"
           "<div class='container-fluid'>"
           "<a class='navbar-brand' href='/home.exe'><h3>HI-5 SOLUTIONS</h3></a>"
           "<h1 class='text-white'>HELPDESK TICKETING SYSTEM</h1>"
           "<ul class='navbar-nav'>"
           "<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/tic_home.exe'>Home</a></li>"
           "<li class='nav-item'><a class='nav-link text-white' href='/contact.html'>Contact</a></li>"
           "<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/logout.exe'>Logout</a></li>"
           "</ul></div></nav>");

    // Status filter dropdown form
    printf("<div class='px-3 mt-5'>"
           "<form method='get' class='mb-4'>"
           "<label class='form-label'><strong>Filter by Status:</strong></label>"
           "<select name='status' class='form-select w-25 d-inline'>"
           "<option%s>All</option>"
           "<option%s>Open</option>"
           "<option%s>InProgress</option>"
           "<option%s>Closed</option>"
           "</select>"
           "<button type='submit' class='btn btn-primary ms-2'>Filter</button>"
           "</form>",
           strcmp(statusFilter, "All") == 0 ? " selected" : "",
           strcmp(statusFilter, "Open") == 0 ? " selected" : "",
           strcmp(statusFilter, "InProgress") == 0 ? " selected" : "",
           strcmp(statusFilter, "Closed") == 0 ? " selected" : "");

    // Start table
    printf("<div class='table-responsive'><table class='table table-bordered table-striped table-hover'>"
           "<thead class='table-dark'><tr>"
           "<th>ID</th><th>Email</th><th>Problem</th><th>Description</th>"
           "<th>Status</th><th>Product Status</th><th>Created At</th><th>Pay</th>");
    if (strcmp(role, "admin") == 0)
        printf("<th>Actions</th>");
    printf("</tr></thead><tbody>");

    // Print each ticket row
    Ticket *tk;
    for (tk = tickets; tk; tk = tk->next) {
        int overdue = is_overdue(tk->createdAt);
        printf("<tr class='%s'>", overdue ? "table-danger" : "");

        if (strcmp(role, "admin") == 0) {
            printf("<form method='post' action='/cgi-bin/update_ticket.exe'>"
                   "<td><input type='text' name='ticket_id' value='%s' readonly class='form-control-plaintext'></td>", tk->id);
            printf("<td>%s</td><td>%s</td><td>%s</td>", tk->email, tk->problem, tk->description);
            printf("<td><select name='status' class='form-select'>"
                   "<option%s>Open</option><option%s>InProgress</option><option%s>Closed</option></select></td>",
                   strcmp(tk->status, "Open") == 0 ? " selected" : "",
                   strcmp(tk->status, "InProgress") == 0 ? " selected" : "",
                   strcmp(tk->status, "Closed") == 0 ? " selected" : "");
            printf("<td><select name='productStatus' class='form-select'>"
                   "<option%s>Need to Surrender</option><option%s>Surrendered</option><option%s>Dispatched</option></select></td>",
                   strcmp(tk->productStatus, "Need to Surrender") == 0 ? " selected" : "",
                   strcmp(tk->productStatus, "Surrendered") == 0 ? " selected" : "",
                   strcmp(tk->productStatus, "Dispatched") == 0 ? " selected" : "");
            printf("<td>%s</td>", tk->createdAt);
            printf("<td><select name='pay' class='form-select'>"
                   "<option%s>Not paid</option><option%s>paid</option></select></td>",
                   strcmp(tk->pay, "Not paid") == 0 ? " selected" : "",
                   strcmp(tk->pay, "paid") == 0 ? " selected" : "");
            printf("<td><button class='btn btn-success btn-sm me-2' type='submit'>Save</button></form>"
                   "<form method='post' action='/cgi-bin/delete_ticket.exe' style='display:inline;'>"
                   "<input type='hidden' name='ticket_id' value='%s'>"
                   "<button class='btn btn-danger btn-sm mt-1' type='submit'>Delete</button></form></td>", tk->id);
        } else {
            printf("<td>%s</td><td>%s</td><td>%s</td><td>%s</td>"
                   "<td>%s</td><td>%s</td><td>%s</td><td>%s</td>",
                   tk->id, tk->email, tk->problem, tk->description,
                   tk->status, tk->productStatus, tk->createdAt, tk->pay);
        }

        printf("</tr>");
    }

    // Close table and container
    printf("</tbody></table></div></div>");
	printf("</body></html>");

    // Free memory
    free_tickets(tickets);
    return 0;
}

