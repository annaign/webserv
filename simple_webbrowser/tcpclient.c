#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>


// standart HTTP port - 80
#define SERVER_PORT 9999

#define MAXLINE 4096
#define SA struct sockaddr

void err_n_die(const char *fmt, ...);



int main(int argc, char **argv)
{
    int     sockfd, n;
    int     sendbytes;
    struct sockaddr_in  servaddr;
    char    sendline[MAXLINE];
    char    recvline[MAXLINE];
	uint16_t server_port;

    if (argc != 3)
        err_n_die("usage: %s <server address port>", argv[0]);

	server_port = atoi(argv[2]);

    // create a socket
    /* AF_INET - Address Family - Internet, SOCK_STREAM - Stream socket (not datagram), 0 == use TCP*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("Error while creating the socket!");

    bzero(&servaddr, sizeof(servaddr)); /* Zero out the address */
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port); /* htons = "host to network, short" */


    // converting a string represent of IP address into binary representation "1.2.3.4" => [1,2,3,4]
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_n_die("inet_pton error for %s ", argv[1]);

    // connect to the server!
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_n_die("connect failed!");

    //We're connected. Prepare the message.
    sprintf(sendline, "GET /error.html HTTP/1.1\r\n\r\n"); /* GET / - command means give a root page */
    sendbytes = strlen(sendline); /* \r\n\r\n -  means end of the request */

    // Send the request -- making sure you send it all
    // This code is a bit tragile, since it bails if only some of the
    // bytes are sent.
    // normally, you would want to retry, unless the return value was -1.

    if (write(sockfd, sendline, sendbytes) != sendbytes)
        err_n_die("write error");

    memset(recvline, 0, MAXLINE);
    // Now read the server's response.
    while ( (n = read(sockfd, recvline, MAXLINE - 1)) > 0)
    {
        printf("%s", recvline); /* print a response into a standrt out */
        memset(recvline, 0, MAXLINE); /* zero receive line */
    }
    if (n < 0)
        err_n_die("read error");

    exit(0); // end succesfully!

}


void err_n_die(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    // any system or library call can set errno, so we need to 
    // save it now
    errno_save = errno;


    // print out the fmt+args to standart out
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // print out error message is errno was set.
    if (errno_save != 0)
    {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save,
        strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    // this is the ..and_die part. Terminate with an error.
    exit(1);
}