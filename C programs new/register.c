// register.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    char name[100], email[100], password[100], phone[20], role[10];
    struct User* next;
} User;

void get_post_data(char *buf, int len) {
    fread(buf,1,len,stdin);
    buf[len]='\0';
}

void url_decode(const char *src, char *dest) {
    while (*src) {
        if (*src=='+') *dest++=' ';
        else if (*src=='%'&&src[1]&&src[2]) {
            char h[3]={src[1],src[2],0};
            *dest++=(char)strtol(h,NULL,16);
            src+=2;
        } else *dest++=*src;
        src++;
    }
    *dest=0;
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

User* load_users() {
    FILE *fp=fopen("users.txt","r");
    if(!fp) return NULL;
    User *h=NULL,*t=NULL;
    char ln[512];
    while(fgets(ln,sizeof(ln),fp)){
        User *u=malloc(sizeof(User));
        sscanf(ln,"%[^,],%[^,],%[^,],%[^,],%s",
               u->name,u->email,u->password,u->phone,u->role);
        u->next=NULL;
        if(!h) h=t=u;
        else {t->next=u; t=u;}
    }
    fclose(fp);
    return h;
}

int exists(User *h,const char *n,const char*e){
    for(;h;h=h->next)
        if(!strcmp(h->name,n)||!strcmp(h->email,e)) return 1;
    return 0;
}

void alert(const char *m,const char *l){
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('%s');window.location.href='%s';</script>",m,l);
}

int main(){
    char *l=getenv("CONTENT_LENGTH");int ln=l?atoi(l):0;
    if(ln<=0){alert("No data","/register.html");return 0;}
    char bd[1024]; get_post_data(bd,ln);
    char n[100],e[100],p[100],c[100],ph[20];
    extract_field(bd,"username",n);
    extract_field(bd,"email",e);
    extract_field(bd,"password",p);
    extract_field(bd,"confirm-password",c);
    extract_field(bd,"phone",ph);
    if(strcmp(p,c)){alert("Passwords mismatch","/register.html");return 0;}
    User *us=load_users();
    if(exists(us,n,e)){alert("E-mail already exists","/register.html");return 0;}
    FILE*fp=fopen("users.txt","a");
    if(!fp){alert("File error","/register.html");return 0;}
    fprintf(fp,"%s,%s,%s,%s,user\n",n,e,p,ph);
    fclose(fp);
    alert("Registered! Login now","/login.html");
    return 0;
}

