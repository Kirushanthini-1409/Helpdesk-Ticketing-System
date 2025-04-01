#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct node{
	int no;
	char name[30];
	char problem[100];
	char short_msg[100];
	char status[100];
	struct node *next;
}ticket;

typedef struct Node{
	char name[30];
	char password[20];
	char mail[30];
	char phone[20];
	ticket *tic;
	struct Node *next;
}node;

void ticket_file(node *head) 
{
    ticket temp;
    node *p;
    FILE *tic = fopen("tickets.dat", "rb");
    
    if (tic == NULL) 
	{
        printf("Error: Unable to open tickets.dat\n");
        return;
    }

    while (fread(&temp, sizeof(ticket), 1, tic) == 1) 
	{
        p = head;
        while (p != NULL) 
		{
            if (strcmp(temp.name, p->name) == 0) 
			{
                ticket *newTicket = (ticket *)malloc(sizeof(ticket));
                ticket *head=(ticket *)malloc(sizeof(ticket));
                if (!newTicket) 
				{
                    printf("Memory allocation failed.\n");
                    fclose(tic);
                    return;
                }
                *newTicket = temp;
                newTicket->next = NULL;
                
                if (p->tic == NULL) 
				{
					head->next=newTicket;
                    p->tic = head;
                } 
				else 
				{
                    ticket *p1 = p->tic;
                    while (p1->next != NULL)
					{
                        p1 = p1->next;
                    }
                    p1->next = newTicket;
                }
            }
            p = p->next;
        }
    }
    fclose(tic);
}

void user_file(node *head) 
{
    node temp;
    FILE *users = fopen("users.dat", "rb");

    if (users == NULL) 
	{
        printf("Error: Unable to open users.dat\n");
        return;
    }

    while (fread(&temp, sizeof(node), 1, users) == 1) 
	{
        node *newUser = (node *)malloc(sizeof(node));
        if (!newUser) 
		{
            printf("Memory allocation failed.\n");
            fclose(users);
            return;
        }
        *newUser = temp;
        newUser->next = NULL;
        newUser->tic = NULL;

        node *p = head;
        while (p->next != NULL) 
		{
            p = p->next;
        }
        p->next = newUser;
    }
    fclose(users);
}

void write(node *head) 
{
    FILE *user = fopen("users.dat", "wb");
    FILE *tic = fopen("tickets.dat", "wb");

    if (!user || !tic) 
	{
        printf("Error: Unable to open files for writing\n");
        if (user) fclose(user);
        if (tic) fclose(tic);
        return;
    }

    node *p = head->next;
    while (p != NULL) 
	{
        fwrite(p, sizeof(node), 1, user);
        ticket *temp = p->tic;
        while (temp != NULL) 
		{
            fwrite(temp, sizeof(ticket), 1, tic);
            temp = temp->next;
        }
        p = p->next;
    }
    fclose(user);
    fclose(tic);
}

int find_duplicate(char *f,node *head,int c)
{
	node *p=head;
	while(p!=NULL)
	{
		if(c==1)
		{
			if(strcmp(f,p->name)==0)
			{
				return 1;
			}
		}
		else if(c==2)
		{
			if(strcmp(f,p->mail)==0)
			{
				return 1;
			}
		}
		else if(c==3)
		{
			if(strcmp(f,p->phone)==0)
			{
				return 1;
			}
		}
		p=p->next;
	}
	return -1;
}

int is_password_valid(char *username)
{
	int i,j=0,len,k=0,n=0;
	for(len=0; *(username+len)!='\0'; len++)
	{
	}
	if(len<8)
	{
		return -1;
	}
	for(i=0; i<len; i++)
	{
		if((*(username+i)>='a' && *(username+i)<='z') ||( *(username+i)>='A' && *(username+i)<='Z' ))
		{
			j++;
		}
		else if( (*(username+i)>='0' && *(username+i)<='9'))
		{
			k++;
		}
		else if((*(username+i)>=' ' && *(username+i)<='@' ) || (*(username+i)>='[' && *(username+i)<='_' ))
		{
			n++;
		}
		else
		{
			return 0;
		}
	}
	if(len==k || len==j || len==k+j || len==n || len==k+n || len==n+j)
	{
		return 0;
	}
	else if(len==k+j+n)
	{
		return 1;
	}
}

