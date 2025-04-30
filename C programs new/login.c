// login.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    char name[100],email[100],password[100],phone[20],role[10];
    struct User *next;
} User;

void get_post_data(char *b,int l){ fread(b,1,l,stdin);b[l]=0; }

void url_decode(const char *s, char *d) {
    while (*s) {
        if (*s=='+') *d++=' ';
        else if (*s=='%'&&s[1]&&s[2]) {
            char h[3]={s[1],s[2],0};
            *d++=(char)strtol(h,NULL,16);
            s+=2;
        } else *d++=*s;
        s++;
    }
    *d=0;
}

void extract_field(const char *data,const char *f,char *out){
    char p[64]; sprintf(p,"%s=",f);
    char *s=strstr(data,p);
    if(!s){out[0]=0;return;}
    s+=strlen(p);
    char *e=strchr(s,'&');
    int n=e?e-s:strlen(s);
    char t[256]={0};
    strncpy(t,s,n);
    url_decode(t,out);
}

User* load_users(){
    FILE *fp=fopen("users.txt","r");
    if(!fp) return NULL;
    User *h=NULL,*t=NULL; char ln[512];
    while(fgets(ln,sizeof(ln),fp)){
        User *u=malloc(sizeof(User));
        sscanf(ln,"%[^,],%[^,],%[^,],%[^,],%s",
               u->name,u->email,u->password,u->phone,u->role);
        u->next=NULL;
        if(!h) h=t=u; else {t->next=u; t=u;}
    }
    fclose(fp);
    return h;
}

void alert(const char*m,const char*l){
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s');window.location.href='%s';</script>",m,l);
}

int main(){
    char *L=getenv("CONTENT_LENGTH");int ln=L?atoi(L):0;
    if(ln<=0){alert("No data","/login.html");return 0;}
    char bd[1024]; get_post_data(bd,ln);
    char role[10],u[100],p[100];
    extract_field(bd,"role",role);
    extract_field(bd,"username",u);
    extract_field(bd,"password",p);

    if(!strcmp(role,"admin")&&
      (!strcmp(u,"kishor")||!strcmp(u,"kishor@gmail.com"))&&
      !strcmp(p,"kishor9361")){
        FILE*sf=fopen("session.txt","w");
        if(sf){fprintf(sf,"kishor@gmail.com,admin\n");fclose(sf);}
        alert("Admin logged in.....","/cgi-bin/tic_home.exe");
        return 0;
    }

    User*us=load_users();
    while(us){
        if((!strcmp(us->email,u)||!strcmp(us->name,u))
           &&!strcmp(us->password,p)){
            FILE*sf=fopen("session.txt","w");
            if(sf){fprintf(sf,"%s,user\n",us->email);fclose(sf);}
            alert("User logged in......","/cgi-bin/home.exe");
            return 0;
        }
        us=us->next;
    }
    alert("Invalid credentials","/login.html");
    return 0;
}

