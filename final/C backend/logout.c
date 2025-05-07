// logout.c
#include <stdio.h>
int main(){
    FILE *f=fopen("session.txt","w");
    if(f)fclose(f);
    printf("Content-Type: text/html\n\n");
    printf("<script>alert('Logged out successfully');window.location.href='/login.html';</script>");
    return 0;
}

