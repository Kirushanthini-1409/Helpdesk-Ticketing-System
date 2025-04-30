// view_ticket.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ticket {
    char id[20], email[100], problem[200], description[500], status[20];
    struct Ticket *next;
} Ticket;

Ticket* load_tickets(){
    FILE*fp=fopen("tickets.txt","r");
    if(!fp)return NULL;
    Ticket *h=NULL,*t=NULL;
    char ln[1024];
    while(fgets(ln,sizeof(ln),fp)){
        Ticket *x=malloc(sizeof(Ticket));
        sscanf(ln,"%[^,],%[^,],%[^,],%[^,],%s",x->id,x->email,x->problem,x->description,x->status);
        x->next=NULL;
        if(!h)h=t=x; else {t->next=x; t=x;}
    }
    fclose(fp);return h;
}

int main(){
    char email[100],role[10];
    FILE*sf=fopen("session.txt","r");
    if(!sf){printf("Content-Type: text/html\n\n<script>alert('Login');window.location.href='/login.html';</script>");return 0;}
    fscanf(sf,"%[^,],%s",email,role);fclose(sf);

    Ticket*tk=load_tickets();
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang='en'><head>"
           "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>Home - Helpdesk Ticketing System</title>"
           "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>"
           "</head><body>");

    // Navbar
    printf("<nav class='navbar navbar-expand-lg navbar-dark bg-dark px-2 py-3'>"
           "<div class='container-fluid'>"
           "<a class='navbar-brand' href='/home.exe'><h3>HI-5 SOLUTIONS</h3></a>"
           "<h1 class='text-white'>HELPDESK TICKETING SYSTEM</h1>"
           "<ul class='navbar-nav'>"
           "<li class='nav-item'><a class='nav-link text-white' href='/tic_home.html'>Home</a></li>"
           "<li class='nav-item'><a class='nav-link text-white' href='#contact'>Contact</a></li>");
    printf("<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/logout.exe'>Logout</a></li>");
    printf("</ul></div></nav>");
	printf("<div class='container mt-4'><h2>Tickets</h2><table class='table table-bordered'><thead><tr>"
           "<th>ID</th><th>Email</th><th>Problem</th><th>Description</th><th>Status</th>");
    if(!strcmp(role,"admin"))printf("<th>Update</th>");
    printf("</tr></thead><tbody>");

    while(tk){
        if(!strcmp(role,"admin")||!strcmp(tk->email,email)){
            printf("<tr><form action='/cgi-bin/update_ticket.exe' method='post'>");
            printf("<td><input name='ticket_id' value='%s' readonly class='form-control-plaintext'></td>",tk->id);
            printf("<td>%s</td>",tk->email);
            printf("<td><input name='problem' value='%s' class='form-control'></td>",tk->problem);
            printf("<td><textarea name='description' class='form-control'>%s</textarea></td>",tk->description);
            printf("<td>");
            if(!strcmp(role,"admin")){
                printf("<select name='status' class='form-select'>"
                       "<option%s>Open</option>"
                       "<option%s>InProgress</option>"
                       "<option%s>Closed</option>"
                       "</select>",
                       !strcmp(tk->status,"Open")?" selected":"",
                       !strcmp(tk->status,"InProgress")?" selected":"",
                       !strcmp(tk->status,"Closed")?" selected":"");
            } else {
                printf("%s",tk->status);
            }
            printf("</td>");
            if(!strcmp(role,"admin")){
                printf("<td><button class='btn btn-primary' type='submit'>Save</button></td>");
            }
            printf("</form></tr>");
        }
        tk=tk->next;
    }

    printf("</tbody></table></div></body></html>");
    return 0;
}

