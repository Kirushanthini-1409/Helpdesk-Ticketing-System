// home.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Read session
    char sess_email[100] = "", sess_role[10] = "";
    FILE *sf = fopen("session.txt","r");
    if (sf) {
        fscanf(sf,"%99[^,],%9s", sess_email, sess_role);
        fclose(sf);
    }

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
           "<a class='navbar-brand' href='/cgi-bin/home.exe'><h3>HI-5 SOLUTIONS</h3></a>"
           "<h1 class='text-white'>HELPDESK TICKETING SYSTEM</h1>"
           "<ul class='navbar-nav'>"
           "<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/home.exe'>Home</a></li>"
           "<li class='nav-item'><a class='nav-link text-white' href='/contact.html'>Contact</a></li>");
    if (strlen(sess_role)==0) {
        printf("<li class='nav-item'><a class='nav-link text-white' href='/login.html'>Login</a></li>"
		"<li class='nav-item'><a class='nav-link text-white' href='/register.html'>Register</a></li>");
    } else {
        printf("<li class='nav-item'><a class='nav-link text-white' href='/cgi-bin/logout.exe'>Logout</a></li>");
    }
    printf("</ul></div></nav>");

    printf("<div class='container text-center mt-4'>"
           "<h2>Welcome to HI-5 SOLUTIONS</h2>"
           "<p>Your one-stop solution for laptops, mobiles, and repairs.</p>"
           "<img src='https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRInwp3KwXrDSLlYu5VB6A5wwEG-yblJD_EOw&s' class='img-fluid rounded' style='height:200px;'>"
           "</div>");

    // Steps and Buttons
    printf("<div class='container mt-4 brand-section'><h3>Steps to Raise a New Ticket</h3><ol>"
           "<li>Login/Register</li><li>Click the button below</li><li>Open a New Ticket</li>"
           "<li>Register Your Details and Queries</li><li>Submit</li><li>View Your Ticket Status</li>"
           "</ol>");
    if (strcmp(sess_role,"admin")==0) {
        printf("<a href='/cgi-bin/tic_home.exe' class='btn btn-primary'>Admin Panel</a>");
    } else if (strcmp(sess_role,"user")==0) {
        printf("<a href='/cgi-bin/tic_home.exe' class='btn btn-primary'>Raise & View Ticket</a>");
    }
    printf("</div>");

    // Contact & Footer
    printf("<div class='container mt-4 brand-section' id='contact'><h3>Our Address</h3><p>123 Tech Street, Cityname, Country</p></div>"
           "<footer class='bg-dark text-white text-center py-3 mt-5'>&copy; 2025 HI-5 SOLUTIONS | Laptop & Mobile Sales and Repairs</footer>"
           "</body></html>");

    return 0;
}

