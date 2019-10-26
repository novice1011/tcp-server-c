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
    //1.domain 2.type 3. protocol http://man7.org/linux/man-pages/man2/socket.2.html#top_of_page
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
    /*
     * INADDR_ANY :
     * For a server, you typically want to bind to all interfaces - not just "localhost".
     * If you wish to bind your socket to localhost only
     * the syntax would be my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
     * then call bind(my_socket, (SOCKADDR *) &my_sockaddr, ...).
     */
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
