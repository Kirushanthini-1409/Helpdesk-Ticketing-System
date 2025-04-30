// update_ticket.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ticket {
    char id[20], email[100], problem[200], description[500], status[20];
    struct Ticket *next;
} Ticket;

void get_post_data(char *b,int l){fread(b,1,l,stdin);b[l]=0;}
void url_decode(const char*s,char*d){while(*s){if(*s=='+')*d++=' ';else if(*s=='%'&&s[1]&&s[2]){char h[3]={s[1],s[2],0};*d++=(char)strtol(h,NULL,16);s+=2;}else*d++=*s;s++;}*d=0;}
void extract_field(const char*data,const char*f,char*out){char p[64];sprintf(p,"%s=",f);char*s=strstr(data,p);if(!s){*out=0;return;}s+=strlen(p);char*e=strchr(s,'&');int n=e?e-s:strlen(s);char t[512]={0};strncpy(t,s,n);url_decode(t,out);}

Ticket*load_tickets(){FILE*fp=fopen("tickets.txt","r");if(!fp)return NULL;Ticket*h=NULL,*t=NULL;char ln[1024];while(fgets(ln,sizeof(ln),fp)){Ticket*x=malloc(sizeof(Ticket));sscanf(ln,"%[^,],%[^,],%[^,],%[^,],%s",x->id,x->email,x->problem,x->description,x->status);x->next=NULL;if(!h)h=t=x;else{t->next=x;t=x;}}fclose(fp);return h;}
void save_tickets(Ticket*h){FILE*fp=fopen("tickets.txt","w");while(h){fprintf(fp,"%s,%s,%s,%s,%s\n",h->id,h->email,h->problem,h->description,h->status);h=h->next;}fclose(fp);}
void alert(const char*m,const char*l){printf("Content-Type: text/html\n\n<script>alert('%s');window.location.href='%s';</script>",m,l);}

int main(){
    char email[100],role[10];
    FILE*sf=fopen("session.txt","r");
    if(!sf){alert("Login required","/login.html");return 0;}
    fscanf(sf,"%[^,],%s",email,role);fclose(sf);
    if(strcmp(role,"admin")!=0){alert("Access denied","/cgi-bin/home.exe");return 0;}

    char *L=getenv("CONTENT_LENGTH");int ln=L?atoi(L):0;if(ln<=0){alert("No data","/cgi-bin/view_ticket.exe");return 0;}
    char bd[2048],id[20],pr[200],desc[500],st[20];
    get_post_data(bd,ln);
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
    
    extract_field(bd,"ticket_id",id);
    extract_field(bd,"problem",pr);
    extract_field(bd,"description",desc);
    extract_field(bd,"status",st);

    Ticket*tk=load_tickets();Ticket*cur=tk;
    while(cur){if(!strcmp(cur->id,id)){strcpy(cur->problem,pr);strcpy(cur->description,desc);strcpy(cur->status,st);break;}cur=cur->next;}
    save_tickets(tk);
    alert("Ticket updated","/cgi-bin/view_ticket.exe");
    return 0;
}