void user_home(node *user)
{
	ticket *p,*p1;
	int cho,last;
	printf("\n");
	while(true)
	{
		p1=user->tic;
		printf("1-ticket raise \n2-view ticket \n3-exit \nEnter your choice :");
		scanf("%d", &cho);
		if(cho==1)
		{
			p=(ticket *)malloc(sizeof(ticket));
			while(p1->next!=NULL)
			{
				p1=p1->next;
			}
			printf("Type your probelm :");
			scanf(" %[^\n]s", p->problem);
			printf("Type your particular problem (explanation):");
			scanf(" %[^\n]s", p->short_msg);
			last=p1->no;
			p->no=last+1;
			strcpy(p->status,"new");
			strcpy(p->name,user->name);
			p->next=p1->next;
			p1->next=p;
		}
		else if(cho==2)
		{
			p1=p1->next;
			printf("--------------------------------\n");
			while(p1!=NULL)
			{
				printf("Ticket No. : %d \n",p1->no);
				printf("Probelm : %s \n",p1->problem);
				printf("Explanation : %s \n",p1->short_msg);
				printf("Status : %s \n", p1->status);
				printf("--------------------------------\n");
				p1=p1->next;
			}
		}
		else if(cho==3)
		{
			printf("Logging Out..............\n");
			break;
		}
		else
		{
			printf("Enter a proper input \n");
		}
		printf("\n");
	}
}

void register_user(node *head)
{
	int s,i,k,l,j,no=0;
	ticket *temp1;
	node *temp,*p=head;
	temp=(node *)malloc(sizeof(node));
	temp1=(ticket *)malloc(sizeof(ticket));
	while(1)
	{
		printf("Create your username :");
		scanf("%s",temp->name);
		s=find_duplicate(temp->name,head,1);
		if(s>0)
		{
			printf("User name already exits.....!! \n\n\n");
		}
		else if(strlen(temp->name)<10)
		{
			printf("Enter the username size greater than 10 \n\n");
		}
		else
		{
			break;
		}
	}
	while(1)
	{
		printf("Create a strong password :");
		scanf("%s",temp->password);
		j=is_password_valid(temp->password);
		if (j<0)
		{
			printf("Enter the password greater than 7 \n\n");
		}
		else if(j==1)
		{
			printf("The password is strong \n\n");
			break;
		}
		else
		{
			printf("The password is weak(add some special character or numbers or words)\n\n");
		}
	}
	while(1)
	{
		printf("Enter your mail id :");
		scanf("%s", temp->mail);
		s=find_duplicate(temp->mail,head,2);
		if(s>0)
		{
			printf("E-mail already exits.....!! \n\n\n");
		}
		else
		{
			for(i=0; temp->mail[i]!='\0'; i++)
			{
				if(temp->mail[i]=='@')
				{
					j=0;
					for(k=i+1 ; temp->mail[k]!='\0'; k++)
					{
						if(temp->mail[k]=='.')
						{
							j=0;
							for(l=k+1; temp->mail[l]!='\0'; l++)
							{
								if(temp->mail[l]>='a' && temp->mail[l]<='z')
								{
									j=0;
									no=1;
									break;
								}
								else
								{
									j=1;
								}
							}
						}
						else
						{
							j=1;
						}
					}
				}
				else
				{
					j=1;
				}
			}
		}	
		if(no==0)
		{
			printf("Invalid mail format \n Enter a proper one \n\n");
		}
		else
		{
			break;
		}
	}
	while(1)
	{
		printf("Enter your phone number :");
		scanf("%s", temp->phone);
		s=find_duplicate(temp->phone,head,3);
		if(s>0)
		{
			printf("Phone number already exits.....!! \n\n\n");
		}	
		else if(strlen(temp->phone)==10)
		{
			break;
		}
		else
		{
			printf("Invalid phone number \n\n");
		}
	}
	while(p->next!=NULL)
	{
		p=p->next;
	}
	temp1->no=0;
	temp1->next=NULL;
	temp->tic=temp1;
	temp->next=p->next;
	p->next=temp;
	printf("\n\n");
}

