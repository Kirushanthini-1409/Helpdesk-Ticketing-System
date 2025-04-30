// tic_home.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char sess_email[100] = "", sess_role[10] = "";
    FILE *sf = fopen("session.txt","r");
    if (!sf) {
        printf("Content-Type: text/html\n\n<script>alert('Login required');window.location.href='/login.html';</script>");
        return 0;
    }
    fscanf(sf,"%99[^,],%9s", sess_email, sess_role);
    fclose(sf);

    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html lang='en'><head>"
           "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>Home - Helpdesk Ticketing System</title>"
           "<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>"
           "<style>body{background:url('https://eazypc.in/wp-content/uploads/2023/07/Video-Editing-Second-Hand-Laptops-Mobile-Cover-BG-EazyPCs.jpg')no-repeat center center fixed;background-size:cover;color:white;} .brand-section{background:rgba(0,0,0,0.7);padding:20px;border-radius:10px;}</style>"
           "</head><body>");

    // Navbar
    printf("<nav class='navbar navbar-expand-lg navbar-dark bg-dark px-2 py-3'>"
           "<div class='container-fluid'>"
           "<a class='navbar-brand' href='/home.exe'><h3>HI-5 SOLUTIONS</h3></a>"
           "<h1 class='text-white'>HELPDESK TICKETING SYSTEM</h1>"
           "<ul class='navbar-nav'>"
           "<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/home.exe'>Home</a></li>"
           "<li class='nav-item'><a class='nav-link text-white' href='#contact'>Contact</a></li>");
    if (strlen(sess_role)==0) {
        printf("<li class='nav-item'><a class='nav-link text-white' href='/register.html'>Register</a></li>"
               "<li class='nav-item'><a class='nav-link text-white' href='/login.html'>Login</a></li>");
    } else {
        printf("<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/logout.exe'>Logout</a></li>");
    }
    printf("</ul></div></nav>");

    // Content
    printf("<div class='container d-flex justify-content-center align-items-center vh-100'><div class='text-center'>");
    if (strcmp(sess_role,"admin")==0) {
        printf("<h2 class='mb-4 text-white'>Admin Panel</h2>"
               "<a href='/cgi-bin/view_ticket.exe' class='btn btn-danger btn-lg mb-3'>View All Tickets</a><br>"
               "<a href='/cgi-bin/view_users.exe' class='btn btn-danger btn-lg'>View Registered Users</a>");
    } else {
        printf("<h2 class='mb-4 text-white'>Helpdesk Actions</h2>"
               "<a href='/cgi-bin/raise_ticket.exe' class='btn btn-danger btn-lg mb-3'>Raise a Ticket</a><br>"
               "<a href='/cgi-bin/view_ticket.exe' class='btn btn-danger btn-lg'>View Your Tickets</a>");
    }
    printf("</div></div>"
           "<footer class='bg-dark text-white text-center py-3'>Address | Copyrights</footer>"
           "</body></html>");

    return 0;
}

