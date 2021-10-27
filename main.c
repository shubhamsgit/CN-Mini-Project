#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

//Declaring Socket as Global for Signalling in my_
int SD;
int fd;

//my_isr is just for changing the behaviour of SIGNAL 2 i.e ctrl+x /////
void my_isr(int n)
{
printf("In Isr..\n");
write(fd,"**************SERVER TERMINATED!!************",50);
close(SD);
printf("SOCKET closed\n");
kill(getpid(),9);///Kill this process now…
}

int main()
{

int portNumber = 8080;

//Sending buffer for “sever to client”
char send_buff[300];
memset(send_buff,0,300);

//receving buffer for “Client to server”
char rec_buff[300];
memset(rec_buff,0,300);

//FIll the  family and address..so that we can bind this details with the socket to be created..
struct sockaddr_in addr;
memset(&addr,0,sizeof(addr));
addr.sin_family=AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(portNumber);

//////////////////////Creating a SOCKET/////////////////////////
if((SD=socket(AF_INET,SOCK_STREAM,0))==-1)
{
printf("ERROR:Socket failed\n");

return 1;
}
printf("SUCCESS:Socket opened\n");

///////////////////////////binding.//////////////////////////////////////


///////////////////////////Listening./////////////////////////////////////////
printf("LIstening\n");
listen(SD,5);//listen for any client for up to 5 queues

///////////////////////////////////Accepting/.///////////////////////////////

if((fd=accept(SD,NULL, NULL))<0)
{
printf("ERROR:ACCEPT by server\n");
close(SD);
return 1;
}
printf("SUCCESS:ACCEPT by Server\n");

while(1){
char strB[100];
printf("enter country name\n");
scanf("%s", strB);
char cmd[512];
char strC[500];
char strA[500]="curl 'https://api.covid19api.com/country//status/confirmed/live?from=2021-08-01T00:00:00Z&to=2021-08-02T00:00:00Z'";
int x = 41;
strncpy(strC, strA, x);
strC[x] = '\0';
strcat(strC, strB);
strcat(strC, strA + x);
sprintf(cmd, "%s", strC);
system(cmd);
}

signal(2,my_isr);//modifying the behaviour of the signal 2 as defined in my_isr..routine

//////////////////Creating parallel processes in while execution//////////////////////////

if(fork()==0)//CHILD process..
{
while(1)
{
fgets(send_buff,sizeof(send_buff),stdin);
write(fd,send_buff,300);
}
}
else//PARENT process…………..
{
while(1)
{
if(read(fd,rec_buff,300)!=0)
puts(rec_buff);
}
}

}
