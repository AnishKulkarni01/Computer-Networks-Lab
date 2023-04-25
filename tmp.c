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

int main()
{
     char const* const fileName = "output.txt";

    FILE* fp = fopen(fileName, "w");

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
char line[BUFLEN];
char line2[5]="hello";
// while(1)
// {
// if(fgets(line, sizeof(line), fp) != NULL)
// {
// printf("%s %d\n", line,strlen(line));
// }
// else break;
// }
fprintf(fp, line2);
close(fp);
    return 0;
}