// Anish Kulkarni
// 2020A7PS0975P
#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h> // use time.h header file to use time
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h> // use select() system call
#include <fcntl.h>
#define BUFLEN 512 // Max length of buffer
#define PORT 8882  // The port on which to send data
#define TIMEOUT 2

typedef struct packet
{
    int sq_no;
    char data[BUFLEN];
    int size;
    int isAck;
    int clientNo;
    int data_size;
} PKT;

void die(char* s)
{
    perror(s);
    exit(1);
}
int main()
{
    /* CREATE A TCP SOCKET*/

    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) { printf("Error in opening a socket"); exit(0); }
    printf("Client Socket Created\n");
    
    /*CONSTRUCT SERVER ADDRESS STRUCTURE*/
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    
    /*memset() is used to fill a block of memory with a particular value*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); //You can change port number here
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Specify server's IP address here
    printf("Address assigned\n");
    
    /*ESTABLISH CONNECTION*/
    int c1 = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (c1 < 0)
    {
        printf("Error while establishing connection");
        exit(0);
    }
    printf("Connection Established\n");

    char const* const fileName = "name.txt";

    FILE* fp = fopen(fileName, "r");
    

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    int state=0;
    PKT sendpkt;
    PKT rcvpkt;
    int global_sq_no=0;
    int flag=0;
    struct timeval tv;
    time_t start_time;
    char c;
    while(1)
    {
        switch(state)
        {
            case 0:   //send packet from c1
            {

                char line[BUFLEN];
                
                if(c=='.'){
                    sendpkt.isAck=0;
                    strcpy(sendpkt.data,".");
                    sendpkt.sq_no=global_sq_no;
                    sendpkt.clientNo=0;
                    sendpkt.data_size=1;
                    send(sock, &sendpkt, sizeof(sendpkt), 0);
                    flag=1;
                }
                while((c=fgetc(fp))!=',' && c!='.')
                {
                    if(c==EOF)
                    {
                        flag=1;
                        break;
                    }
                   strncat(line, &c, 1);
                }
            
                //printf("%s\n",line);
                 int len = strlen(line);
                 //printf("%d\n",len);
                strcpy(sendpkt.data, line);
                memset(line, 0, sizeof(line));
                sendpkt.size = len;
                sendpkt.sq_no = global_sq_no;
                sendpkt.isAck = 0;
                sendpkt.clientNo=0;
                sendpkt.data_size=len;
                //printf("Sending packet from c1 ............\n");
                send(sock, &sendpkt, sizeof(sendpkt), 0);
                //printf("Packet sent from c1 with seq_no :%d  data:%s size : %d isAck : %d\n",sendpkt.sq_no,sendpkt.data,sendpkt.size,sendpkt.isAck);
                printf("SENT_PKT : Seq.No = %d, Size = %d Bytes, Data= %s \n",sendpkt.sq_no,sendpkt.data_size,sendpkt.data);
                
                 state=1;
                  gettimeofday(&tv, NULL);
            start_time = time(NULL);
                 break;
            }
            case 1: //wait for ack
            {
            fd_set readfds;
            FD_ZERO(&readfds);   //monitor socket for incoming packets
            FD_SET(sock, &readfds);

            tv.tv_sec = TIMEOUT;// set the timeout to 2 seconds
            tv.tv_usec = 0;

            if (select(sock + 1, &readfds, NULL, NULL, &tv) == 0)
            {
             printf("TIMEOUT\n");
             
              send(sock, &sendpkt, sizeof(sendpkt), 0);
              printf("RE-TRANSMIT_PKT : Seq.No = %d, Size = %d Bytes , Data = %s\n",sendpkt.sq_no,sendpkt.data_size,sendpkt.data);
               gettimeofday(&tv, NULL); // restart timer
            start_time = time(NULL);


              break;
            }
                char line[BUFLEN];
                int bytesRecvd = recv(sock, &rcvpkt, sizeof(rcvpkt), 0); //receive ack
                if(rcvpkt.isAck==1 && rcvpkt.sq_no==global_sq_no && rcvpkt.clientNo==0){
                   // printf("Received ack from s2 with seq_no :%d  size : %d isAck : %d\n",rcvpkt.sq_no,rcvpkt.size,rcvpkt.isAck);
                    printf("RECV_ACK : Seq.No = %d \n",rcvpkt.sq_no);
                    global_sq_no+=sendpkt.data_size;
                    state=0;
                }
                else{
                    state=1;
                }
                 
                 break;
            }
           
        }
        if(flag==1)
        {
            break;
        }
    }




    return 0;
}