void login_user(node *head)
{
	node *p=head;
	int q=1;
	while(q!=0)
	{
		p=head;
		int found=0;
		char name[50], pass[50];
		printf("Enter your user name or mail id :");
		scanf("%s",name);
		printf("Enter your password :");
		scanf("%s", pass);
		while(p!=NULL)
		{
			if(strcmp(name,p->name)==0 && strcmp(pass,p->password)==0)
			{
				printf("Successfully Logged in.......\n");
				user_home(p);
				found++;
				q=0;
			}
			else if(strcmp(name,p->mail)==0 && strcmp(pass,p->password)==0)
			{
				printf("Successfully Logged in.......\n");
				user_home(p);
				found++;
				q=0;
			}
			p=p->next;
		}
		if(found==0)
		{
			printf("Invalid username/mail or Password \n");
		}
		printf("\nwant to login again? press 1 or exit-0 :");
		scanf("%d",&q);
		printf("\n\n");
	}
		
}

void display(node *head)
{
	node *p=head->next;
	printf("Registered users : \n");
	
	while(p!=NULL)
	{
		printf("----------------------------\n");
		printf("username : %s\n",p->name);
		printf("password : %s\n", p->password);
		printf("mail id : %s\n", p->mail);
		printf("phone no.: %s\n", p->phone);
		printf("----------------------------\n");
		p=p->next;
	}
	printf("\n\n");
}

void display_tickets(ticket *p2)
{
	printf("--------------------------------\n");
	printf("username : %s\n",p2->name);
	printf("Ticket No. : %d \n",p2->no);
	printf("Probelm : %s \n",p2->problem);
	printf("Explanation : %s \n",p2->short_msg);
	printf("Status : %s \n", p2->status);
	printf("--------------------------------\n\n");	
}

void view_request(node *head,int check)
{
	node *p1,*p=head;
	ticket *p2;
	p1=p->next;
	
	while(p1!=NULL)
	{
		p2=p1->tic;
		
		while(p2!=NULL)
		{
			if(strcmp(p2->status,"new")==0 && check==1)
			{
				display_tickets(p2);
			}
			else if(strcmp(p2->status,"inprogress")==0 && check==2)
			{
				display_tickets(p2);
			}
			else if(strcmp(p2->status,"completed")==0 && check==3)
			{
				display_tickets(p2);
			}
			else if(strcmp(p2->status,"pending")==0 && check==4)
			{
				display_tickets(p2);
			}
			p2=p2->next;
		}
		p1=p1->next;
	}
}

void update(node* head)
{
	node* p=head;
	ticket *p1;
	char nam[50];
	int no,upd,j=0,sta;
	printf("====New Tickets==== \n\n");
	view_request(head,1);
	printf("====Inprogress Tickets==== \n\n");
	view_request(head,2);
	printf("====Completed Tickets==== \n\n");
	view_request(head,3);
	printf("====Pending Tickets==== \n\n");
	view_request(head,4);
	printf("Enter the username to update their ticket :");
	scanf("%s",nam);
	while(p!=NULL)
	{
		if(strcmp(p->name,nam)==0)
		{
			j=1;
			p1=p->tic;
			printf("Enter the ticket no. to update :");
			scanf("%d", &no);
			while(p1!=NULL)
			{
				if(p1->no==no)
				{
					printf("1-Inprogress \n2-Completed \n3-pending \nEnter your choice to update :");
					scanf("%d",&sta);
					
					if(sta==1)
					{
						strcpy(p1->status,"inprogress");
						printf("Ticket status updated to inprogress \n");
					}
					else if(sta==2)
					{
						strcpy(p1->status,"completed");
						printf("Ticket status updated to completed \n");
					}
					else if(sta==3)
					{
						strcpy(p1->status,"pending");
						printf("Ticket status updated to pending \n");
					}
					else
					{
						printf("Invalid input\n");
					}
					
					break;
				}
				p1=p1->next;
			}
		}
		p=p->next;
	}
	if(j==0)
	{
		printf("The user name is not found \n");
	}
}

