#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  // Check command-line argument for protocol port and extract
  // host and port number
  if ( argc < 4 ) {
    printf("Usage: echo-client host port string\n");
    exit(1);
  }

  // Extract host name
  char * host = argv[1];

  // Extract port number
  int port = atoi(argv[2]);

  char * string = argv[3];

  // Initialize socket address structure
  struct  sockaddr_in socketAddress;

  // Clear sockaddr structure
  memset((char *)&socketAddress,0,sizeof(socketAddress));

  // Set family to Internet 
  socketAddress.sin_family = AF_INET;

  // Test for port legal value
  if (port > 0) {
    socketAddress.sin_port = htons((u_short)port);
  }
  else {
    fprintf(stderr,"bad port number %s\n", argv[2]);
    exit(1);
  }

  // Get host table entry for this host
  struct  hostent  *ptrh = gethostbyname(host);
  if ( ptrh == NULL ) {
    fprintf(stderr, "invalid host: %s\n", host);
    perror("gethostbyname");
    exit(1);
  }

  // Copy the host ip address to socket address structure
  memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

  // Get TCP transport protocol entry
  struct  protoent *ptrp = getprotobyname("tcp");
  if ( ptrp == NULL ) {
    fprintf(stderr, "cannot map \"tcp\" to protocol number");
    perror("getprotobyname");
    exit(1);
  }
  
  // Create a tcp socket
  int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sock < 0) {
    fprintf(stderr, "socket creation failed\n");
    perror("socket");
    exit(1);
  }

  // Connect the socket to the specified server
  if (connect(sock, (struct sockaddr *)&socketAddress,
	      sizeof(socketAddress)) < 0) {
    fprintf(stderr,"connect failed\n");
    perror("connect");
    exit(1);
  }

  // Send string to server
  write(sock, string, strlen(string));
  write(sock,"\r\n",2);
  
  // Recieve echo from server
  char buf[sizeof(string)+2];
  int n = recv(sock, buf, sizeof(buf)+2, 0);
  write(1,buf,n);

  // Close the socket.
  close(sock);

  // Terminate the client program gracefully.
  exit(0);
}
