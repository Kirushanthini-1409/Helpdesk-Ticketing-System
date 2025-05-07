// delete_ticket.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100
#define MAX_PROB 200
#define MAX_DESC 500

typedef struct Ticket {
    char id[20];
    char email[MAX_LEN];
    char problem[MAX_PROB];
    char description[MAX_DESC];
    char status[20];
    char productStatus[30];
    char createdAt[50];
    char pay[50];
    struct Ticket *next;
} Ticket;

void get_post_data(char *b, int l) {
    fread(b, 1, l, stdin);
    b[l] = '\0';
}

void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') *dest++ = ' ';
        else if (*src == '%' && src[1] && src[2]) {
            char hex[3] = {src[1], src[2], 0};
            *dest++ = (char)strtol(hex, NULL, 16);
            src += 2;
        } else *dest++ = *src;
        src++;
    }
    *dest = '\0';
}

void extract_field(const char *data, const char *field, char *out) {
    char key[64];
    sprintf(key, "%s=", field);
    char *start = strstr(data, key);
    if (!start) { out[0] = '\0'; return; }
    start += strlen(key);
    char *end = strchr(start, '&');
    int len = end ? (end - start) : strlen(start);
    char tmp[512] = {0};
    strncpy(tmp, start, len);
    url_decode(tmp, out);
}

void strip_newline(char *s) {
    char *p = strchr(s, '\n');
    if (p) *p = '\0';
}

Ticket *load_tickets() {
    FILE *fp = fopen("tickets.txt", "r");
    if (!fp) return NULL;
    Ticket *front = NULL, *rear = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        strip_newline(line);
        Ticket *t = malloc(sizeof(Ticket));
        if (!t) continue;
        memset(t, 0, sizeof(Ticket));
        int count = sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]",
            t->id, t->email, t->problem, t->description,
            t->status, t->productStatus, t->createdAt, t->pay);
        if (count >= 7) {
            t->next = NULL;
            if (!front) front = rear = t;
            else { rear->next = t; rear = t; }
        } else {
            free(t);
        }
    }
    fclose(fp);
    return front;
}

void save_tickets(Ticket *front) {
    FILE *fp = fopen("tickets.txt", "w");
    if (!fp) return;
    while (front) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s\n",
            front->id, front->email, front->problem, front->description,
            front->status, front->productStatus, front->createdAt, front->pay);
        front = front->next;
    }
    fclose(fp);
}

void append_ticket_to_file(const Ticket *t, const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (!fp) return;
    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s\n",
        t->id, t->email, t->problem, t->description,
        t->status, t->productStatus, t->createdAt, t->pay);
    fclose(fp);
}

void free_tickets(Ticket *front) {
    Ticket *tmp;
    while (front) {
        tmp = front;
        front = front->next;
        free(tmp);
    }
}

void alert(const char *msg, const char *loc) {
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s'); window.location.href='%s';</script>", msg, loc);
}

int main() {
    char sessionEmail[MAX_LEN], sessionRole[10];
    FILE *sf = fopen("session.txt", "r");
    if (!sf) { alert("Please login first", "/login.html"); return 0; }
    fscanf(sf, "%[^,],%s", sessionEmail, sessionRole);
    fclose(sf);

    if (strcmp(sessionRole, "admin") != 0) {
        alert("Access denied", "/cgi-bin/home.exe");
        return 0;
    }

    char *lenStr = getenv("CONTENT_LENGTH");
    int len = lenStr ? atoi(lenStr) : 0;
    if (len <= 0) {
        alert("No data received", "/cgi-bin/view_ticket.exe");
        return 0;
    }

    char postData[2048], ticketId[20];
    get_post_data(postData, len);
    extract_field(postData, "ticket_id", ticketId);

    Ticket *front = load_tickets();
    Ticket *prev = NULL, *cur = front;

    while (cur) {
        if (strcmp(cur->id, ticketId) == 0) {
            if (strcmp(cur->productStatus, "Dispatched") == 0)
                append_ticket_to_file(cur, "completed.txt");
            else
                append_ticket_to_file(cur, "deleted.txt");

            if (prev) prev->next = cur->next;
            else front = cur->next;
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    save_tickets(front);
    free_tickets(front);
    alert("Ticket deleted successfully", "/cgi-bin/view_ticket.exe");
    return 0;
}

