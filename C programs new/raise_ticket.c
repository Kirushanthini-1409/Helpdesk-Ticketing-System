// raise_ticket.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_post_data(char *b,int l){fread(b,1,l,stdin);b[l]=0;}
void url_decode(const char*s,char*d){while(*s){if(*s=='+')*d++=' ';else if(*s=='%'&&s[1]&&s[2]){char h[3]={s[1],s[2],0};*d++=(char)strtol(h,NULL,16);s+=2;}else*d++=*s;s++;}*d=0;}
void extract_field(const char*data,const char*f,char*out){char p[64];sprintf(p,"%s=",f);char*s=strstr(data,p);if(!s){*out=0;return;}s+=strlen(p);char*e=strchr(s,'&');int n=e?e-s:strlen(s);char t[1024]={0};strncpy(t,s,n);url_decode(t,out);}
void alert(const char*m,const char*l){printf("Content-Type: text/html\n\n<script>alert('%s');window.location.href='%s';</script>",m,l);}

int main(){
    char email[100],role[10];
    FILE*sf=fopen("session.txt","r");
    if(!sf){alert("Login required","/login.html");return 0;}
    fscanf(sf,"%[^,],%s",email,role);fclose(sf);
    if(strcmp(role,"user")!=0){alert("Only users","/cgi-bin/home.exe");return 0;}

    char *L=getenv("CONTENT_LENGTH");int ln=L?atoi(L):0;
    if(ln<=0){alert("No data","/raise_ticket.html");return 0;}
    char bd[1024],prob[200],desc[500];
    get_post_data(bd,ln);
    extract_field(bd,"problem",prob);
    extract_field(bd,"description",desc);

    int cnt=0;char ln2[1024];
    FILE*tf=fopen("tickets.txt","r");
    while(tf && fgets(ln2,sizeof(ln2),tf))cnt++;
    if(tf)fclose(tf);
    char tid[20];sprintf(tid,"TCKT-%d",cnt+1);

    tf=fopen("tickets.txt","a");
    if(!tf){alert("File err","/raise_ticket.html");return 0;}
    fprintf(tf,"%s,%s,%s,%s,Open\n",tid,email,prob,desc);
    fclose(tf);

    alert("Ticket raised","/cgi-bin/view_ticket.exe");
    return 0;
}

