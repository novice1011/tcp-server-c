# TCP server C

TCP server using c language, also equipped with reconnect when client disconnect

This code is only for receiving message from client. Also, this code using blocking read mode

If you want non-blocking read on server side, use:

```c
int connfd = accept4(sockfd, (SA*)&cli, &len, SOCK_NONBLOCK);
```

and this is for the client side

```c
int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
```
