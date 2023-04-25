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
#define PDR 10
typedef struct packet
{
    int sq_no;
    char data[BUFLEN];
    int size;
    int isAck;
    int isLast;
} PKT;

void die(char *s)
{
    perror(s);
    exit(1);
}

int main()
{

    char const* const fileName = "output.txt";

    FILE* fp = fopen(fileName, "w");

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    char line[BUFLEN];
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), pkt_len;
    // char buf[BUFLEN];
    PKT rcvpkt;
    PKT ackpkt;
    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }
    printf("Binding successful...\n");
    fflush(stdout);
    int state = 0;
    srand(time(0));
    while(1)
    {
        switch (state)
        {
        case 0:
        printf("Waiting for packet 0 from sender...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            
            if ((pkt_len = recvfrom(s, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
            {
                die("recvfrom()");
            }
            if(rand()%2)
            {
                printf("Packet 0 dropped\n");
                fflush(stdout);
                break;
            }
            // if(rand()%100 < PDR)
            // {
            //     printf("Packet 0 dropped\n");
            //     fflush(stdout);
            //     break;
            // }
            

            
            //printf("Packet received with seq. no. %d and Packet content is = %s\n", rcvpkt.sq_no, rcvpkt.data);
            
            
                if (rcvpkt.sq_no == 0)
                {
                    printf("Packet received with seq. no. %d and Packet content is = %s\n", rcvpkt.sq_no, rcvpkt.data);
                    ackpkt.sq_no = 0;
                    ackpkt.isAck = 1;
                    pkt_len=sizeof(ackpkt);
                    fprintf(fp,rcvpkt.data);
                    fflush(fp);
                    if (sendto(s, &ackpkt, pkt_len, 0, (struct sockaddr *)&si_other,slen) == -1)
                    {
                        die("sendto()");
                    }
                    printf("Acknowledgement sent for packet 0\n");
                    state = 1;
                    break;
                }
                else{
                    printf("Packet with seq0 expected\n");
                }
            break;
        case 1:
        printf("Waiting for packet 1 from sender...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((pkt_len = recvfrom(s, &rcvpkt, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
            {
                die("recvfrom()");
            }
            //   if(rand()%100 < PDR)
            // {
            //     printf("Packet 1 dropped\n");
            //     fflush(stdout);
            //     break;
            // }
            
                if (rcvpkt.sq_no == 1)
                {
                    printf("Packet received with seq. no. %d and Packet content is = %s\n", rcvpkt.sq_no, rcvpkt.data);
                    ackpkt.sq_no = 1;
                    ackpkt.isAck = 1;
                    fprintf(fp,rcvpkt.data);
                    fflush(fp);
                    if (sendto(s, &ackpkt, pkt_len, 0, (struct sockaddr *)&si_other,slen) == -1)
                    {
                        die("sendto()");
                    }
                    printf("Acknowledgement sent for packet 1\n");
                    state = 0;
                    break;
                }
                else{
                    printf("Packet with seq1 expected");
                }
            break;
        
        default:
            break;
        }
    }
    close(fp);
    close(s);
    return 0;
}