void admin_home(node *admin, node *head)
{
	while(1)
	{
		int cho;
		printf("========ADMIN HOME======== \n1-Display registered users \n2-view new requests \n3-update status \n4-view inprogress tickets \n5-view completed tickets \n6-view pending \n7-exit \nEnter your choice :")	;
		scanf("%d",&cho);
		if(cho==1)
		{
			display(head);
		}
		else if(cho==2)
		{
			printf("====New Tickets==== \n\n");
			view_request(head,1);
		}
		else if(cho==3)
		{
			update(head);
		}
		else if(cho==4)
		{
			printf("====Inprogress Tickets==== \n\n");
			view_request(head,2);
		}
		else if(cho==5)
		{
			printf("====Completed Tickets==== \n\n");
			view_request(head,3);
		}
		else if(cho==6)
		{
			printf("====Pending Tickets==== \n\n");
			view_request(head,4);
		}
		else if(cho==7)
		{
			printf("Thank you \nexiting..........\n");
			break;
		}
		printf("\n\n");
	}
}

void admin_page(node *admin,node *head)
{
	node *p=admin;
	char name[50],pass[50];
	printf("Enter your username /mail id :");
	scanf("%s",name);
	printf("Enter your password :");
	scanf("%s",pass);
	if((strcmp(name,p->name)==0 && strcmp(pass,p->password)==0) || (strcmp(name,p->mail)==0 && strcmp(pass,p->password)==0))
	{
		printf("Successfully Logged in.......\n\n\n");
		admin_home(p,head);
	}
	else
	{
		printf("Invalid username/mail or Password \n");
	}
}

int main()
{
	node *head,*admin;
	int cho,user;
	head=(node *)malloc(sizeof(node));
	admin=(node *)malloc(sizeof(node));
	strcpy(admin->name,"kishor_098");
	strcpy(admin->password,"kishor@90");
	strcpy(admin->mail,"kishor98@gmail.com");
	strcpy(admin->phone,"9514921683");
	admin->next=NULL;
	admin->tic=NULL;
	head->next=NULL;
	user_file(head);
	ticket_file(head);
	
	while(true)
	{
		printf("========HOME======== \n1-User \n2-Admin \n3-exit \nEnter your choice :");
		scanf("%d",&user);
		if(user==1)
		{
			while(1)
			{
				printf("========USER PAGE======== \n1-Register \n2-Login \n3-exit \nEnter your choice :");
				scanf("%d",&cho);
				
				if(cho==1)
				{
					register_user(head);
				}
				else if(cho==2)
				{
					login_user(head);
				}
				else if(cho==3)
				{
					printf("Thank you \nexiting .............\n");
					break;
				}
				else
				{
					printf("Enter a proper input!!!! \n");
				}
			}
		}
		else if(user==2)
		{
			while(1)
			{
				printf("========ADMIN PAGE======== \n1-Login \n2-exit \nEnter your choice :");
				scanf("%d",&cho);
				
				if(cho==2)
				{
					printf("Thank you \nexiting .............\n");
					break;
				}
				if(cho==1)
				{
					admin_page(admin,head);
				}
				else
				{
					printf("Enter a proper input!!!! \n");
				}
				printf("\n\n");
			}
		}
		else if(user==3)
		{
			write(head);
			printf("Thank you \nexiting.............\n");
			break;
		}
		else
		{
			printf("Invalid Input \n");
		}
		printf("\n\n");
	}
}
