#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


//for printing error messages
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen; //socklen_t type, which 
     //is an integer type of width of at least 32 bits;
     char buffer[255]; //message string will be stored here, maximum size string is 255

     struct sockaddr_in serv_addr, cli_addr;
     /*
     The <sys/socket.h> header shall define the sockaddr structure,
       which shall include at least the following members:

           sa_family_t  sa_family  Address family.
           char         sa_data[]  Socket address (variable-length data).

       The sockaddr structure is used to define a socket address which
       is used in the bind(), connect(), getpeername(), getsockname(),
       recvfrom(), and sendto() functions.
     */

     int n;
     //If command line arguments are less than 2, that means
     // port number is not provided
     // structure:- filename portnumber
     // example:- ./server 9898
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n"); //standard error, 
         //is the default file descriptor where a process can write error messages.
         exit(1); //finish the program
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0); 
     //sockfd = socket(int socket_family, int socket_type, int protocol);
     //SOCK_STREAM - TCP , SOCK_DGRAM - UDP
     if (sockfd < 0) 
        error("ERROR opening socket");

    //if sockfd returns -1, then socket creation is failed,
    //otherwise it will return 0
     bzero((char *) &serv_addr, sizeof(serv_addr));
     /*
     void bzero(void *s, size_t n);
     The bzero() function erases the data in the n bytes of the memory
       starting at the location pointed to by s, by writing zeros (bytes
       containing '\0') to that area.
     */
     portno = atoi(argv[1]); //atoi() converts string arguments to integer
     serv_addr.sin_family = AF_INET; //IPv4
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     /*
     INADDR_ANY is a constant, that contain 0 in value . 
     This will used only when you want connect from all active ports 
     you don't care about ip-add . so if you want connect any particular ip 
     you should mention like as my_sockaddress.sin_addr.s_addr = inet_addr("192.168.78.2")
     */
     serv_addr.sin_port = htons(portno); //sin_port field identifies the port to which the application must bind.
     /*The htons()- host to network short, function converts the unsigned short integer 
     hostshort from host byte order to network byte order. */
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

     //The system call bind associates an address to a socket descriptor created by socket.


     listen(sockfd,5); //max number of connections simultaneously, it can handle
     clilen = sizeof(cli_addr); 
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");

     //messages sending
     while(1)
     {
           bzero(buffer,255);
           n = read(newsockfd,buffer,255);
           if (n < 0) error("ERROR reading from socket");
           printf("Client: %s\n",buffer);
          bzero(buffer,255);
          fgets(buffer,255,stdin);
          n = write(newsockfd,buffer,strlen(buffer));
           if (n < 0) error("ERROR writing to socket");
           int i=strncmp("Bye" , buffer, 3);
           if(i == 0)
               break;
     }
     close(newsockfd);
     close(sockfd);
     return 0; 
}





/*References:- 
1.) https://man7.org/linux/man-pages/man0/sys_socket.h.0p.html




*/
