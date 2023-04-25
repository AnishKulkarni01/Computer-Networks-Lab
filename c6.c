
/*
Simple udp client with stop and wait functionality
*/
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
typedef struct packet1
{
    int sq_no;
} ACK_PKT;
typedef struct packet2
{
    int sq_no;
    char data[BUFLEN];
} DATA_PKT;
void die(char *s)
{
    perror(s);
    exit(1);
}

void discardRandom()
{
}
int main(void)
{

    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    DATA_PKT send_pkt;
    ACK_PKT rcv_ack;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("172.24.70.240");
    int state = 0;
    while (1)
    {
        switch (state)
        {
        case 0:
            printf("Enter message 0: "); // wait for sending packet with seq. no. 0
            fgets(send_pkt.data, sizeof(send_pkt), stdin);
            send_pkt.sq_no = 0;
            if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            state = 1;
            struct timeval tv;
            gettimeofday(&tv, NULL);
            time_t start_time = time(NULL); /// start timer
            break;
        case 1: // waiting for ACK 0
            discardRandom();
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(s, &readfds);

            tv.tv_sec = 5; // set the timeout to 5 seconds
            tv.tv_usec = 0;

            if (select(s + 1, &readfds, NULL, NULL, &tv) == 0) // timeout occurred
            {
                printf("Timeout occurred, resending packet with seq. no. %d\n", send_pkt.sq_no);
                if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }
                gettimeofday(&tv, NULL); // restart timer
            }
            else // data received
            {
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if (rcv_ack.sq_no == 0)
                {
                    printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
                    state = 2;
                    break;
                }
                else
                {
                    printf("Received wrong ack seq. no. %d, resending packet with seq. no. %d\n", rcv_ack.sq_no, send_pkt.sq_no);
                    if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                    {
                        die("sendto()");
                    }
                    break;
                }
            }
        case 2:
            printf("Enter message 1: ");
            // wait for sending packet with seq. no. 1
            fgets(send_pkt.data, sizeof(send_pkt), stdin);
            send_pkt.sq_no = 1;
            if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            state = 3;
            time_t start_time_2 = time(NULL); // start the timer for state 3
            break;
        case 3: // waiting for ACK 1
            discardRandom();
            // fd_set
            readfds;
            // struct timeval tv;
            FD_ZERO(&readfds);
            FD_SET(s, &readfds);
            tv.tv_sec = 5; // set the timeout to 5 seconds
            tv.tv_usec = 0;

            int select_retval = select(s + 1, &readfds, NULL, NULL, &tv); // wait for the timeout or the ACK signal
            if (select_retval == -1)
            {
                die("select()");
            }
            else if (select_retval == 0)
            {
                // timeout occured, resend the packet
                printf("Timeout occurred, resending packet with seq. no. %d\n", send_pkt.sq_no);
                if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }
                break;
            }
            else
            {
                // ACK received
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if (rcv_ack.sq_no == 1)
                {
                    printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
                    state = 0;
                    break;
                }
                else
                {
                    printf("Received wrong ack seq. no. %d, resending packet with seq. no. %d\n", rcv_ack.sq_no, send_pkt.sq_no);
                    if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                    {
                        die("sendto()");
                    }
                    break;
                }
            }
        }
    }
    close(s);
    return 0;
}
