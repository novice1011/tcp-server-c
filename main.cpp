#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> //read write
#include <fcntl.h>


#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void reconnect(int *sockfd, int *connfd){
    struct sockaddr_in servaddr, cli;
    socklen_t len = sizeof(cli);
    // socket create and verification
    close(*sockfd);
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (*sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr)); //create memory

    // assign IP, PORT
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//192.168.0.104
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(*sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    //arguments: sockfd:file descriptor,backlog:argument defines the maximum length to which the queue of
    //pending connections for sockfd may grow
    if ((listen(*sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    // Accept the data packet from client and verification
    *connfd = accept(*sockfd, (SA*)&cli, &len);
    if (*connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");
}

// Function designed for chat between client and server.
int func(int connfd)
{
    char buff[MAX];
    ssize_t x = sizeof (long int);

    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        x = read(connfd, buff, sizeof(buff));   // read the message from client and copy it in buffer
        printf("%s", buff);                     // print buffer which contains the client contents

        if (x<=0){
            printf(".........connection break :(\n\n");
            return -1;
        }

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            return 0;
        }
    }
}

// Driver function
int main()
{
    int sockfd = 0, connfd = 0;
    reconnect(&sockfd,&connfd);

    while (1) {
        if (func(connfd)<0){
            close(sockfd);
            reconnect(&sockfd,&connfd);
        }
        else if (func(connfd)==0) {
            close(sockfd);
        }
    }
}

//code modified from :
//https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/

//reconnect, reuse socket immediately :
//https://stackoverflow.com/questions/5106674/error-address-already-in-use-while-binding-socket-with-address-but-the-port-num

//reconnect procedure:
//https://stackoverflow.com/questions/16211018/how-to-reconnect-the-clients-to-server

//create socket argument:
//http://man7.org/linux/man-pages/man2/socket.2.html#top_of_page

/*
 * INADDR_ANY :
 * https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
 * For a server, you typically want to bind to all interfaces - not just "localhost".
 * If you wish to bind your socket to localhost only
 * the syntax would be my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
 * then call bind(my_socket, (SOCKADDR *) &my_sockaddr, ...).
 */

//NON BLOCKING READ FROM
///https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking
/*
   // client side
   int socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

   // server side - see man page for accept4 under linux
   int socketfd = accept4( ... , SOCK_NONBLOCK);
 */

/*
/*
#include <fcntl.h>
//Returns true on success, or false if there was an error
bool SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef _WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}
 */
