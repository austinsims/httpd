#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int QueueLength = 5;
void processEchoRequest( int socket );

int
main( int argc, char ** argv )
{
  // Print usage if not enough arguments
  if ( argc < 2 ) {
    fprintf( stderr, "Usage: echo-server <port>\n");
    exit( -1 );
  }
  
  // Get the port from the arguments
  int port = atoi( argv[1] );
  
  // Set the IP address and port for this server
  struct sockaddr_in serverIPAddress; 
  memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons((u_short) port);
  
  // Allocate a socket
  int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
  if ( masterSocket < 0) {
    perror("socket");
    exit( -1 );
  }
   
  // Bind the socket to the IP address and port
  int error = bind( masterSocket,
		    (struct sockaddr *)&serverIPAddress,
		    sizeof(serverIPAddress) );
  if ( error ) {
    perror("bind");
    exit( -1 );
  }
  
  // Put socket in listening mode
  error = listen( masterSocket, QueueLength);
  if ( error ) {
    perror("listen");
    exit( -1 );
  }

  while ( 1 ) {

    // Accept incoming connections
    struct sockaddr_in clientIPAddress;
    int alen = sizeof( clientIPAddress );
    int slaveSocket = accept( masterSocket,
			      (struct sockaddr *)&clientIPAddress,
			      (socklen_t*)&alen);

    // Process request.
    processEchoRequest( slaveSocket );

    // Close socket
    close( slaveSocket );
  }
}

void
processEchoRequest( int fd )
{
  // Buffer used to store the name received from the client
  const int MAX = 1024;
  char str[ MAX + 1 ];
  int strLength = 0;
  int n;

  // Currently character read
  unsigned char newChar;

  // Last character read
  unsigned char lastChar = 0;

  // Read a line until <CR><LF> is found.
  while ( strLength < MAX &&
	  ( n = read( fd, &newChar, sizeof(newChar) ) ) > 0 ) {

    if ( lastChar == '\r' && newChar == '\n' ) {
      // Discard previous <CR> from name
      strLength--;
      break;
    }

    str[ strLength ] = newChar;
    strLength++;

    lastChar = newChar;
  }

  str[ strLength ] = 0;
  write( fd, str, strlen( str ) );

  // Send last newline
  const char* newline ="\r\n";
  write(fd, newline, strlen(newline));
}
