// view_users.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    char name[100], email[100], password[100], phone[20], role[10];
    struct User *next;
} User;

User*load_users(){FILE*fp=fopen("users.txt","r");if(!fp)return NULL;User*h=NULL,*t=NULL;char ln[512];while(fgets(ln,sizeof(ln),fp)){User*u=malloc(sizeof(User));sscanf(ln,"%[^,],%[^,],%[^,],%[^,],%s",u->name,u->email,u->password,u->phone,u->role);u->next=NULL;if(!h)h=t=u;else{t->next=u;t=u;}}fclose(fp);return h;}

int main(){
    char email[100],role[10];
    FILE*sf=fopen("session.txt","r");
    if(!sf){printf("Content-Type: text/html\n\n<script>alert('Login required');window.location.href='/login.html';</script>");return 0;}
    fscanf(sf,"%[^,],%s",email,role);fclose(sf);
    if(strcmp(role,"admin")!=0){printf("Content-Type: text/html\n\n<script>alert('Access denied');window.location.href='/cgi-bin/home.exe';</script>");return 0;}

    User*us=load_users();
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
    printf("</ul></div></nav>"
           "<div class='container mt-4'><h2>Registered Users</h2>"
           "<table class='table table-bordered'><thead><tr><th>Name</th><th>Email</th><th>Password</th><th>Phone</th></tr></thead><tbody>");
    while(us){
        printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
               us->name,us->email,us->password,us->phone);
        us=us->next;
    }
    printf("</tbody></table></div></body></html>");
    return 0;
}

