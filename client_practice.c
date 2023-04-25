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
typedef struct packet
{
    int sq_no;
    char data[BUFLEN];
    int size;
    int isAck;
    int isLast;
} PKT;

void die(char* s)
{
    perror(s);
    exit(1);
}
int main() {
    int flag = 0;
    char const* const fileName = "input.txt";

    FILE* fp = fopen(fileName, "r");

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
    char line[BUFLEN];

    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    memset((char*)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");
    PKT sendpkt;
    PKT rcvpkt;
    int state = 0;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    int len = 0;
    while (1)
    {
        if (flag == 1)break;
        switch (state)
        {

        case 0:
            fflush(stdout);
            if (fgets(line, sizeof(line), fp) != NULL)flag = 0;
            else { flag = 1;break; }
            len = strlen(line);
            strcpy(sendpkt.data, line);
            sendpkt.size = len;
            sendpkt.sq_no = 0;
            sendpkt.isAck = 0;
            sendpkt.isLast = 0;
            printf("Sending.....\n"); // wait for sending packet with seq. no. 0

            if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            printf("Sent packet with seq.no. %d,data %s,size %d,isAck %d, isLast %d \n", sendpkt.sq_no, sendpkt.data, sendpkt.size, sendpkt.isAck, sendpkt.isLast);
            state = 1;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            time_t start_time = time(NULL); /// start timer
            break;
        case 1:
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(s, &readfds);

            tv.tv_sec = 5; // set the timeout to 5 seconds
            tv.tv_usec = 0;
            if (select(s + 1, &readfds, NULL, NULL, &tv) == 0) // timeout occurred
            {
                printf("Timeout occurred in state1, resending packet with seq. no. %d\n", sendpkt.sq_no);
                if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
                {
                    die("sendto()");
                }
                gettimeofday(&tv, NULL); // restart timer
            }
            else {
                if (recvfrom(s, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr*)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if (rcvpkt.sq_no == 0 && rcvpkt.isAck == 1)
                {
                    printf("Received ack seq. no. %d\n", rcvpkt.sq_no);
                    state = 2;
                    break;
                }
                else {
                    printf("Received wrong ack seq. no. %d, resending packet with seq. no. %d\n", rcvpkt.sq_no, sendpkt.sq_no);
                    if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
                    {
                        die("sendto()");
                    }
                    break;
                }
            }

            break;

        case 2:
            if (fgets(line, sizeof(line), fp) != NULL)flag = 0;
            else { flag = 1;break; }
            len = strlen(line);
            strcpy(sendpkt.data, line);
            sendpkt.size = len;
            sendpkt.sq_no = 1;
            sendpkt.isAck = 0;
            printf("Sending .....\n"); // wait for sending packet with seq. no. 0

            if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            printf("Sent packet with seq.no. %d,data %s,size %d,isAck %d, isLast %d \n", sendpkt.sq_no, sendpkt.data, sendpkt.size, sendpkt.isAck, sendpkt.isLast);

            state = 3;
            time_t start_time_2 = time(NULL);
            break;
        case 3:
           readfds;
            FD_ZERO(&readfds);
            FD_SET(s, &readfds);

            tv.tv_sec = 5; // set the timeout to 5 seconds
            tv.tv_usec = 0;
            if (select(s + 1, &readfds, NULL, NULL, &tv) == 0) // timeout occurred
            {
                printf("Timeout occurred in state1, resending packet with seq. no. %d\n", sendpkt.sq_no);
                if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
                {
                    die("sendto()");
                }
                gettimeofday(&tv, NULL); // restart timer
            }
            else {
                if (recvfrom(s, &rcvpkt, sizeof(rcvpkt), 0, (struct sockaddr*)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if (rcvpkt.sq_no == 1 && rcvpkt.isAck == 1)
                {
                    printf("Received ack seq. no. %d\n", rcvpkt.sq_no);
                    state = 0;
                    break;
                }
                else {
                    printf("Received wrong ack seq. no. %d, resending packet with seq. no. %d\n", rcvpkt.sq_no, sendpkt.sq_no);
                    if (sendto(s, &sendpkt, sizeof(sendpkt), 0, (struct sockaddr*)&si_other, slen) == -1)
                    {
                        die("sendto()");
                    }
                    break;
                }
            }

            break;

        }


    }


    close(s);
    return 0;
}