/* 
 *Aaron Steiner PA2
 *
 * tcpechosrv.c - A concurrent TCP echo server using threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      /* for fgets */
#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

int open_listenfd(int port);
//void echo(int connfd);
void *thread(void *vargp);


void restoreBuf(char* c) 
{ 
    int i; 
    for (i = 0; i < MAXLINE; i++) 
        c[i] = '\0'; 
}

/*read the file and return its contents*/
int rFile(int file, char* sBuf, int s,int* numBytes)
{
    
    *numBytes = read(file,sBuf,s);
    
    if(*numBytes < MAXLINE){
        return 1;
    }
    
    return 0;
    
} 

/*get the file size*/
long int fileSize(char filename[]) 
{ 
    //printf("In fileSize function\n");
    FILE* file = fopen(filename, "r"); 
  
    if (file == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(file, 0L, SEEK_END); 
   
    long int number = ftell(file); 
   
    fclose(file); 
    //printf("Returning: %ld\n",number);
    return number; 
}

int get(int connfd){
    size_t n; 
    char buf[MAXLINE];
    char rString[MAXLINE];
    char Default[14];
    char fstring[MAXLINE];
    char getString[MAXLINE];
    int numBytes = 0;
    //char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n<html><h1>Hello CSCI4273 Course!</h1>"; 
    char* httpmsg;
    n = read(connfd, buf, MAXLINE);
    printf("server received the following request:\n%s\n",buf);
    
    int a = 0;
    int b = 4;
    int buflength = strlen(buf) - 1;
    char filepath[buflength - 4];
    while( a <= buflength - 4){
        filepath[a] = buf[b];
        a++;
        b++;
        }
    strtok(filepath, "\n");


    for(int i=0; i < 14; i++){
        Default[i] = buf[i];
    }

    /*if nothing or a directory is requested show default index.html*/
    if(strcmp(Default, "GET / HTTP/1.1") == 0){
        //printf("Requesting Default webpage\n");
        //int numBytes = 0;
        long int fsize = fileSize("index.html");
        
        int file;
        file = open("index.html", O_RDONLY); 
        rFile(file,fstring,MAXLINE,&numBytes);

        httpmsg="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:";

        char sizeString[4];
        sprintf(sizeString, "%ld", fsize);
        //printf("sizeString %s\n",sizeString);
        strcat(rString, httpmsg);
        //printf("rString%s\n",rString);
        strcat(rString,sizeString);
        //printf("httpmsg after first cat%s\n",rString);
        strcat(rString,fstring);
        //printf("httpmsg after second%s\n",rString);
        strcpy(buf,rString);
        printf("server returning a http message with the following content.\n%s\n",buf);
        write(connfd, buf,sizeof(rString));
        //restoreBuf(buf);
    }

    // else{
    //     /*extract directory*/
    //     char dir[20];
    //     char fname[20];
    //     char c = filepath[1];
    //     int i = 1;
    //     while(c != '/'){
    //         c = filepath[i];
    //         dir[i] = filepath[i];
    //         i++;
    //     }
    //     /*extract filename*/
        
    //     while(c != ' '){
    //         c = filepath[i];
    //         fname[i] = filepath[i];
    //         i++;
    //     }
    //     strcat(dir,fname);
    //     long int fsize = fileSize(dir);
    //     int numBytes = 0;
    //     int file;
    //     file = open(dir, O_RDONLY); 
    //     rFile(file,fstring,MAXLINE,&numBytes); //= "\r\n\r\n<html><h1>Default</h1>";

    //     httpmsg="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:";

    //     char sizeString[4];
    //     sprintf(sizeString, "%ld", fsize);
    //     //printf("sizeString %s\n",sizeString);
    //     strcat(rString, httpmsg);
    //     //printf("rString%s\n",rString);
    //     strcat(rString,sizeString);
    //     //printf("httpmsg after first cat%s\n",rString);
    //     strcat(rString,fstring);
    //     //printf("httpmsg after second%s\n",rString);
    //     strcpy(buf,rString);
    //     printf("server returning a http message with the following content.\n%s\n",buf);
    //     write(connfd, buf,sizeof(rString));
    //     restoreBuf(buf);

        
    // }
}


int main(int argc, char **argv) 
{
    int listenfd, *connfdp, port, clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while (1) {
	   connfdp = malloc(sizeof(int));
	   *connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
	   pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* thread routine */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);

    pthread_detach(pthread_self());
    get(connfd);
    free(vargp);
    //echo(connfd);
    close(connfd);
    return NULL;
}

/*
 * echo - read and echo text lines until client closes connection
 */
// void echo(int connfd) 
// {
//     size_t n; 
//     char buf[MAXLINE]; 
//     char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n<html><h1>Hello CSCI4273 Course!</h1>"; 

//     n = read(connfd, buf, MAXLINE);
//     printf("server received the following request:\n%s\n",buf);
//     strcpy(buf,httpmsg);
//     printf("server returning a http message with the following content.\n%s\n",buf);
//     write(connfd, buf,strlen(httpmsg));
    
// }

/* 
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure 
 */
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */


