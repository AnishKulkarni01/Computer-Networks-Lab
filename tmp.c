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
     char const* const fileName = "input.txt";

    FILE* fp = fopen(fileName, "r");

    if (!fp) {
        printf("\n Unable to open : %s ", fileName);
        return -1;
    }
char line[BUFLEN];
char c;
while((c=fgetc(fp))!=',')
{
    //printf("%c",c);
   // fflush(stdout);
   strncat(line, &c, 1);
 
}
printf("%s\n",line);
 c=fgetc(fp);
 printf("%c\n",c);
  
close(fp);
    return 0;
}