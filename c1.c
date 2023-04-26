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
    int c = connect(sock, (struct sockaddr*)&serverAddr, sizeof
    (serverAddr));
    printf("%d\n", c);
    if (c < 0)
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





    return 0;
}