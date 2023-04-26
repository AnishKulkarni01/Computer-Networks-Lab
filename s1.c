#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAXPENDING 5
#define BUFFERSIZE 32
#define BUFLEN 512
#define PDR 10    // write 10 for 10% loss rate
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
    int c1_seqno=-1;
    int c2_seqno=-1;
    PKT sendpkt1;
    PKT rcvpkt1;
    PKT sendpkt2;
    PKT rcvpkt2;
    /*CREATE A TCP SOCKET*/

    int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0) { printf("Error while server socketcreation"); exit(0); }
    printf("Server Socket Created\n");

    /*CONSTRUCT LOCAL ADDRESS STRUCTURE*/

    struct sockaddr_in serverAddress, clientAddress1, clientAddress2;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Server address assigned\n");

    int temp = bind(serverSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress));
    if (temp < 0)
    {
        printf("Error while binding\n");
        exit(0);
    }
    printf("Binding successful\n");
    int temp1 = listen(serverSocket, MAXPENDING);
    if (temp1 < 0)
    {
        printf("Error in listen");
        exit(0);
    }

    printf("Now Listening\n");
    char msg[BUFFERSIZE];
    int clientLength1 = sizeof(clientAddress1);
    int clientSocket1 = accept(serverSocket, (struct sockaddr*)&clientAddress1, &clientLength1);
    printf("Handling Client %s\n", inet_ntoa(clientAddress1.sin_addr));

    if (clientLength1 < 0) { printf("Error in client socket"); exit(0); }

    int clientLength2 = sizeof(clientAddress2);
    int clientSocket2 = accept(serverSocket, (struct sockaddr*)&clientAddress2, &clientLength2);
    printf("Handling Client %s\n", inet_ntoa(clientAddress2.sin_addr));
    if (clientLength2 < 0) { printf("Error in client socket"); exit(0); }
    int state = 0;
        char const* const fileName = "list.txt";

    FILE* fp = fopen(fileName, "w");
    

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    int t=10;
    int flag1=1;
    int flag2=1;
    int count=0;
    
    while (flag1 && flag2)
    {
        switch (state)
        {
        case 0: { // recieve from client1
            int temp2 = recv(clientSocket1, &rcvpkt1, sizeof(rcvpkt1), 0);
            if(rcvpkt1.sq_no==c1_seqno)continue;
            if(rand()%100<PDR )
            {
                printf("DROP_PKT Seq.No. = %d, Client.No. = %d\n",rcvpkt1.sq_no,rcvpkt1.clientNo+1);
               // printf("Dropping packet from client1 with data:%s\n",rcvpkt1.data);
                break;
            }
            printf("RECV_PKT : Seq.No. = %d, Size = %d Bytes, Data = %s\n",rcvpkt1.sq_no,rcvpkt1.data_size,rcvpkt1.data);
            if(strcmp(rcvpkt1.data,".")==0)
            {
                flag1=0;
                break;
            }
            fprintf(fp," %s ",rcvpkt1.data);
            fprintf(fp,", ");
            
            if (temp2 < 0)
            {
                printf("problem in recieving from client 1");
                exit(0);
            }
            sendpkt1.isAck = 1;
            sendpkt1.clientNo = 0;
            sendpkt1.sq_no = rcvpkt1.sq_no;
            c1_seqno=rcvpkt1.sq_no;
            sendpkt1.size = 0;
            sendpkt1.data_size=0;
           // printf("Sending ack to client1 .....\n");
            send(clientSocket1, &sendpkt1, sizeof(sendpkt1), 0);
            printf("SENT_ACK : Seq.No. = %d, Client.No. = %d\n",sendpkt1.sq_no,sendpkt1.clientNo+1);
            state=1;
            break;
        }
        case 1: // recieve from client2
        {
            int temp3 = recv(clientSocket2, &rcvpkt2, sizeof(rcvpkt2), 0);
              if(rcvpkt2.sq_no==c2_seqno)continue;
            if(rand()%100<PDR )
            {
                printf("DROP_PKT Seq.No. = %d, Client.No. = %d\n",rcvpkt2.sq_no,rcvpkt2.clientNo+1);
               // printf("Dropping packet from client1 with data:%s\n",rcvpkt1.data);
                break;
            }
            printf("RECV_PKT Seq.No. = %d, Size = %d Bytes, Data = %s\n",rcvpkt2.sq_no,rcvpkt2.data_size,rcvpkt2.data);
            if(strcmp(rcvpkt2.data,".")==0)
            {
                flag2=0;
                break;
            }
            fprintf(fp,"%s ",rcvpkt2.data);
            //fprintf(fp,",");
            //fprintf(fp,"\n");
            fflush(fp);
            if (temp3 < 0)
            {
                printf("problem in recieving from client2");
                exit(0);
            }
            sendpkt2.isAck = 1;
            sendpkt2.clientNo = 1;
            sendpkt2.sq_no = rcvpkt2.sq_no;
            c2_seqno=rcvpkt2.sq_no;
            sendpkt2.size = 0;
            sendpkt2.data_size=0;
           // printf("Sending ack to client2 .....\n");
            send(clientSocket2, &sendpkt2, sizeof(sendpkt2), 0);
            printf("SENT_ACK : Seq.No. = %d, Client.No. = %d\n",sendpkt2.sq_no,sendpkt2.clientNo+1);
            state=0;
            break;
        }


        }
    }
    close(clientSocket1);
    close(clientSocket2);
    close(serverSocket);
    fclose(fp);
return 0;
}
